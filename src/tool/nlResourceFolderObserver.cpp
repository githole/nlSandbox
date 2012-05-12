#include "main/stdafx.h"
#include "tool/nlResourceFolderObserver.hpp"

#include "tool/nlId.hpp"
#include "shared/mesh/nlMesh.hpp"
#include "shared/d3d11resource/d3d11resource.hpp"
#include "tool/nlModelScripLoader.h"
#include "tool/nlUtil.hpp"
#include "tool/nlCurveSerializer.hpp"
#include "shared/nlLog.hpp"
#include "tool/nlLogComposer.hpp"


using namespace std;

/**/
static std::vector<QByteArray> getFuncList( const QString& path )
{
    std::vector<QByteArray> baFuncList;
    QFile funcListFile( path );
    if( !funcListFile.exists() )
    {
        funcListFile.close();
        /* TODO ログを出力する */
        return baFuncList;
    }
    funcListFile.open(QIODevice::ReadOnly|QIODevice::Text);
    while (!funcListFile.atEnd()) 
    {
        QByteArray ba = funcListFile.readLine();
        if( ba.endsWith("\n") ){ ba.truncate( ba.length() - 1 ); }
        baFuncList.push_back(ba);
    }
    funcListFile.close();
    return baFuncList;
}

/**/
nlResourceFolderObserver::nlResourceFolderObserver( nlEngineContext* cxt )
    :cxt_(cxt)
{
}
/**/
void nlResourceFolderObserver::initialize(QString dir,QString ext)
{
    /**/
    QDir root(dir);
    /* 全てのファイルをロードする */
    BOOST_FOREACH(const QFileInfo& info , root.entryInfoList() )
    {
        if( info.isFile() && info.suffix() == ext )
        {
            fileList_.push_back(info);
            onLoad( info );
        }
    }
    /* 一定時間ごとに全体を監視する */
    connect( &timer_, SIGNAL(timeout()), this, SLOT(cheackAllFile()) );
    timer_.start( 500 );
}
/**/
nlResourceFolderObserver::~nlResourceFolderObserver()
{
    timer_.stop();
}
/**/
void nlResourceFolderObserver::cheackAllFile()
{
    /**/
    bool existChange = false;
    /**/
    for( int i=0;i<fileList_.size();++i)
    {
        QFileInfo& info = fileList_[i];
        QFileInfo curInfo( info.absoluteFilePath() );
        if( curInfo.lastModified() != info.lastModified() )
        {
            info = curInfo;
            QByteArray baFileName = info.fileName().toLocal8Bit();
            NL_LOG( INF_006, baFileName.data() );
            onLoad( info );
            existChange = true;
        }
    }
    /**/
    if( existChange ) 
    {
        emit(onChangeContent());
    }
}

#if 0
/**/
TSFolderObserver::TSFolderObserver( nlEngineContext* cxt )
:nlResourceFolderObserver(cxt),
groupName_( "TS_NAME" )
{
    /**/
    clearId(groupName_);
    /**/
    for( int i=0;i<cxt_->pss.size();++i)
    {
        cxt->texs[i].texture = NULL;
        cxt->texs[i].rtView = NULL;
        cxt->texs[i].shaderView = NULL;
    }
    /**/
    initialize(sandboxPath( SP_SHADER ),"hlsl");
    /**/
    setId(groupName_,"",-1);
}
/**/
TSFolderObserver::~TSFolderObserver()
{
    /* 全てのシェーダを解放 */
    for( int i=0;i<MAX_SHADER_NUM;++i )
    {
        NL_SAFE_RELEASE( cxt_->texs[i].texture );
        NL_SAFE_RELEASE( cxt_->texs[i].rtView );
        NL_SAFE_RELEASE( cxt_->texs[i].shaderView );
    }
    /* 全てのスクリプトの解放 */
    ShaderScripts& shaderScripts = cxt_->shaderScripts;
    ShaderScriptGroup& tsScripts = shaderScripts.tsScripts;
    for( int pi=0;pi<tsScripts.numScript;++pi)
    {
        ShaderScript& script = tsScripts.scripts[pi];
        if( script.name ){ free(script.name); }
        if( script.script ){ free(script.script); }
    }
}
/**/
void TSFolderObserver::onLoad( const QFileInfo& path )
{
    /**/
    QFile scriptFile( path.absoluteFilePath() );
    if( !scriptFile.exists() )
    {
        /* TODO ログ */
        return ;
    }
    /* ロードするインデックスを判定 */
    ShaderScripts& shaderScripts = cxt_->shaderScripts;
    ShaderScriptGroup& tsScripts = shaderScripts.tsScripts;
    int ti;
    for( ti=0;ti<tsScripts.numScript;++ti)
    {
        const ShaderScriptGroup& scriptGroup = tsScripts;
        const ShaderScript& script = scriptGroup.scripts[ti];
        QByteArray baBaseName = path.baseName().toLocal8Bit();
        if( !strcmp( baBaseName.data(), script.name ) ) 
        {
            NL_SAFE_RELEASE( cxt_->pss[ti].shader_ );
            break;
        }
    }
    /**/
    ShaderScript& script = tsScripts.scripts[ti];
    /* ロードが初の場合 */
    if( ti == tsScripts.numScript )
    {
        /* 総ロード数を増やす */
        ++tsScripts.numScript;
        /* IDの再登録 */
        setId( groupName_, path.baseName() );
    }
    /* リロードになる場合 */
    else
    {
        /* スクリプト文字列の解放 */
        if( script.name )
        {
            free( script.name ); 
            script.name = NULL;
        }
        if( script.script )
        {
            free( script.script ); 
            script.script = NULL;
        }
    }
    /**/
    scriptFile.open(QIODevice::ReadOnly|QIODevice::Text);
    QByteArray scriptData = scriptFile.readAll();
    script.script = (nlInt8*)calloc((scriptData.size()+1)*sizeof(nlInt8),1);
    strcpy( script.script, scriptData.data() );
    QByteArray baBaseName = path.baseName().toLocal8Bit();
    script.name = (nlInt8*)calloc((baBaseName.size()+1)*sizeof(nlInt8),1);
    strcpy( script.name, baBaseName.data() );
    /* テクスチャの作成 */
    ntGenerateTextures( shaderScripts, ti, cxt_ );
}
#endif
/**/
MeshFolderObserver::MeshFolderObserver(nlEngineContext* cxt)
:nlResourceFolderObserver(cxt)
{
    initialize(sandboxPath(SP_MESH),"kmd");
}
/**/
MeshFolderObserver::~MeshFolderObserver()
{
}
/**/
void MeshFolderObserver::onLoad( const QFileInfo& path )
{
    /**/
    QFile meshFile(path.absoluteFilePath());
    if( !meshFile.exists() )
    { return ; }
    if( !meshFile.open(QIODevice::ReadOnly) )
    { return ; }
    /* IDを取得 */
    boost::optional<int> id = getId( "MESH_NAME", path.baseName() );
    if( !id ) 
    {
        setId( "MESH_NAME", path.baseName() );
        id = getId( "MESH_NAME", path.baseName() );
    }
    /* 各当するメッシュを解放する。 */
    nlMesh& mesh = cxt_->models[*id];
    NL_SAFE_RELEASE( mesh.indices_ );
    NL_SAFE_RELEASE( mesh.vretexes_ );
    mesh.numIndex_ = 0;
    /* 各当するメッシュをロードする */    
    QByteArray meshData = meshFile.readAll();
    nlMeshLoad( meshData.data(), meshData.size(), &mesh, cxt_ );
}

/**/
MusicFolderObserver::MusicFolderObserver(nlEngineContext* cxt)
:nlResourceFolderObserver(cxt)
{
    initialize(sandboxPath(SP_MUSIC),".xm");
}
/**/
MusicFolderObserver::~MusicFolderObserver()
{
}
/**/
void MusicFolderObserver::onLoad( const QFileInfo& path )
{
    QByteArray ba = path.absoluteFilePath().toLocal8Bit();
    //printf( "%s\n",ba.data());
    /* TODO 実際にリソースをロードする */
}

/**/
RTFolderObserver::RTFolderObserver(nlEngineContext* cxt)
:nlResourceFolderObserver(cxt)
{
    setId("RT_NAME","",-1);
    setId("RT_NAME","BackBuffer",0);
    setId("DS_NAME","",0);
    setId("DS_NAME","default",0);
    /**/
    initialize(sandboxPath(SP_RENDERTARGET),"ini");
}
/**/
RTFolderObserver::~RTFolderObserver()
{
}
/**/
void RTFolderObserver::onLoad( const QFileInfo& path )
{
    QByteArray tmpByteArray = path.absoluteFilePath().toLocal8Bit();
    RenderTargetDescs descs = loadRTDesc( path.absoluteFilePath() );

    /* もともとあったRTを削除(0番目はバックバッファであることに注意) */
    for( int i=1;i<cxt_->rendertargets.size();++i)
    { 
        nlRenderTarget& rt = cxt_->rendertargets[i];
        NL_SAFE_RELEASE( rt.renderTarget_ ); 
        NL_SAFE_RELEASE( rt.renderTargetShaderResource_ ); 
        NL_SAFE_RELEASE( rt.renderTargetView_ ); 
    }
    for( int i=1;i<cxt_->depthStencils.size();++i)
    {
        NL_SAFE_RELEASE( cxt_->depthStencils[i].view ); 
        NL_SAFE_RELEASE( cxt_->depthStencils[i].tex ); 
    }
    /* RTの作成 */
    nlCreateRenderTarget(descs,cxt_);
}

/**/
CSFolderObserver::CSFolderObserver(nlEngineContext* cxt)
:nlResourceFolderObserver(cxt)
{
    initialize(sandboxPath(SP_SHADER),"cs");
}
/**/
CSFolderObserver::~CSFolderObserver()
{
}
/**/
void CSFolderObserver::onLoad( const QFileInfo& path )
{
    QByteArray ba = path.absoluteFilePath().toLocal8Bit();
    //printf( "%s\n",ba.data());
    /* TODO 実際にリソースをロードする */
}

/**/
GSFolderObserver::GSFolderObserver(nlEngineContext* cxt)
:nlResourceFolderObserver(cxt),
groupName_("GS_NAME")
{
    initialize(sandboxPath(SP_SHADER),"geom");
    setId("GS_NAME","",-1);
}
/**/
GSFolderObserver::~GSFolderObserver()
{
}
/**/
void GSFolderObserver::onLoad( const QFileInfo& path )
{
    QByteArray ba = path.absoluteFilePath().toLocal8Bit();
    //printf( "%s\n",ba.data());
    /* TODO 実際にリソースをロードする */
    /**/
    QFile scriptFile( path.absoluteFilePath() );
    if( !scriptFile.exists() )
    {
        /* TODO ログ */
        return ;
    }
    /* ロードするインデックスを判定 */
    ShaderScripts& shaderScripts = cxt_->shaderScripts;
    ShaderScriptGroup& gsScripts = shaderScripts.gsScripts;
    int gi;
    for( gi=0;gi<gsScripts.numScript;++gi)
    {
        const ShaderScriptGroup& scriptGroup = gsScripts;
        const ShaderScript& script = scriptGroup.scripts[gi];
        QByteArray baBaseName = path.baseName().toLocal8Bit();
        if( !strcmp( baBaseName.data(), script.name ) ) 
        {
            NL_SAFE_RELEASE( cxt_->gss[gi].shader );
            break;
        }
    }
    /**/
    ShaderScript& script = gsScripts.scripts[gi];
    /* ロードが初の場合 */
    if( gi == gsScripts.numScript )
    {
        /* 総ロード数を増やす */
        ++gsScripts.numScript;
        /* IDの再登録 */
        setId( groupName_, path.baseName() );
    }
    /* リロードになる場合 */
    else
    {
        /* スクリプト文字列の解放 */
        if( script.name )
        {
            free( script.name ); 
            script.name = NULL;
        }
        if( script.script )
        {
            free( script.script ); 
            script.script = NULL;
        }
    }
    /**/
    scriptFile.open(QIODevice::ReadOnly|QIODevice::Text);
    QByteArray scriptData = scriptFile.readAll();
    script.script = (nlInt8*)calloc((scriptData.size()+1)*sizeof(nlInt8),1);
    strcpy( script.script, scriptData.data() );
    QByteArray baBaseName = path.baseName().toLocal8Bit();
    script.name = (nlInt8*)calloc((baBaseName.size()+1)*sizeof(nlInt8),1);
    strcpy( script.name, baBaseName.data() );
    /* ターゲットをロードする */
    nlCreateShaders( ShaderType_GS, gi, shaderScripts, *cxt_ );
}

/**/
PSFolderObserver::PSFolderObserver( nlEngineContext* cxt )
:nlResourceFolderObserver(cxt),
groupName_( "PS_NAME" )
{
    /**/
    clearId(groupName_);
    /**/
    for( int i=0;i<cxt_->pss.size();++i)
    {
        cxt->pss[i].shader_ = NULL;
    }
    /**/
    initialize(sandboxPath( SP_SHADER ),"psh");
    /**/
    setId(groupName_,"",-1);
}
/**/
PSFolderObserver::~PSFolderObserver()
{
    /* 全てのシェーダを解放 */
    for( int i=0;i<MAX_SHADER_NUM;++i )
    {
        NL_SAFE_RELEASE( cxt_->pss[i].shader_ );
    }
    /* 全てのスクリプトの解放 */
    ShaderScripts& shaderScripts = cxt_->shaderScripts;
    ShaderScriptGroup& psScripts = shaderScripts.psScripts;
    for( int pi=0;pi<psScripts.numScript;++pi)
    {
        ShaderScript& script = psScripts.scripts[pi];
        if( script.name ){ free(script.name); }
        if( script.script ){ free(script.script); }
    }
}
/**/
void PSFolderObserver::onLoad( const QFileInfo& path )
{
    /**/
    QFile scriptFile( path.absoluteFilePath() );
    if( !scriptFile.exists() )
    {
        /* TODO ログ */
        return ;
    }

    /* ロードするインデックスを判定 */
    ShaderScripts& shaderScripts = cxt_->shaderScripts;
    ShaderScriptGroup& psScripts = shaderScripts.psScripts;
    int pi;
    for( pi=0;pi<psScripts.numScript;++pi)
    {
        const ShaderScriptGroup& scriptGroup = psScripts;
        const ShaderScript& script = scriptGroup.scripts[pi];
        QByteArray baBaseName = path.baseName().toLocal8Bit();
        if( !strcmp( baBaseName.data(), script.name ) ) 
        {
            NL_SAFE_RELEASE( cxt_->pss[pi].shader_ );
            break;
        }
    }
    /**/
    ShaderScript& script = psScripts.scripts[pi];
    /* ロードが初の場合 */
    if( pi == psScripts.numScript )
    {
        /* 総ロード数を増やす */
        ++psScripts.numScript;
        /* IDの再登録 */
        setId( groupName_, path.baseName() );
    }
    /* リロードになる場合 */
    else
    {
        /* スクリプト文字列の解放 */
        if( script.name )
        {
            free( script.name ); 
            script.name = NULL;
        }
        if( script.script )
        {
            free( script.script ); 
            script.script = NULL;
        }
    }
    /**/
    scriptFile.open(QIODevice::ReadOnly|QIODevice::Text);
    QByteArray scriptData = scriptFile.readAll();
    script.script = (nlInt8*)calloc((scriptData.size()+1)*sizeof(nlInt8),1);
    strcpy( script.script, scriptData.data() );
    QByteArray baBaseName = path.baseName().toLocal8Bit();
    script.name = (nlInt8*)calloc((baBaseName.size()+1)*sizeof(nlInt8),1);
    strcpy( script.name, baBaseName.data() );
    /* ターゲットをロードする */
    nlCreateShaders( ShaderType_PS, pi, shaderScripts, *cxt_ );
}
/**/
VSFolderObserver::VSFolderObserver(nlEngineContext* cxt)
:nlResourceFolderObserver(cxt),
groupName_( "VS_NAME" )
{
    /**/
    clearId(groupName_);
    /**/
    for( int i=0;i<cxt_->vss.size();++i)
    {
        cxt_->vss[i].shader_ = NULL;
    }
    initialize(sandboxPath(SP_SHADER),"vsh");
    /**/
    setId(groupName_,"",-1);
}
/**/
VSFolderObserver::~VSFolderObserver()
{
    /* 全てのシェーダを解放 */
    for( int i=0;i<cxt_->vss.size();++i )
    {
        NL_SAFE_RELEASE( cxt_->vss[i].shader_ );
    }
    /* 全てのスクリプトの解放 */
    ShaderScripts& shaderScripts = cxt_->shaderScripts;
    ShaderScriptGroup& vsScripts = shaderScripts.vsScripts;
    for( int vi=0;vi<vsScripts.numScript;++vi)
    {
        ShaderScript& script = vsScripts.scripts[vi];
        if( script.name ){ free(script.name); }
        if( script.script ){ free(script.script); }
    }
}
/**/
void VSFolderObserver::onLoad( const QFileInfo& path )
{
    /**/
    volatile QString hoge = path.absoluteFilePath();
    QFile scriptFile( path.absoluteFilePath() );
    if( !scriptFile.exists() )
    {
        /* TODO ログ */
        return ;
    }
    /* ロードするインデックスを判定 */
    ShaderScripts& shaderScripts = cxt_->shaderScripts;
    ShaderScriptGroup& vsScripts = shaderScripts.vsScripts;
    int vi;
    for( vi=0;vi<vsScripts.numScript;++vi)
    {
        const ShaderScriptGroup& scriptGroup = vsScripts;
        const ShaderScript& script = scriptGroup.scripts[vi];
        QByteArray baBaseName = path.baseName().toLocal8Bit();
        if( !strcmp( baBaseName.data(), script.name ) ) 
        {
            NL_SAFE_RELEASE( cxt_->vss[vi].shader_ );
            NL_SAFE_RELEASE( cxt_->vss[vi].inputLayout_ );
            break;
        }
    }
    /**/
    ShaderScript& script = vsScripts.scripts[vi];
    /* ロードが初の場合 */
    if( vi == vsScripts.numScript )
    {
        /* 総ロード数を増やす */
        ++vsScripts.numScript;
        /* IDの再登録 */
        setId( groupName_, path.baseName() );
    }
    /* リロードになる場合 */
    else
    {
        /* スクリプト文字列の解放 */
        if( script.name )
        {
            free( script.name ); 
            script.name = NULL;
        }
        if( script.script )
        {
            free( script.script ); 
            script.script = NULL;
        }
    }
    /**/
    scriptFile.open(QIODevice::ReadOnly|QIODevice::Text);
    QByteArray scriptData = scriptFile.readAll();
    script.script = (nlInt8*)calloc((scriptData.size()+1)*sizeof(nlInt8),1);
    strcpy( script.script, scriptData.data() );
    QByteArray baBaseName = path.baseName().toLocal8Bit();
    script.name = (nlInt8*)calloc((baBaseName.size()+1)*sizeof(nlInt8),1);
    strcpy( script.name, baBaseName.data() );
    /* ターゲットをロードする */
    nlCreateShaders( ShaderType_VS, vi, shaderScripts, *cxt_ );
}

/**/
CommonShaderObserver::CommonShaderObserver(struct nlEngineContext* cxt)
    :nlResourceFolderObserver(cxt)
{
    /**/
    initialize(sandboxPath( SP_SHADER ),"fx");
}
CommonShaderObserver::~CommonShaderObserver()
{
    /* TODO 全てのスクリプトの解放 */
}
void CommonShaderObserver::onLoad( const QFileInfo& path )
{
    /**/
    QFile scriptFile( path.absoluteFilePath() );
    if( !scriptFile.exists() )
    {
        /* TODO ログ */
        return ;
    }
    /* ロードするインデックスを判定 */
    ShaderScript& script = cxt_->shaderScripts.commonScripts;
    /* リロードになる場合 */
    if( script.script )
    {
        /* スクリプト文字列の解放 */
        if( script.name )
        {
            free( script.name ); 
            script.name = NULL;
        }
        if( script.script )
        {
            free( script.script ); 
            script.script = NULL;
        }
    }
    /**/
    scriptFile.open(QIODevice::ReadOnly|QIODevice::Text);
    QByteArray scriptData = scriptFile.readAll();
    script.script = (nlInt8*)calloc((scriptData.size()+1)*sizeof(nlInt8),1);
    strcpy( script.script, scriptData.data() );
    QByteArray baBaseName = path.baseName().toLocal8Bit();
    script.name = (nlInt8*)calloc((baBaseName.size()+1)*sizeof(nlInt8),1);
    strcpy( script.name, baBaseName.data() );

#if 0
    /* 全シェーダをロードしなおす */
    for( int i=0;i<cxt_->vss.size();++i)
    { 
        NL_SAFE_RELEASE( cxt_->vss[i].shader_ ); 
        NL_SAFE_RELEASE( cxt_->vss[i].inputLayout_ ); 
    }
    for( int i=0;i<cxt_->pss.size();++i)
    { 
        NL_SAFE_RELEASE( cxt_->pss[i].shader_ ); 
    }
    nlCreateShaders( ShaderType_NONE, -1, shaderScripts, *cxt_ );
#endif
}

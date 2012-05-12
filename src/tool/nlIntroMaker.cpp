#include "main/stdafx.h"
#include "tool/nlIntroMaker.hpp"
#include "shared/scene/nlScene.hpp"
#include "shared/d3d11resource/d3d11resource.hpp"
#include "tool/nlResourceFolderObserver.hpp"
#include "tool/nlUtil.hpp"

#pragma warning(disable:4505)

using namespace std;
using namespace boost;

/**/
static bool prepareExportHeader();
static bool exportPath(nlEngineContext*cxt);
static bool exportModelHeader();
static bool exportScene( const nlArray<nlScene,MAX_SCENE_NUM>& scenes );
static bool exportShaders( nlEngineContext* cxt  );
static bool exportSound( QString path );
static bool exportRT();
static bool exportConfing();
static bool Build();
static bool compress();
static bool showResult();
const char* dataPath = "../src/generated/intro_data.h";
/**/
void nlIntroMaker::make()
{
    /**/
    if( MessageBoxA(NULL,"Makeを開始しますか？","nlSandbox",MB_TASKMODAL|MB_YESNO) == IDNO )
    { return; }
    /**/
    if( !makeMain() )
    {puts("failed build");}
    else
    {puts("finished build");}
}
/**/
bool nlIntroMaker::makeMain()
{
    /**/
    puts("remove data path");
    remove(dataPath);
    puts("prepareExportHeader");
    if( !prepareExportHeader() )
    {return false;}
    puts("exportPath");
    if( !exportPath(cxt_) )
    {return false;}
    puts("exportModelHeader");
    if( !exportModelHeader() )
    {return false;}
    puts("exportScene");
    if( !exportScene(scenes_) )
    {return false;}
    puts("exportShaders");
    if( !exportShaders(cxt_) )
    {return false;}
    puts("exportSound");
    if( !exportSound( musicPath_ ) )
    {return false;}
    puts("exportRT");
    if( !exportRT() )
    {return false;}
    puts("exportConfing");
    if( !exportConfing() )
    {return false;}
    puts("Build");
    if( !Build() ) 
    {return false;}
    puts("Compress");
    if( !compress() )
    { return false; }
    puts("show result");
    if( !showResult() )
    { return false; }
    /**/
    return true;
}
/**/
static bool prepareExportHeader()
{
    /* ヘッダ部分を出力 */
    QFile dataFile(dataPath);
    dataFile.open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Append);
    dataFile.write("#pragma once\n" );
    dataFile.write("\n" );
    dataFile.write("#pragma warning( push )\n" );
    dataFile.write("#pragma warning ( disable : 4245 ) /* intからunsigned intへの変換 */\n" );
    dataFile.write("\n" );

    return true;
}
/**/
static bool exportPath(nlEngineContext* cxt )
{
    QFile dataFile(dataPath);
    dataFile.open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Append);
    /* nlPaths2 */
    dataFile.write("struct nlPaths2\n" );
    dataFile.write("{\n" );
    dataFile.write("\t/* pathe */\n" );
    dataFile.write("\tstruct\n" );
    dataFile.write("\t{\n" );
    dataFile.write("\t\tint type_;/* 0:線形 1:Catmul */\n" );
    dataFile.write("\t\tint len_;\n" );
    dataFile.write("\t\tstruct\n" );
    dataFile.write("\t\t{\n" );
    dataFile.write("\t\t\tunsigned int frame_;\n" );
    dataFile.write("\t\t\tfloat        xyz_[3];\n" );
    QString line; 
    line.sprintf("\t\t}points_[%d];\n", MAX_PATHPOINT_NUM);
    dataFile.write( line.toLatin1() );
    line.sprintf("\t}pathes[%d];\n", MAX_PATH_NUM );
    dataFile.write( line.toLatin1() );
    dataFile.write("};\n" );
    dataFile.write("\n" );

    /* 形式を合わせて出力 */
    dataFile.write("const nlPaths2 g_64kPathes =\n" );
    dataFile.write("{\n" );
    for( int i=0;i<MAX_PATH_NUM;++i)
    {
        const nlCurve& path         = cxt->paths[i];
        const nlCurvePoints& points = path.points_;
        const int pathType = path.type_;
        const int pathLen  = points.len_;
        /* pathのtypeと総延長 */
        QString line;
        line.sprintf("\t%d,%d,", pathType, pathLen );
        dataFile.write(line.toLatin1());
        /* pathの構成ポイントを出力 */
        dataFile.write("{\n");
        for( int j=0;j<pathLen;++j)
        {
            QString line;
            line.sprintf(
                "\t\t%d,%f,%f,%f, \n", 
                points[j].time_, 
                f16b(points[j].xyz_[0]),
                f16b(points[j].xyz_[1]),
                f16b(points[j].xyz_[2]) );
            dataFile.write( line.toLatin1() );
        }
        dataFile.write( "\t},\n"  );
    }
    dataFile.write( "};\n" );
    /**/
    return true;
}
/**/
static bool exportSound( QString path )
{
    /**/
    QFile musicFile(path);
    musicFile.open(QIODevice::ReadOnly);
    QByteArray buffer = musicFile.readAll();
    /* データを書き出す */
    QFile dataFile(dataPath);
    dataFile.open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Append);
    dataFile.write("const nlUint8 soundData[] = \n");
    dataFile.write("{\n\t");
    for( int i=0;i<buffer.size();++i)
    {
        QString lineData;
        unsigned int ch = (nlUint8)buffer[i];
        lineData.sprintf("0x%02x,",ch);
        dataFile.write(lineData.toLatin1());
        if(!((i+1)%50)){ dataFile.write("\n\t"); }
    }
    dataFile.write("\n};\n");
    /**/
    return true;
}
/**/
static bool exportRT()
{
    /* RTの設定を取得する */
    RenderTargetDescs descs = loadRTDesc( sandboxPath(SP_RENDERTARGET)+QString("rt.ini") );
    /* データを書き出す */
    QString lineData;
    QFile dataFile(dataPath);
    dataFile.open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Append);
    dataFile.write("/* RenderTarget */\n");
    dataFile.write("static const RenderTargetDescs g_rtDescs =\n");
    dataFile.write("{\n");
    for( int i=0;i<MAX_RENDERTARGET_NUM;++i)
    {
        lineData.sprintf("\t%d,",descs.rtDescs[i].scaleByBackbuffer);
        dataFile.write(lineData.toLocal8Bit());
        lineData.sprintf("(DXGI_FORMAT)%d,\n",descs.rtDescs[i].format);
        dataFile.write(lineData.toLocal8Bit());
    }
    lineData.sprintf("\t%d,\n",descs.numRT );
    dataFile.write(lineData.toLocal8Bit());
    /**/
    for( int i=0;i<MAX_RENDERTARGET_NUM;++i)
    {
        QString lineData;
        lineData.sprintf("\t%d,",descs.dsDescs[i].scaleByBackbuffer);
        dataFile.write(lineData.toLocal8Bit());
        lineData.sprintf("(DXGI_FORMAT)%d,\n",descs.dsDescs[i].format);
        dataFile.write(lineData.toLocal8Bit());
    }
    lineData.sprintf("\t%d,\n",descs.numDS );

    dataFile.write(lineData.toLocal8Bit());
    dataFile.write("};\n");
    /**/
    return true;
}
/**/
static bool exportConfing()
{
    /**/
    return true;
}
/**/
static bool exportModelHeader()
{
    /* 全てのkmdファイルをロードする。これのロード順とnlEngineContext内のモデルの順番が同じであることを仮定している */
    QFileInfoList fileList;
    std::vector<QByteArray> fileDatas;
    QDir root(sandboxPath(SP_MESH));
    /* 全てのファイルをロードする */
    int maxFileSize = 0;
    BOOST_FOREACH(const QFileInfo& info , root.entryInfoList() )
    {
        if( info.isFile() && info.suffix() == "kmd" )
        {
            QFile file( info.absoluteFilePath() );
            file.open( QIODevice::ReadOnly );
            QByteArray data = file.readAll();
            maxFileSize = MX( data.size(), maxFileSize );
            fileDatas.push_back( data );
        }
    }
    NL_ASSERT( fileDatas.size() < MAX_MESH_NUM );
    //fileDatas.resize(MAX_MESH_NUM);
    /**/
    QString lineData;
    QFile dataFile(dataPath);
    dataFile.open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Append);
    dataFile.write("/* Model */\n");
    dataFile.write("struct\n");
    dataFile.write("{\n");
    dataFile.write("    unsigned int size;\n");
    lineData.sprintf("    nlUint8 data[%d];\n", maxFileSize+1 );
    dataFile.write(lineData.toLocal8Bit());
    dataFile.write("}g_models[] =\n");
    dataFile.write("{\n");
    for( int i=0;i<fileDatas.size();++i)
    {
        const QByteArray& data =  fileDatas[i];
        lineData.sprintf("\t/* %d */\n", i );
        dataFile.write( lineData.toLocal8Bit() );
        lineData.sprintf("\t{\n", i );
        dataFile.write( lineData.toLocal8Bit() );
        lineData.sprintf("\t\t%d,\n\t\t", data.size() );
        dataFile.write( lineData.toLocal8Bit() );
        for( int j=0;j<data.size();++j)
        {
            nlUint8 ch = data[j];
            lineData.sprintf("0x%02x,", ch );
            dataFile.write(lineData.toLocal8Bit());
            if( !((j+1)%20) ){ dataFile.write("\n\t\t"); }
        }
        dataFile.write("\n\t},\n");
    }
    dataFile.write("};\n");
    /**/
    return true;
}
/**/
static bool exportScene( const nlArray<nlScene,MAX_SCENE_NUM>& scenes )
{
    /**/
    QString line;
    QFile dataFile(dataPath);
    dataFile.open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Append);
    /* 全てのシーンのシーンマップをエクスポートする。 */
    dataFile.write( "static const SceneArray scenes = \n" );
    dataFile.write( "{\n" );
    for( int i=0;i<MAX_SCENE_NUM;++i)
    {
        const nlScene& scene = scenes[i];
        /* 実際にシーンを出力  */
        line.sprintf( "\t/* scene%d  */\n", i );
        dataFile.write(line.toLatin1() );
        line.sprintf("\t%d, /* sceneTimeInMs */ \n", scene.sceneTimeInMs );
        dataFile.write( line.toLatin1() );
        line.sprintf("\t%d, /* num scenemap */ \n", scene.numSceneMap_ );
        dataFile.write( line.toLatin1() );
        /**/
        //dataFile.write( "\t\t{\n" );
        for( int j=0;j<MAX_SCENENODE_NUM;++j)
        {
            nlSceneNode node = {};
            if( j < scene.sceneMaps_.size() )
            {
                node = scene.sceneMaps_[j];
            }
            /**/
            line.sprintf("\t(nlSceneNode_t)%d,", node.type_ );
            dataFile.write(line.toLatin1());
            for( int k=0;k<node.prevNode_.size();++k)
            {
                unsigned int prevIdx = node.prevNode_[k];
                line.sprintf("%d,", prevIdx );
                dataFile.write(line.toLatin1());
            }
            for( int k=0;k<node.prop_.size();++k)
            {
                float prop = node.prop_[k];
                line.sprintf("%ff,", f16b(prop) );
                dataFile.write(line.toLatin1());
            }
            /* バルブ数の出力 */
            line.sprintf( "%d,", node.numValve_ );
            dataFile.write(line.toLatin1());
            /* 出力バルブの型 */
            line.sprintf( "(nlEdgeType)%d,", node.outEdgeType );
            dataFile.write(line.toLatin1());
            /**/
            dataFile.write( "\n" );
        }
    }
    dataFile.write( "};\n" );
    dataFile.write( "\n" );
    /**/
    return true;
}
/**/
static bool exportShaders( nlEngineContext* cxt  )
{
    /* ソースに書き出し */
    class Local
    {
    public:
        /**/
        static void writeShaderGroup( const ShaderScriptGroup& shaderGroup, QByteArray& fileStr, const ShaderScript& commonShader, ShaderType shaderType, nlEngineContext* cxt )
        {
            fileStr.append("   {\n");
            fileStr.append( "       {\n");
            BOOST_FOREACH( const ShaderScript& script, shaderGroup.scripts )
            {
                if(script.name)
                {
                    //printf("start[%s]\n",script.name);
                }

                fileStr.append( "           \""+(QString(script.name)+"\",\"").toLocal8Bit() );
                /*bool show = false;
                if( !strcmp( script.name, "test") )
                {
                    show = true;
                }*/
                QByteArray minifyedShader = minifyShader(script.script);
                fileStr.append( (minifyedShader+"\",\n") );

                /* common shaderと連結してビルドできることを確かめる */
                if(script.name)
                {
                    QByteArray conbinedScript = QByteArray( commonShader.script ) + minifyedShader;
                    switch(shaderType)
                    {
                    case ShaderType_VS:
                        {
                            nlVertexShader vs = nlCreateVertexShader( conbinedScript.data(), conbinedScript.size(), "main", *cxt );
                            NL_SAFE_RELEASE( vs.shader_ );
                            NL_SAFE_RELEASE( vs.inputLayout_ );
                        }
                        break;
                    case ShaderType_PS:
                    case ShaderType_TS:
                        {
                            nlPixelShader ps = nlCreatePixelShader( conbinedScript.data(), conbinedScript.size(), "main", *cxt );
                            NL_SAFE_RELEASE( ps.shader_ );
                        }
                        break;
                    case ShaderType_GS:
                        {
                            /* NOTE 今は特に何もしない */
                        }
                        break;
                    default:
                        NL_ASSERT(!"対応していないシェーダ形式");
                        break;
                    }
                }
            }
            fileStr.append( "       }\n");
            fileStr.append( "       "+(QString(",")+QString::number(shaderGroup.numScript)+QString("\n")).toLocal8Bit() );
            fileStr.append( "   },\n" );
        }
        /**/
        static void writeShaderScript( const ShaderScript& script, QByteArray& fileStr )
        {
            fileStr.append( "           \""+(QString(script.name)+"\",\"").toLocal8Bit() );
            QByteArray minifyedShader = minifyShader(script.script);
            fileStr.append( (minifyedShader+"\",\n") );
        }
        /**/
        static QByteArray minifyShader( QByteArray inputFileBytes )
        {
            /* "*//*"を削除 */
            inputFileBytes.replace("*//*","");

            /* 一行コメントの削除 */
            int cursor = 0;
            while( cursor != -1 )
            {
                int commentStart = inputFileBytes.indexOf( "//", cursor );
                int commentEnd = inputFileBytes.indexOf("\n", commentStart );
                if( commentEnd == -1 ){ break; }
                inputFileBytes.remove( commentStart, commentEnd-commentStart);
                cursor = commentStart;
            }

            /* 複数行コメントの削除 */
            cursor = 0;
            while( cursor != -1 )
            {
                int commentStart = inputFileBytes.indexOf( "/*", cursor );
                int commentEnd = inputFileBytes.indexOf("*/", commentStart );
                /*if( show && commentEnd != -1 )
                {
                    FILE* fp = fopen("./test.txt","a");
                    fprintf(fp,"%d->%d",commentStart,commentEnd);
                    fprintf(fp,"%s",inputFileBytes.data());
                    fclose(fp);
                }*/
                /**/
                /*if( (commentStart == -1) || (commentEnd == -1) )
                {break;}*/
                ///* ネストされた複数行コメント対策 */
                //if( int nextCommentStart = inputFileBytes.indexOf( "/*", commentStart+1 ) < commentEnd )
                //{
                //    commentStart = nextCommentStart;
                //}
                inputFileBytes.remove( commentStart,commentEnd-commentStart+2);
                cursor = commentStart;
            }
            
            /* 改行を削除 */
            inputFileBytes.replace("\n","");
            /**/
            return inputFileBytes;

#if 0
            /* 変換を読み込み */
            std::vector<QList<QByteArray>>replaceList;
            int currentReplaseIndex = 0;
            while( (currentReplaseIndex = inputFileBytes.indexOf("REPLACE",currentReplaseIndex)) != -1 )
            {
                currentReplaseIndex += strlen("REPLACE")+1;
                const int replaceEndIndex = inputFileBytes.indexOf("*/",currentReplaseIndex);
                QByteArray conv = inputFileBytes.mid(currentReplaseIndex,replaceEndIndex-currentReplaseIndex);
                QList<QByteArray> convdef = conv.split(',');
                if( convdef.size() == 2 )
                {
                    printf("replace [%s]->[%s]\n", convdef[0].data(), convdef[1].data() );
                    replaceList.push_back(convdef);
                    
                }
                else
                {
                    printf("broken conv def[%s]\n", conv.data() );
                }
            }
            
            /* "を\"に置換 */
            inputFileBytes.replace("\"","\\\"");
            /* 事前定義されているdefineに変換する */
            inputFileBytes.replace("float4x4","f4x4");
            inputFileBytes.replace("float4","f4");
            inputFileBytes.replace("float3","f3");
            inputFileBytes.replace("float2","f2");
            inputFileBytes.replace("float1","f1");
            inputFileBytes.replace("float","f1");
            /* 空白/タブを削除する  */
            //inputFileBytes.replace(" ","");
            //inputFileBytes.replace("	","");
            /*  */
            inputFileBytes.replace("define","define ");
            inputFileBytes.replace("return","return ");
            inputFileBytes.replace("const","const ");
            inputFileBytes.replace("const","const ");
            /* 変数定義を復元する */
            inputFileBytes.replace("void","void ");
            inputFileBytes.replace("int","int ");
            inputFileBytes.replace("f4x4","f4x4 ");
            inputFileBytes.replace("f4","f4 ");
            inputFileBytes.replace("f4 x4","f4x4");/* f4 x4と離れてしまうのを直す */
            inputFileBytes.replace("f3","f3 ");
            inputFileBytes.replace("f2","f2 ");
            inputFileBytes.replace("f1","f1 ");
            inputFileBytes.replace("double","double ");
            inputFileBytes.replace("out","out ");
            inputFileBytes.replace("cbuffer","cbuffer ");
            inputFileBytes.replace("Texture1D","Texture1D ");
            inputFileBytes.replace("Texture2D","Texture2D ");
            inputFileBytes.replace("Texture3D","Texture3D ");
            inputFileBytes.replace("struct","struct ");
            inputFileBytes.replace("SamplerState","SamplerState ");
            
            /* さらにくっつけてもOKな変数を元に戻す */
            inputFileBytes.replace("int (","int(");
            inputFileBytes.replace("f4 (","f4(");
            inputFileBytes.replace("f3 (","f3(");
            inputFileBytes.replace("f2 (","f2(");
            inputFileBytes.replace("f1 (","f1(");
            inputFileBytes.replace("int (","int(");
            inputFileBytes.replace("double (","double(");
            inputFileBytes.replace("float4x4 (","float4x4(");                      

            /*REPLACE */
            BOOST_FOREACH( QList<QByteArray> replacePair, replaceList )
            {
                inputFileBytes = inputFileBytes.replace( replacePair[0], replacePair[1]);
            }
            printf(inputFileBytes.data());

            /* 文字列置換  */
            //QByteArray convFileBytes  = convFile.readAll();
            //QList<QByteArray> convList = convFileBytes.split('\n');
            //foreach(const QByteArray& conv, convList)
            //{
            //    /* コメント行はスキップ */
            //    if( conv.size() && conv[0] == '#' )
            //    { continue; }
            //    /*  */
            //    QList<QByteArray> convdef = conv.split(',');
            //    if( convdef.size() == 2 )
            //    {
            //        printf("replace [%s]->[%s]\n", convdef[0].data(), convdef[1].data() );
            //        inputFileBytes = inputFileBytes.replace( convdef[0], convdef[1]);
            //    }
            //    else
            //    {
            //        printf("broken conv def[%s]\n", conv.data() );
            //    }
            //}
            /* 空行を削除 */
            /*QList<QByteArray> outputLines = inputFileBytes.split('\n');
            for( int i=0;i<outputLines.size();++i)
            {
                QByteArray& line = outputLines[i];
                if( !line.isEmpty() )
                {
                    outputFile.write( line + "\n" );
                }
            }*/
            return inputFileBytes;
#endif
        }
    };
    /* シェーダ構造体の作成 */
    QByteArray fileStr;
    fileStr.append( "static ShaderScripts g_shaderScripts =\n" );
    fileStr.append( "{\n" );
    /**/
    fileStr.append( "   {\n" );
    const ShaderScripts& scripts = cxt->shaderScripts;
    Local::writeShaderScript( scripts.commonScripts, fileStr );
    fileStr.append( "   },\n" );
    Local::writeShaderGroup( scripts.vsScripts, fileStr, scripts.commonScripts, ShaderType_VS, cxt );
    Local::writeShaderGroup( scripts.psScripts, fileStr, scripts.commonScripts, ShaderType_PS, cxt );
    Local::writeShaderGroup( scripts.gsScripts, fileStr, scripts.commonScripts, ShaderType_GS, cxt );
    Local::writeShaderGroup( scripts.tsScripts, fileStr, scripts.commonScripts, ShaderType_TS, cxt );    
    fileStr.append("};\n");
    /* ファイルに書き込む */
    QFile dataFile(dataPath);
    dataFile.open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Append);
    dataFile.write( fileStr );
    dataFile.close();
    /**/
    return true;
}
/**/
static bool Build()
{
    /* ビルド */
    PROCESS_INFORMATION ps= {};
    STARTUPINFOA si = {};    
    GetStartupInfoA( &si ) ;
    QString vc9Path = configFileValue("config","VC9Path","").toString();
    if( vc9Path == "" )
    { return false; }
    QString cmdLine = vc9Path +  " /r ..\\project\\intro\\intro.vcproj \"Demo|Win32\"";
    QByteArray cmdLineBA = cmdLine.toLocal8Bit();
    const char* cmd = cmdLineBA.data();
    //const char* cmd = "\"C:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\VC\\vcpackages\\vcbuild.exe\" /r ..\\intro\\intro.vcproj \"Demo|Win32\"";
    CreateProcessA(NULL , const_cast<char*>( cmd ), NULL , NULL , FALSE ,  0 , NULL , NULL , &si , &ps) ;
    WaitForSingleObject( ps.hProcess, INFINITE ) ;    
    /**/
    return true;
}
/**/
static bool compress()
{
    QString cmdLine = sandboxPath( SP_APP )+"/kkrunchy_k7  --best --refsize 64 --out intro.exe "+ sandboxPath( SP_APP ) + "/../project/intro/Demo/intro.exe";
    QByteArray baCmdLine = cmdLine.toLocal8Bit();
    puts(baCmdLine.data());
    QProcess minifyProcess;
    minifyProcess.execute(cmdLine);
    minifyProcess.waitForFinished(-1);
    return true;
}
/**/
static bool showResult()
{
    /**/
    QFile nonCompressFile(  sandboxPath( SP_APP ) + "/../project/intro/Demo/intro.exe" );
    nonCompressFile.open(QIODevice::ReadOnly|QIODevice::Text);
    QFile compressFile( sandboxPath( SP_APP ) + "/intro.exe" );
    compressFile.open(QIODevice::ReadOnly);
    /**/
    qint64 size0 = nonCompressFile.size();
    qint64 size = compressFile.size();
    printf( "%lldb((%2.1fkb))->%lldb(%2.1fkb)", nonCompressFile.size(), (float)nonCompressFile.size()/(float)1024, compressFile.size(), (float)compressFile.size()/(float)1024 );
    return true;
}
#include "main/stdafx.h"
#include "nlsandbox.h"
#include "tool/nlCam.h"
#include "shared/nlMemory.h"
#include "shared/sound/nlSound.h"
#include "shared/render/nlRender.h"
#include "shared/d3d11resource/d3d11resource.hpp"
#include "tool/nlLauncher.h"
#include "tool/nlIntroMaker.hpp"
#include "tool/nlResourceFolderObserver.hpp"
#include "tool/nlId.hpp"
#include "tool/nlUtil.hpp"
#include "tool/nlLogComposer.hpp"

using namespace std;
using namespace boost;

namespace
{
    const char* pathXMLFileName = "media/path/path.xml";
}
/**/
Ui::nlSandboxClass ui;
nlEngineContext g_ctx = {};
static void resizePreview( float scale );
/**/
void geneSceneMap(
                  nlScene& scene,
                  ProcessBuffer& processBuffer,
                  const ExportSceneInfo& sceneInfo)
{
    /**/
    nlArray<nlSceneNode,MAX_SCENENODE_NUM>& sceneMap = scene.sceneMaps_;
    unsigned int& numSceneMap = scene.numSceneMap_;
    const NodeExportInfos& nodeExportInfos = sceneInfo.nodeInfo;
    /* sceneMapを手動でクリアする */
    for( int i=0;i<sceneMap.size();++i)
    {
        nlSceneNode& curNode = sceneMap[i];
        curNode.type_ = SNT_INVALID;
    }
    /**/
    numSceneMap = nodeExportInfos.size();
    scene.sceneTimeInMs = sceneInfo.totalTimeInMs;
    /**/
    for( int i=0;i<nodeExportInfos.size();++i )
    {
        const NodeExportInfo& info = nodeExportInfos[i];
        nlSceneNode& curNode = sceneMap[i];
        /* 利用していないなら生成しない */
        if( !info.prop_.isUse_ )
        { continue; }
        /**/
        curNode.type_ =  info.future_.nodeId_;
        for(int j=0;j<SCENENODE_INPUT_NUM;++j)
        {
            curNode.prevNode_[j] = info.prevNode_[j];
        }
        sceneMap[i].numValve_ = info.prop_.numValve_;
        /**/
        switch( info.future_.outEdgeType_ )
        {
        case NL_ET_None:
            sceneMap[i].outEdgeType = NL_ET_None;
            break;
        case NL_ET_Vec4:
            sceneMap[i].outEdgeType = NL_ET_Vec4;
            break;
        case NL_ET_Matrix:
            sceneMap[i].outEdgeType = NL_ET_Matrix;
            break;
        case NL_ET_Render:
            sceneMap[i].outEdgeType = NL_ET_Render;
            break;
        default:
            NL_ASSERT(!"存在しないノードバルブタイプ");
            break;
        }
        /* FIXME プロパティ上限数だけ回すように修正する */
        curNode.prop_[0] = info.prop_.prop_[0];
        curNode.prop_[1] = info.prop_.prop_[1];
        curNode.prop_[2] = info.prop_.prop_[2];
        curNode.prop_[3] = info.prop_.prop_[3];
        curNode.prop_[4] = info.prop_.prop_[4];
        curNode.prop_[5] = info.prop_.prop_[5];
        /* */
        curNode.numValve_;
        curNode.outEdgeType;
        curNode.prevNode_;
        curNode.prop_;
        curNode.type_;
    }
}

void consoleTopMost()
{
    #define MY_BUFSIZE 1024
    char pszOldWindowTitle[MY_BUFSIZE];
    GetConsoleTitleA(pszOldWindowTitle, MY_BUFSIZE);
    const HWND hwd =FindWindowA(NULL, pszOldWindowTitle);
    SetWindowPos( hwd, HWND_TOPMOST, 0, 0, 0, 0, (SWP_SHOWWINDOW|SWP_NOMOVE|SWP_NOSIZE) );
}

/**/
nlSandbox::nlSandbox(
                     QWidget *parent, Qt::WFlags flags)
                     :QMainWindow(parent, flags),
                     isRenderDebugLine_( false )
{
    /* バックバッファに関する設定を取得する */
    QString configPath = sandboxPath(SP_APP)+"config.ini";
    const int w = configFileValue( "config", "BACKBUFFER_WIDTH" ).toInt();
    const int h = configFileValue( "config", "BACKBUFFER_HEIGHT" ).toInt();
    g_ctx.rendertargets[0].width_  = w; 
    g_ctx.rendertargets[0].height_ = h;
    /**/
    launcher_   = make_shared<nlLauncher>();
    /**/
    consoleTopMost();

    /**/
}
/**/
void nlSandbox::initialize()
{
    initializeAllSandbox_();
    /**/
    logInit(&ui);
    /* GUI配置リストア */
    const QString settingIniPath = sandboxPath(SP_APP)+QString("windowsetting.ini");
    QSettings setting(settingIniPath, QSettings::IniFormat);
    restoreGeometry(setting.value("geometry").toByteArray());
    restoreState(setting.value("windowState").toByteArray());

    /* プレビューウィンドウの位置の初期化 */
    ui.previewDock->setGeometry( setting.value("preview_geom").toRect() );
    /* プレビューウィンドウのサイズ設定 */
    const float scale = setting.value("previewScale").toFloat();
    resizePreview(scale);
}
/**/
void nlSandbox::initializeAllSandbox_()
{
    /* シーンマップ用のメモリを確保 */
    nlScene_init();	
    /**/
    /* 基本UIの構築 */
    ui.setupUi(this);
    setWindowTitle( WINDOW_TITLE );
    ui.dockTimeline->initialize( &ui );
    ui.dockSceneEdit->initialize( &ui  );
    ui.pathWidget->initialize( &ui, &g_ctx );
    /**/
    const bool isFullScreen = false;
    nlInitD3D11( ui.previewWidget->winId(), isFullScreen, g_ctx );
    /* 各リソースマネージャを初期化してロード開始 */
    meshFolderObserver_  = make_shared<MeshFolderObserver>(&g_ctx);
    musicFolderObserver_ = make_shared<MusicFolderObserver>(&g_ctx);
    rtFolderObserver_    = make_shared<RTFolderObserver>(&g_ctx);
    commonShaderObserver_= make_shared<CommonShaderObserver>(&g_ctx); /* commonShaderはどのシェーダよりも先に実行すること */
    csFolderObserver_    = make_shared<CSFolderObserver>(&g_ctx);
    gsFolderObserver_    = make_shared<GSFolderObserver>(&g_ctx);
    psFolderObserver_    = make_shared<PSFolderObserver>(&g_ctx);
    vsFolderObserver_    = make_shared<VSFolderObserver>(&g_ctx);
#if 0
    textureFolderObserver_ = make_shared<TSFolderObserver>(&g_ctx);
#endif
    /**/
    /* HACK iniファイルから取得しているが、フォルダから自動取得するように変更する */
    QString musicPath = sandboxPath(SP_MUSIC)+configFileValue("config","music").toString();
    SceneArray& sceneArray = nlScene_getSceneData();
    introMaker_ = make_shared<nlIntroMaker>(&g_ctx, sceneArray, musicPath );
    /* レンダーの初期化 */
    nlRenderInit(&g_ctx);
    /**/
    ui.previewWidget->initialize( &g_ctx );
    /**/
    setupQtConnections();
    /* シーンマップのプロパティを設定した状態にする。 */
    std::vector<ExportSceneInfo> scenes = ui.sceneEditor->sceneInfoList();
    for( int i=0;i<scenes.size();++i)
    {
        const ExportSceneInfo& info = scenes[i];
        ProcessBuffer& processBuffer = nlScene_getProcessBuffer();
        geneSceneMap( sceneArray[i], processBuffer, info );
    }
    /* 最初のシーンに勝手に合わせてしまう */
    ui.sceneEditor->onChangeCurrentScene(0);
    ExportSceneInfo exportInfo = ui.sceneEditor->exportNodeInfoFromMemory();
    ProcessBuffer& processBuffer = nlScene_getProcessBuffer();
    geneSceneMap( sceneArray[0], processBuffer, exportInfo );
}
/**/
void nlSandbox::setupQtConnections()
{
    /**/
    connect( ui.chkRenderAxis, SIGNAL(clicked(bool)), this, SLOT(onToggleRenderAxis(bool)));
    /* path関連の初期化 */
    connect( ui.pathList,  SIGNAL(currentRowChanged(int)), ui.pathWidget, SLOT(onChangeCurrentCurve(int)) );
    connect( ui.radioCurveLiner,  SIGNAL(clicked(bool)), ui.pathWidget, SLOT(onClickLiner(bool)) );
    connect( ui.radioCurveCatmul,  SIGNAL(clicked(bool)), ui.pathWidget, SLOT(onClickCatmul(bool)) );		
    /* SceneEditorの初期化 */
    //NodeeditorboardDesc desc =  setUpNodeeditorboardDesc();
    //ui.sceneEditor->initialize( desc );
    ui.sceneEditor->initialize();
    /* メニュー関連 */
    connect( ui.actionMedia,         SIGNAL(triggered()), launcher_.get(), SLOT(onOpenMediaFolder() ) );
    connect( ui.actionMedia_Scene,   SIGNAL(triggered()), launcher_.get(), SLOT(onOpenSceneFolder() ) );
    connect( ui.actionMedia_Shader,    SIGNAL(triggered()), launcher_.get(), SLOT(onOpenShaderFolder() ) );
    connect( ui.actionMedia_Path,    SIGNAL(triggered()), launcher_.get(), SLOT(onOpenPathFolder() ) );
    connect( ui.actionMedia_Model,   SIGNAL(triggered()), launcher_.get(), SLOT(onOpenModelFolder() ) );
    connect( ui.actionExe,           SIGNAL(triggered()), launcher_.get(), SLOT(onOpenExeFolder() ) );
    connect( ui.actionMake,          SIGNAL(triggered()), introMaker_.get(), SLOT(make() ) );
    connect( ui.actionSaveCurrentScene, SIGNAL(triggered()), ui.sceneEditor, SLOT(saveCurrentScene() ) );
    connect( ui.actionClose_Open_preview,   SIGNAL(triggered()), this, SLOT(onTogglePreviewWindwow()) );
    connect( ui.actionClose_Open_path,      SIGNAL(triggered()), this, SLOT(onTogglePathEditWindwow()) );
    connect( ui.actionClose_Open_SceneEdit, SIGNAL(triggered()), this, SLOT(onToggleSceneEditWindwow()) );
    connect( ui.actionClose_Open_Timeline,  SIGNAL(triggered()), this, SLOT(onToggleTimelineWindwow()) );
    connect( ui.dockTimeline,  SIGNAL(stopMusic()), this, SLOT(onStopMusic()) );
    connect( ui.dockTimeline,  SIGNAL(startMusic(int)), this, SLOT(onRestartSong(int)) );
    connect( ui.actionPreview_1_1, SIGNAL(triggered()), this, SLOT(onResizePreview1over1()) );
    connect( ui.actionPreview_1_2, SIGNAL(triggered()), this, SLOT(onResizePreview1over2()) );
    connect( ui.actionPreview_1_4, SIGNAL(triggered()), this, SLOT(onResizePreview1over4()) );
    connect( ui.actionPreview_goto_origin, SIGNAL(triggered()), this, SLOT(onResetPreviewPos()) );
    /* シーン更新関連 */
    connect( ui.comboBox_scene, SIGNAL(currentIndexChanged(int)), ui.sceneEditor, SLOT(onChangeCurrentScene(int)));
    connect( ui.comboBox_scene, SIGNAL(currentIndexChanged(int)), ui.dockTimeline, SLOT(onChangeScene(int)));
    connect( ui.comboBox_scene, SIGNAL(currentIndexChanged(int)), ui.previewWidget, SLOT(RenderingForce()));
    /* パスエディタ関連 */
    connect( ui.pathWidget, SIGNAL(onRepaintMainPreview()), ui.previewWidget, SLOT(Rendering()) );
    connect( ui.resetPath,  SIGNAL(clicked(bool)), ui.pathWidget, SLOT(onResetCurve()) );
    /**/
    connect( ui.dockTimeline,  SIGNAL(changeFrameByMs(unsigned int)), ui.previewWidget, SLOT(Rendering()) );
    /**/
#if 0
    connect( textureFolderObserver_.get(),  SIGNAL(onChangeContent(void)), ui.previewWidget, SLOT(Rendering()) );
#endif
    connect( meshFolderObserver_.get(),  SIGNAL(onChangeContent(void)), ui.previewWidget, SLOT(Rendering()) );
    connect( musicFolderObserver_.get(),  SIGNAL(onChangeContent(void)), ui.previewWidget, SLOT(Rendering()) );
    connect( rtFolderObserver_.get(),  SIGNAL(onChangeContent(void)), ui.previewWidget, SLOT(Rendering()) );
    connect( csFolderObserver_.get(),  SIGNAL(onChangeContent(void)), ui.previewWidget, SLOT(Rendering()) );
    connect( gsFolderObserver_.get(),  SIGNAL(onChangeContent(void)), ui.previewWidget, SLOT(Rendering()) );
    connect( psFolderObserver_.get(),  SIGNAL(onChangeContent(void)), ui.previewWidget, SLOT(Rendering()) );
    connect( vsFolderObserver_.get(),  SIGNAL(onChangeContent(void)), ui.previewWidget, SLOT(Rendering()) );
    connect( commonShaderObserver_.get(),  SIGNAL(onChangeContent(void)), ui.previewWidget, SLOT(Rendering()) );
}
/**/
void nlSandbox::closeEvent(QCloseEvent *event)
{
    /* GUI配置保存 */
    const QString settingIniPath = sandboxPath(SP_APP)+QString("windowsetting.ini");
    QSettings setting( settingIniPath, QSettings::IniFormat);
    setting.setValue("windowState", saveState());
    setting.setValue( "geometry", saveGeometry());

    /* プレビューウィンドウの位置の保存 */
    setting.setValue( "preview_geom", ui.previewDock->geometry() );

    QMainWindow::closeEvent(event);
}
/**/
nlSandbox::~nlSandbox()
{
    /**/
    ui.sceneEditor->finalize();
}
/**/
void nlSandbox::onToggleRenderAxis( bool isRenderDebugLine )
{
    /* 座標のレンダリングを止めさせる。 */
    ui.previewWidget->setRenderDebugLine(isRenderDebugLine);
}
/**/
void nlSandbox::keyPressEvent(QKeyEvent* e )
{
    /* escで終了する */
    if( e->key() == Qt::Key_Escape )
    {
        exitApp();
    }
    /*  */
    QMainWindow::keyPressEvent( e );
}
/*  */
void nlSandbox::exitApp()
{
    close();
}
/* プレビューウィンドウの閉じ明けを行う */
void toggleDockVisible(QDockWidget* dock )
{
    dock->setVisible(!dock->isVisible());
}
void nlSandbox::onTogglePreviewWindwow()
{
    toggleDockVisible(ui.previewDock);
}
void nlSandbox::onTogglePathEditWindwow()
{
    toggleDockVisible(ui.dockPath);
}
void nlSandbox::onToggleSceneEditWindwow()
{
    toggleDockVisible(ui.dockSceneEdit);
}
void nlSandbox::onToggleTimelineWindwow()
{
    toggleDockVisible(ui.dockTimeline);
}
void nlSandbox::onStopMusic()
{
    nlSound_stop( g_ctx.sound_ );
}
void nlSandbox::onRestartSong(int timeInMs )
{
    nlSound_play( g_ctx.sound_, timeInMs );
}
/**/
static void resizePreview( float scale )
{
    /**/
    const QString settingIniPath = sandboxPath(SP_APP)+QString("windowsetting.ini");
    QSettings setting( settingIniPath, QSettings::IniFormat);
    setting.setValue("previewScale", scale );
    /* @NOTE Dockの設定がWidgetの設定になることに注意 */
    const int w = g_ctx.rendertargets[0].width_;
    const int h = g_ctx.rendertargets[0].height_;
    const int aimW = w*scale;
    const int aimH = h*scale;
    QSize s = ui.previewDock->size();
    ui.previewDock->setMinimumSize( QSize(aimW,aimH) );
    ui.previewDock->setMaximumSize( QSize(aimW,aimH) );
    ui.previewDock->setMinimumSize( QSize(aimW,aimH) );
}
void nlSandbox::onResizePreview1over1()
{
    resizePreview(1.0f);
}
void nlSandbox::onResizePreview1over2()
{
    resizePreview(0.5f);
}
void nlSandbox::onResizePreview1over4()
{
    resizePreview(0.25f);
}
/**/
void nlSandbox::onResetPreviewPos()
{
    ui.previewDock->move(0,0);
    /* HACK logも原点に。 */
    ui.dockLog->move(0,0);
}

#ifndef NLSANDBOX_H
#define NLSANDBOX_H

#include "main/stdafx.h"

#include "ui_nlsandbox.h"


extern Ui::nlSandboxClass ui;
extern nlEngineContext g_ctx;

/**/
NodeeditorboardDesc setUpNodeeditorboardDesc();
/**/
class nlSandbox 
    :public QMainWindow
{
    Q_OBJECT

public:
    nlSandbox(QWidget *parent = 0, Qt::WFlags flags = 0);
public:
    ~nlSandbox();
public:
    void initialize();
private:
    void closeEvent(QCloseEvent *event);
private:
    void setupQtConnections();
    void initializeAllSandbox_();
    void keyPressEvent(QKeyEvent* e );
public slots:
    void onToggleRenderAxis(bool);

    //void updateSceneList_();

    void exitApp();
    /* DockŠJ•Â */
    void onTogglePreviewWindwow();
    void onTogglePathEditWindwow();
    void onToggleSceneEditWindwow();
    void onToggleTimelineWindwow();
    /**/
    void onStopMusic();
    void onRestartSong(int value);
    /**/
    void onResizePreview1over1();
    void onResizePreview1over2();
    void onResizePreview1over4();
    void onResetPreviewPos();
private:
    void openFolder_( std::string path );
private:
    /**/
    QTreeWidgetItem* curSelectedConstItem_;
    float* targetVal_[4];
    /**/
    bool isRenderDebugLine_;
    /**/
    boost::shared_ptr<class nlLauncher> launcher_;
    boost::shared_ptr<class nlIntroMaker> introMaker_;
#if 0
    boost::shared_ptr<class TSFolderObserver> textureFolderObserver_;
#endif
    boost::shared_ptr<class MeshFolderObserver> meshFolderObserver_;
    boost::shared_ptr<class MusicFolderObserver> musicFolderObserver_;
    boost::shared_ptr<class PathFolderObserver> pathFolderObserver_;
    boost::shared_ptr<class RTFolderObserver> rtFolderObserver_;
    boost::shared_ptr<class CSFolderObserver> csFolderObserver_;
    boost::shared_ptr<class GSFolderObserver> gsFolderObserver_;
    boost::shared_ptr<class PSFolderObserver> psFolderObserver_;
    boost::shared_ptr<class VSFolderObserver> vsFolderObserver_;
    boost::shared_ptr<class CommonShaderObserver> commonShaderObserver_;
};

#endif // NLSANDBOX_H

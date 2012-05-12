#pragma once

/**
* @brief ƒ‰ƒ“ƒ`ƒƒ[
*/
class nlLauncher
    :public QObject
{
    Q_OBJECT
public:
    public slots:
        void onOpenMediaFolder();
        void onOpenSceneFolder();
        void onOpenShaderFolder();
        void onOpenPathFolder();
        void onOpenModelFolder();
        void onOpenExeFolder();
private:
    void openFolder_( QString path );
    void execCommandLine( QString cmdline, bool isWatiForShutdown );
};

#include "main/stdafx.h"
#include "nlsandbox.h"
#include "shared/nlMemory.h"
#include "shared/sound/nlSound.h"
#include "tool/nlUtil.hpp"
#include "shared/nlLog.hpp"

/**/

#ifdef INTROMODE
/* TODO ここにリリース時のメイン関数を書く */
#else
int main(nlInt32 argc, nlInt8* argv[])
{
    /*　ヒープメモリ破壊とメモリリークの検地設定 */
    _CrtSetDbgFlag(_CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    /**/
    int ret = 0;
    memset( &g_ctx, 0, sizeof(nlEngineContext) );

    try
    {
        /**/
        QApplication app(argc, argv);
        QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
        /* exeのあるディレクトリをカレントディレクトリにする */
        QDir::setCurrent( sandboxPath( SP_APP ) );
        /* スキンを設定する */
        QFile file(":/nlSandbox/skin");
        file.open(QFile::ReadOnly);
        QString styleSheet = QLatin1String(file.readAll());
        app.setStyleSheet(styleSheet);
        const Qt::Alignment align = (Qt::AlignRight | Qt::AlignBottom);
        const QColor color( Qt::white );
        const Qt::WFlags wflags = (configFileValue("config","titlebar")=="enable")?0:Qt::FramelessWindowHint;
        nlSandbox mainWindow(0,wflags);
        mainWindow.initialize();
        /**/
        QString songName;
        QString musicPath = sandboxPath(SP_MUSIC)+configFileValue("config","music").toString();
        QFile musicFile(musicPath);
        musicFile.open(QIODevice::ReadOnly);
        QByteArray musicData = musicFile.readAll();
        g_ctx.sound_ = nlSound_constructFromMemory(musicData.data(),musicData.size(),&g_ctx);

        mainWindow.show();
        /* 起動する */
        ret = app.exec();
    }
    catch( std::string error )
    {
        MessageBoxA(NULL, error.c_str(), "error", MB_OK );
        ret = 0;
    }
    /* メモリリークダンプ */
    _CrtDumpMemoryLeaks();

    return ret;
}
#endif

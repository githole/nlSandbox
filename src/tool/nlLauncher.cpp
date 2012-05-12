#include "main/stdafx.h"
#include "tool/nlLauncher.h"
#include "tool/nlUtil.hpp"
#include "shared/nlLog.hpp"
#include "nlLogComposer.hpp"

using namespace std;
using namespace boost;

/**/
void nlLauncher::onOpenMediaFolder()
{
    openFolder_( sandboxPath(SP_MEDIA) );
}
/**/
void nlLauncher::onOpenSceneFolder()
{
    openFolder_( sandboxPath(SP_SCENE) );
}
/**/
void nlLauncher::onOpenShaderFolder()
{
    openFolder_( sandboxPath(SP_SHADER) );
}
/**/
void nlLauncher::onOpenPathFolder()
{
    openFolder_( sandboxPath(SP_PATH) );
}
/**/
void nlLauncher::onOpenModelFolder()
{
    openFolder_( sandboxPath(SP_MESH) );
}
/**/
void nlLauncher::onOpenExeFolder()
{
    openFolder_( sandboxPath(SP_APP) );
}
/**/
void nlLauncher::openFolder_( QString path )
{
    const QString cmdLine = QString("Explorer \"") + path + QString("\"");
    execCommandLine( cmdLine, false );
}
/**/
void nlLauncher::execCommandLine( QString cmdline, bool isWatiForShutdown )
{
    QByteArray ba = cmdline.toLocal8Bit();
    char* cmdlineCStr = ba.data();
    STARTUPINFOA si = {};
    PROCESS_INFORMATION pi = {};
    si.cb = sizeof(si);
    /**/
    if( !CreateProcessA( NULL,
        cmdlineCStr, NULL, NULL, FALSE, 0,
        NULL, NULL, &si, &pi ) )
    {
        NL_ERR(ERR_013,cmdlineCStr);
    }
    else
    {
        if( isWatiForShutdown )
        {
            WaitForSingleObject( pi.hProcess, INFINITE );
            CloseHandle( pi.hProcess );
            CloseHandle( pi.hThread );
        }
    }
}
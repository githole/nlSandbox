#include "main/stdafx.h"
#include "tool/nlUtil.hpp"
#include "tool/nlId.hpp"
#include "shared/nlMemory.h"
#include "shared/nlLog.hpp"
#include "tool/nlLogComposer.hpp"

/**/
RenderTargetDescs loadRTDesc( const QString& path )
{
    /**/
    QByteArray tmpByteArray = path.toLocal8Bit();
    const int numRT = GetPrivateProfileIntA( "RT", "NUM_RT", 0, tmpByteArray.data() );
    /**/
    RenderTargetDescs descs = {0};
    descs.numRT = numRT;
    /* RTは0番がバックバッファで、1番から別のRTを作る点に注意 */
    for(int i=1;i<numRT+1;++i)
    {
        /**/
        RenderTargetDesc& desc = descs.rtDescs[i];
        /**/
        char formatStr[MAX_PATH+1] = {'\0'};
        char keyStr[MAX_PATH+1] = {'\0'};
        /**/
        sprintf(keyStr, "name%d", i );
        GetPrivateProfileStringA( "RT", keyStr, "", formatStr, MAX_PATH, tmpByteArray.data() );
        if( strcmp( formatStr, "" ) )
        {
            setId("RT_NAME", formatStr );
        }
        /* フォーマット取得 */
        sprintf(keyStr, "format%d", i );
        GetPrivateProfileStringA( "RT", keyStr, "", formatStr, MAX_PATH, tmpByteArray.data() );
        if( !strcmp( formatStr, "B8G8R8A8" ))
        { desc.format = DXGI_FORMAT_B8G8R8A8_UNORM; }
        else if( !strcmp( formatStr, "R32G32B32A32_FLOAT" ))
        { desc.format = DXGI_FORMAT_R32G32B32A32_FLOAT; }
        else if( !strcmp( formatStr, "R16G16B16A16_FLOAT" ))
        { desc.format = DXGI_FORMAT_R16G16B16A16_FLOAT; }
        else if( !strcmp( formatStr, "R16G16_FLOAT" ))
        { desc.format = DXGI_FORMAT_R16G16_FLOAT; }
        else if( !strcmp( formatStr, "R32_FLOAT" ))
        { desc.format = DXGI_FORMAT_R32_FLOAT; }
        else if( !strcmp( formatStr, "R32G32_FLOAT" ))
        { desc.format = DXGI_FORMAT_R32G32_FLOAT; }
        else
        { 
            NL_ERR( ERR_015, formatStr );
            desc.format = DXGI_FORMAT_B8G8R8A8_UNORM;
        }
        /* サイズ */
        sprintf(keyStr, "size%d", i );
        GetPrivateProfileStringA( "RT", keyStr, "normal", formatStr, MAX_PATH, tmpByteArray.data() );
        if( !strcmp(formatStr,"normal") )
        { desc.scaleByBackbuffer = 0; }
        else if( !strcmp(formatStr,"half") )
        { desc.scaleByBackbuffer = 1; }
        else if( !strcmp(formatStr,"quarter") )
        { desc.scaleByBackbuffer = 2; }
        else  
        {
            NL_ERR( ERR_016, formatStr );
            desc.scaleByBackbuffer = 0;
        }
    }
    /**/
    descs.numDS = GetPrivateProfileIntA( "DS", "NUM_DS", 0, tmpByteArray.data() );
    /* DSは0番がバックバッファ用で、1番から別のDSを作る点に注意 */
    for(int i=1;i<descs.numDS+1;++i)
    {
        /**/
        RenderTargetDesc& desc = descs.dsDescs[i];
        /**/
        char formatStr[MAX_PATH+1] = {'\0'};
        char keyStr[MAX_PATH+1] = {'\0'};
        /**/
        sprintf(keyStr, "name%d", i );
        GetPrivateProfileStringA( "DS", keyStr, "", formatStr, MAX_PATH, tmpByteArray.data() );
        if( strcmp( formatStr, "" ) )
        {
            setId("DS_NAME", formatStr );
        }
        /* フォーマットは固定 */
        desc.format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        /* サイズ */
        sprintf(keyStr, "size%d", i );
        GetPrivateProfileStringA( "DS", keyStr, "normal", formatStr, MAX_PATH, tmpByteArray.data() );
        if( !strcmp(formatStr,"normal") )
        { desc.scaleByBackbuffer = 0; }
        else if( !strcmp(formatStr,"half") )
        { desc.scaleByBackbuffer = 1; }
        else if( !strcmp(formatStr,"quarter") )
        { desc.scaleByBackbuffer = 2; }
        else  
        {
            NL_ERR( ERR_016, formatStr );
            desc.scaleByBackbuffer = 0;
        }
    }
    /**/
    return descs;
}
/*----------------------------------------------------------------------------------------------------*/
// Path関連
/*----------------------------------------------------------------------------------------------------*/
QVariant configFileValue( QString group, QString key, QString defaultValue )
{
    QVariant result;
    QString fileName  = sandboxPath(SP_APP)+QString("config.ini");
    QSettings settings( fileName, QSettings::IniFormat );
    settings.beginGroup(group);
    result = settings.value( key, defaultValue );
    settings.endGroup();
    /**/
    return result;
}
/**/
QString sandboxPath( SandboxPath pathType )
{
    switch( pathType )
    {
    case SP_EXE:
        {
            QString tmp = QCoreApplication::applicationFilePath();
            return tmp;
        }
    case SP_APP:
        {
            return QCoreApplication::applicationDirPath()+QString("/");
        }
    case SP_MEDIA:
        {
            QVariant mediaRelPath = configFileValue("config","mediaPath","media");
            QString mediaDir = sandboxPath(SP_APP) + mediaRelPath.toString() + "/";
            /* 最後に本当にそのディレクトリがあるかをチェック */
            QDir dir(mediaDir);
            if( !dir.exists( mediaDir ) )
            {
                //nlPrintf( "media directory is not found\n" );
                return "";
            }
            return mediaDir;
        }
    case SP_MESH:
        return sandboxPath(SP_MEDIA)+QString("mesh/");
    case SP_SHADER:
        return sandboxPath(SP_MEDIA)+QString("shader/");
    case SP_RENDERTARGET:
        return sandboxPath(SP_MEDIA)+QString("rendertarget/");
    case SP_PATH:
        return sandboxPath(SP_MEDIA)+QString("path/");
    case SP_MUSIC:
        return sandboxPath(SP_MEDIA)+QString("music/");
    case SP_SCENE:
        return sandboxPath(SP_MEDIA)+QString("scene/");
    case SP_TMP:
        return sandboxPath(SP_APP)+QString("/tmp/");
    }
    return "";
}
/*----------------------------------------------------------------------------------------------------*/
// コマンド
/*----------------------------------------------------------------------------------------------------*/
void cmd( const char* aCmdline, bool isWatiForShutdown )
{
    char* cmdline = const_cast<char*>( aCmdline );
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    memset( &si, 0, sizeof(si) );
    si.cb = sizeof(si);
    memset( &pi, 0, sizeof(pi) );
    if( !CreateProcessA( NULL, // No module name (use command line). 
        cmdline, // Command line. 
        NULL,             // Process handle not inheritable. 
        NULL,             // Thread handle not inheritable. 
        FALSE,            // Set handle inheritance to FALSE. 
        0,                // No creation flags.
        NULL,             // Use parent's environment block.
        NULL,             // Use parent's starting directory.
        &si,              // Pointer to STARTUPINFO structure.
        &pi )             // Pointer to PROCESS_INFORMATION structure.
        )
    {
        NL_ERR(ERR_019,aCmdline);
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
/*----------------------------------------------------------------------------------------------------*/
// その他関数
/*----------------------------------------------------------------------------------------------------*/
float f16b( float src )
{
#if 1
    /* 16bit-nize */
    const int resultInt = ((*(int*)&src)&0xffff0000);
    const float result = *(float*)&resultInt;
    return result;
#else/* 実験用にそのまま戻すバージョン */
    return src;
#endif
}

#pragma once

#include "shared/nlTypedef.h"
#include "shared/nlEngineContext.hpp"

/**
 * @brief nlSandboxに関連するパスの種類
 */
enum SandboxPath
{
    SP_EXE,
    SP_APP,
    SP_MEDIA,
    SP_MESH,
    SP_SHADER,
    SP_RENDERTARGET,
    SP_PATH,
    SP_MUSIC,
    SP_SCENE,
    SP_TMP,
};
/** 
 * @brief nlSandboxに関連するパスを取得する
 */
QString sandboxPath( SandboxPath pathType );
/**
 * @brief コマンドライン実行する
 */
void cmd( const char* cmdLine, bool isWaitForShutdown = false );
/**
 * @brief 指定した浮動少数を下位16bitをなくしたものに変換する
 */
float f16b( float src );
/**
 * @brief 
 */
RenderTargetDescs loadRTDesc( const QString& path );
/**
 * @brief config用のiniから情報を取得する 
 */
QVariant configFileValue( QString group, QString key, QString defaultValue = "" );

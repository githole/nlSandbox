#pragma once

#include "shared/nlTypedef.h"
#include "shared/nlEngineContext.hpp"

/**
 * @brief nlSandbox�Ɋ֘A����p�X�̎��
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
 * @brief nlSandbox�Ɋ֘A����p�X���擾����
 */
QString sandboxPath( SandboxPath pathType );
/**
 * @brief �R�}���h���C�����s����
 */
void cmd( const char* cmdLine, bool isWaitForShutdown = false );
/**
 * @brief �w�肵����������������16bit���Ȃ��������̂ɕϊ�����
 */
float f16b( float src );
/**
 * @brief 
 */
RenderTargetDescs loadRTDesc( const QString& path );
/**
 * @brief config�p��ini��������擾���� 
 */
QVariant configFileValue( QString group, QString key, QString defaultValue = "" );

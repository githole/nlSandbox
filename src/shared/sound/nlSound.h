#pragma once

#include "shared/nlTypedef.h"
#include "shared/nlEngineContext.hpp"

/* ���p����V���Z���`���� */
//#define USE_FMODEX
//#define USE_FMODMINI
#define USE_V2
//#define WAVE_PLAYER
/* TODO ������2�ȏ��`����ƃG���[�ɂȂ�悤�ɂ��� */

/**
* @brief �T�E���h��������
*/
nlSound* nlSound_constructFromMemory( const nlInt8* memory, unsigned int size, nlEngineContext* cxt );
/**
 *
 */
void nlSound_play( nlSound* cxt, unsigned long timeInMs );
/**
* @brief �T�E���h��~����
*/
void nlSound_stop( nlSound* cxt );
/**
* @brief �T�E���h�̍Đ��ʒu���擾����
*/
unsigned int nlSound_position( nlSound* cxt );
/**
* @brief 
*/
void nlSound_setVolume( nlSound* cxt, nlFloat32 volume );
/**
* @brief �Đ������H
*/
bool nlSound_isFinished( nlSound* cxt );
/**
* @brief 
*/
void nlSound_finalize( nlSound* cxt );

#pragma once

#include "shared/nlTypedef.h"
#include "shared/nlEngineContext.hpp"

/* 利用するシンセを定義する */
//#define USE_FMODEX
//#define USE_FMODMINI
#define USE_V2
//#define WAVE_PLAYER
/* TODO 同時に2つ以上定義するとエラーになるようにする */

/**
* @brief サウンド生成する
*/
nlSound* nlSound_constructFromMemory( const nlInt8* memory, unsigned int size, nlEngineContext* cxt );
/**
 *
 */
void nlSound_play( nlSound* cxt, unsigned long timeInMs );
/**
* @brief サウンド停止する
*/
void nlSound_stop( nlSound* cxt );
/**
* @brief サウンドの再生位置を取得する
*/
unsigned int nlSound_position( nlSound* cxt );
/**
* @brief 
*/
void nlSound_setVolume( nlSound* cxt, nlFloat32 volume );
/**
* @brief 再生中か？
*/
bool nlSound_isFinished( nlSound* cxt );
/**
* @brief 
*/
void nlSound_finalize( nlSound* cxt );

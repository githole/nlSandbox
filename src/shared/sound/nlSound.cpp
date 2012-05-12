/**/
#include "main/stdafx.h"
/**/
#include "shared/sound/nlSound.h"

/**/
#include "shared/nlMemory.h"


#ifndef INTROMODE
using namespace std;
#endif

/**/
nlSound* nlSound_constructFromMemory( const nlInt8* memory, unsigned int size, nlEngineContext* cxt )
{
#ifdef USE_FMODEX
    return NULL;
#elif defined USE_FMODMINI
    extern void* fmodmini_constructFromMemory( const nlInt8* memory, unsigned int size );
    return (nlSound*)fmodmini_constructFromMemory(memory,size);
#elif defined USE_V2
    extern void* v2_constructFromMemory( const nlInt8* memory, unsigned int size, nlEngineContext* cxt );
    return (nlSound*)v2_constructFromMemory(memory,size,cxt);
#elif defined WAVE_PLAYER
    extern void* WavePlayer_constructFromMemory( const nlInt8* memory, unsigned int size );
    return (nlSound*)WavePlayer_constructFromMemory(memory,size);
#endif
    return NULL;
}
/**
* @brief 再生開始する
*/
void nlSound_play( nlSound* cxt, unsigned long timeInMs )
{
#ifdef USE_FMODEX
#elif defined USE_FMODMINI
    extern void fmodmini_play(void* aCxt);
    fmodmini_play( cxt );
#elif defined USE_V2
    extern void v2_play(void* aCxt, unsigned timeInMs );
    v2_play(cxt,timeInMs);
#elif defined WAVE_PLAYER
    void WavePlayer_play(void* aCxt );
    WavePlayer_play(cxt);
#else
#endif
}
/**
* @brief サウンド停止する
*/
void nlSound_stop( nlSound* cxt )
{
#ifdef USE_FMODEX
    extern void fmodex_stop( void* aCxt );
    fmodex_stop(cxt);
#elif defined USE_FMODMINI
    extern void fmodmini_stop( void* aCxt );
    fmodmini_stop(cxt);
#elif defined USE_V2
    extern void v2_stop( void* aCxt );
    v2_stop(cxt);
#elif defined WAVE_PLAYER
    extern void WavePlayer_stop( void* aCxt );
    WavePlayer_stop(cxt);
#else
#endif
}
/**
* @brief サウンドの再生位置を取得する
*/
unsigned int nlSound_position( nlSound* cxt )
{
#ifdef USE_FMODEX
    extern unsigned int fmodex_position( void* aCxt );
    return fmodex_position(cxt);
#elif defined USE_FMODMINI
    extern unsigned int fmodmini_position( void* cxt );
    return fmodmini_position(cxt);
#elif defined USE_V2
    extern unsigned int v2_position( void* aCxt );
    return v2_position(cxt);
#elif defined WAVE_PLAYER
    extern unsigned int WavePlayer_position( void* aCxt );
    return WavePlayer_position(cxt);
#else
    return 0;
#endif
}
/**
* @brief 
*/
void nlSound_setVolume( nlSound* cxt, nlFloat32 volume )
{
#ifdef USE_FMODEX
    extern void fmodex_setVolume( void* aCxt, nlFloat32 volume );
    fmodex_setVolume(cxt,volume);
#elif defined USE_FMODMINI
    extern void fmodmini_setVolume( void* aCxt, nlFloat32 volume );
    fmodmini_setVolume(cxt,volume);
#elif defined USE_V2
    extern void v2_setVolume( void* aCxt, nlFloat32 volume );
    v2_setVolume(cxt,volume);
#elif defined WAVE_PLAYER
    extern void WavePlayer_setVolume( void* aCxt, nlFloat32 volume );
    WavePlayer_setVolume(cxt,volume);
#endif
}
/**
* @brief 再生中か？
*/
bool nlSound_isFinished( nlSound* cxt )
{
#ifdef USE_FMODEX
    extern bool fmodex_isFinished( void* aCxt );
    return fmodex_isFinished(cxt);
#elif defined USE_FMODMINI
    extern bool fmodmini_isFinished( void* aCxt );
    return fmodmini_isFinished(cxt);
#elif defined USE_V2
    extern bool v2_isFinished( void* aCxt );
    return v2_isFinished( cxt );
#elif defined WAVE_PLAYER
    extern bool WavePlayer_isFinished( void* aCxt );
    return WavePlayer_isFinished( cxt );
#else
#endif
    return false;
}
/**
* @brief 
*/
void nlSound_finalize( nlSound* cxt )
{
#ifdef USE_FMODEX
    extern void fmodex_finalize( void* aCxt );
    fmodex_finalize(cxt);
#elif defined USE_FMODMINI
    extern void fmodmini_finalize( void* aCxt );
    fmodmini_finalize(cxt);
#elif defined USE_V2
    extern void v2_finalize( void* aCxt );
    v2_finalize(cxt);
#elif defined WAVE_PLAYER
    extern void WavePlayer_finalize( void* aCxt );
    WavePlayer_finalize(cxt);
#else
#endif
}


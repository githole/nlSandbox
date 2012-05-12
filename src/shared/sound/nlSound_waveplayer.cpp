#include "main/stdafx.h"
#include "shared/nlMemory.h"


#ifdef WAVE_PLAYER
#include <iostream>
#include <irrKlang.h>

using namespace irrklang;
#pragma comment(lib, "irrKlang.lib") // link with irrKlang.dll

/**/
struct WavePlayerContext
{
    ISoundEngine* engine;
    ISound* sound;
    irrklang::ISoundSource* soundSource;
};
/**/
void* WavePlayer_constructFromFile( const nlInt8* fileName )
{
    WavePlayerContext* cxt = (WavePlayerContext*)nlMalloc(sizeof(WavePlayerContext));
    cxt->engine = createIrrKlangDevice();
    //cxt->sound  = cxt->engine->play2D(fileName,false,true,true,ESM_STREAMING,false); 
    //cxt->sound  = cxt->engine->play2D(fileName); 
    cxt->soundSource = cxt->engine->addSoundSourceFromFile(fileName); 
    return cxt;
}
/**/
void* WavePlayer_constructFromMemory( const nlInt8* memory, unsigned int size )
{
    /* TODO ŽÀ‘• */
    return NULL;
}
/**/
void WavePlayer_play(void* aCxt )
{
    WavePlayerContext* cxt = (WavePlayerContext*)aCxt;
    cxt->engine->play2D( cxt->soundSource );
    //cxt->sound->setPaused( false );
}
/**/
void WavePlayer_seekInMs(void* aCxt, unsigned int posInMs )
{
    WavePlayerContext* cxt = (WavePlayerContext*)aCxt;
    cxt->sound->setPlayPosition( posInMs );
}
/**/
void WavePlayer_stop( void* aCxt )
{
    WavePlayerContext* cxt = (WavePlayerContext*)aCxt;
    cxt->sound->stop();
}
/**/
unsigned int WavePlayer_position( void* aCxt )
{
    WavePlayerContext* cxt = (WavePlayerContext*)aCxt;
    return cxt->sound->getPlayPosition();
}
/**/
void WavePlayer_setVolume( void* aCxt, float volume )
{
    WavePlayerContext* cxt = (WavePlayerContext*)aCxt;
    cxt->sound->setVolume(volume);   
}
/**/
bool WavePlayer_isFinished( void* aCxt )
{
    WavePlayerContext* cxt = (WavePlayerContext*)aCxt;
    return cxt->sound->isFinished();
}
/**/
void WavePlayer_finalize( void* aCxt )
{
    WavePlayerContext* cxt = (WavePlayerContext*)aCxt;
    cxt->engine->drop();
    nlFree(cxt);
}
#endif /* #ifdef WAVE_PLAYER */

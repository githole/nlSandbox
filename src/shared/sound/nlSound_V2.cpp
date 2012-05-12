/**/
#include "main/stdafx.h"
#include "v2mplayer_custom.h"
#include "libv2.h"
#include "shared/nlMemory.h"

#include "shared/nlTypedef.h"
#include "shared/nlEngineContext.hpp"

/**/
void* v2_constructFromMemory( const nlInt8* memory, unsigned int size, nlEngineContext* cxt )
{
    V2MPlayer* player = (V2MPlayer*)nlMalloc( sizeof(V2MPlayer) );
    player->Init();
    player->Open(memory);
    dsInit(player->RenderProxy,player,GetForegroundWindow());
    player->Stop();
    return player;
}
/**/
void v2_play(void* aCxt, unsigned int timeInMs )
{
    const unsigned long start = timeGetTime();
    V2MPlayer* player = (V2MPlayer*)aCxt;
    /* NOTE ˆêuŽ~‚ß‚È‚¢‚ÆPlayer“à•”‚ÌDirectSound‚Ì‘‚«ž‚Ý‚ªŠÔ‚É‡‚í‚È‚¢Ž–‚É’ˆÓ */
    player->Stop();
    Sleep(10);
    unsigned long truStartTime = timeInMs + (timeGetTime()-start);
    player->Play( truStartTime );
}
/**/
void v2_stop( void* aCxt )
{
    V2MPlayer* player = (V2MPlayer*)aCxt;
    player->Stop();
}
/**/
unsigned int v2_position( void* aCxt )
{
    V2MPlayer* player = (V2MPlayer*)aCxt;
    return player->playPos();
}
/**/
void v2_setVolume( void* aCxt, nlFloat32 volume )
{
    /* NOTE ƒ{ƒŠƒ…[ƒ€‚ÍÝ’è‚Å‚«‚È‚¢ */
}
/**/
bool v2_isFinished( void* aCxt )
{
    V2MPlayer* player = (V2MPlayer*)aCxt;
    return !player->IsPlaying();
}
/**/
void v2_finalize( void* aCxt )
{
    V2MPlayer* player = (V2MPlayer*)aCxt;
    /*dsClose();
    player->Close();
    nlFree(player);*/
}

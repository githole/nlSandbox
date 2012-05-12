#include "main/stdafx.h"
#include "shared/nlMemory.h"
#include "nlTypedef.h"

/*----------------------------------------------------------------------------------------------------*/
// Memory Functions
/*----------------------------------------------------------------------------------------------------*/
/* compatible malloc() */
void* nlMalloc( unsigned int size )
{
#if 0
    return calloc(size,1);
#else
    /*
    XP+SP2よりHeapAllocはアライメントされるようになった。
    XP+SP2以降のマシンで動くことを仮定している。
    */
    void* ptr = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size );
    return ptr;
#endif
}
/**/
void nlFree( void* ptr )
{
#if 0
    free( ptr );
#else
    HeapFree(GetProcessHeap(), 0, ptr );
#endif
}
/* compatible memset() */
void nlMemset( void *dst, int val, int amount )
{
    _asm mov edi, dst;
    _asm mov eax, val;
    _asm mov ecx, amount;
    _asm rep stosb;
}
/**/
void nlMemcpy( void *dst, const void *ori, int amount )
{
    _asm mov edi, dst;
    _asm mov esi, ori;
    _asm mov ecx, amount;
    _asm rep movsb;
}
/**/
void nlMemmove(nlUint8* dst, nlUint8* src, unsigned int size )
{
    /*NL_ASSERT(src != NULL);
    NL_ASSERT(dst != NULL);*/

    const nlUint8 *psrc = (const nlUint8*)src;
    nlUint8 *pdst = (nlUint8*)dst;

    if (dst <= src || pdst >= psrc+size)
    {
        // Non-overlapping buffers, so copy from
        // lower addresses to higher addresses.
        while (size--)
        {
            *pdst++ = *psrc++;
        }
    }
    else
    {
        // Overlapping buffers, so copy from
        // higher addresses to lower addresses.
        pdst = pdst+size-1;
        psrc = psrc+size-1;

        while (size--)
        {
            *pdst-- = *psrc--;
        }
    }
}
/**/
int nlMemEqual(const void* mem0, const void* mem1, unsigned int count)
{
    //eASSERT(mem0 != NULL);
    //eASSERT(mem1 != NULL);
    const nlUint8* ptr0 = (nlUint8*)mem0;
    const nlUint8* ptr1 = (nlUint8*)mem1;
    for(unsigned int i=0; i<count; i++)
    {
        if (ptr0[i] != ptr1[i])
        {
            return 0;
        }
    }
    return 1;
}

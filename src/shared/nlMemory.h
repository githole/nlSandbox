#pragma once

#ifdef __cplusplus
extern "C" {
#endif
/* Memory Functions */
void* nlMalloc( unsigned int size );
void nlFree( void* ptr );
void nlMemset( void *dst, int val, int amount );
void nlMemcpy( void *dst, const void *ori, int amount );
void nlMemmove(unsigned char* dst, unsigned char* src, unsigned int size );
int nlMemEqual(const void* mem0, const void* mem1, unsigned int count);

#ifdef __cplusplus
}
#endif


#include "main/stdafx.h"
#include "shared/container/nlVector.hpp"
#include "shared/nlMemory.h"
#include "shared/nlTypedef.h"
#include "shared/math/nlMath.h"


void nlVectorInit(PtrArray *a, nlUint32 typeSize, nlUint32 size)
{
    NL_ASSERT(a != NULL);
    NL_ASSERT(typeSize > 0);

    a->data_ = NULL;
    a->size_ = 0;
    a->capacity_ = 0;
    a->typeSize_ = typeSize;

    if (size > 0)
    {
        nlVectorResize(a, size);
    }
}

void nlVectorCopy(PtrArray *a, const PtrArray *ta)
{
    NL_ASSERT(a != NULL);
    NL_ASSERT(ta != NULL);

    nlVectorInit(a, ta->typeSize_, 0);
    nlVectorReserve(a, ta->capacity_);
    a->size_ = ta->size_;

    if (ta->size_)
    {
        nlMemcpy(a->data_, ta->data_, ta->size_*ta->typeSize_); 
    }
}

void nlVectorClear(PtrArray *a)
{
    NL_ASSERT(a != NULL);
    a->size_ = 0;
}

void nlVectorFree(PtrArray *a)
{
    NL_ASSERT(a != NULL);

    nlFree(a->data_);
    a->data_ = NULL; //introhack
    a->size_ = 0;
    a->capacity_ = 0;
}

void nlVectorReserve(PtrArray *a, nlUint32 capacity)
{
    NL_ASSERT(a != NULL);

    if (capacity == 0)
    {
        nlVectorClear(a);
    }
    else if (a->capacity_ < capacity)
    {
//        nlUint8 *temp = new nlUint8[capacity*a->typeSize_];
        nlUint8* temp = (nlUint8*) nlMalloc(capacity*a->typeSize_);
        NL_ASSERT(temp != NULL);
        nlUint32 newSize = 0;

        if (a->data_)
        {
            newSize = MN(a->size_, capacity);
            nlMemcpy(temp, a->data_, newSize*a->typeSize_);
            nlFree(a->data_);
        }

        a->data_ = (void**)temp;
        a->size_ = newSize;
        a->capacity_ = capacity;
    }
}

void nlVectorResize(PtrArray *a, nlUint32 size)
{
    NL_ASSERT(a != NULL);

    if (size > a->capacity_)
    {
        nlVectorReserve(a, size);
    }

    a->size_ = size;
}

void nlVectorAppend(PtrArray *a, const void* data)
{
    NL_ASSERT(a != NULL);
    NL_ASSERT(data != NULL);

    if (a->size_ >= a->capacity_)
    {
        const nlUint32 newCapacity = (a->capacity_ > 0 ? a->capacity_*2 : 32);
        nlVectorReserve(a, newCapacity);
    }

    const nlUint8 *src = (nlUint8*)data;
    nlUint8 *dst = ((nlUint8*)a->data_)+a->size_*a->typeSize_;

    for (nlUint32 i=0; i<a->typeSize_; i++)
    {
        dst[i] = src[i];
    }

    a->size_++;
}

void nlVectorInsert(PtrArray *a, nlUint32 index, const void* data)
{
    NL_ASSERT(a != NULL);
    NL_ASSERT(index <= a->size_);
    NL_ASSERT(data != NULL);

    if (a->size_ >= a->capacity_)
    {
        const nlUint32 newCapacity = (a->capacity_ > 0 ? a->capacity_*2 : 32);
        nlVectorReserve(a, newCapacity);
    }

    nlMemmove(((nlUint8 *)a->data_)+(index+1)*a->typeSize_,
             ((nlUint8 *)a->data_)+index*a->typeSize_,
             (a->size_-index)*a->typeSize_);

    nlMemcpy(((nlUint8 *)a->data_)+index*a->typeSize_, data, a->typeSize_);
    a->size_++;
}

void nlVectorRemoveAt(PtrArray *a, nlUint32 index)
{
    NL_ASSERT(a != NULL);
    NL_ASSERT(index < a->size_);
    nlMemmove(((nlUint8 *)a->data_)+index*a->typeSize_,
             ((nlUint8 *)a->data_)+(index+1)*a->typeSize_,
             (a->size_-index-1)*a->typeSize_);

    a->size_--;
}

int nlVectorExists(const PtrArray *a, const void* data)
{
    NL_ASSERT(a != NULL);
    NL_ASSERT(data != NULL);

    for (nlUint32 i=0, index=0; i<a->size(); i++, index+=a->typeSize_)
    {
        if (nlMemEqual((a->data_)+index, data, a->typeSize_))
        {
            return i;
        }
    }

    return -1;
}

#pragma once

/**
 * @brief std::vector相当
 *        メンバ関数をインラインにし、グローバル関数を呼ぶことで
 *        コードの肥大化を防いでいます。
 *        またPOD形のみ対応しています。
 *        nlVector<nlVector<int>>のような使い方は上手く動きません。
 *        コピー動作も動きません。
 *        EnigmaStudio3のソースコードから拝借
 *        http://www.pouet.net/prod.php?which=58418
 */

#include "shared/nlTypedef.h"

/**/
template<class T> class nlVector;
typedef nlVector<void*> PtrArray;
/**/
void nlVectorInit(PtrArray *a, unsigned int typeSize, unsigned int size);
void nlVectorCopy(PtrArray *a, const PtrArray *ta);
void nlVectorClear(PtrArray *a);
void nlVectorFree(PtrArray *a);
void nlVectorReserve(PtrArray *a, unsigned int capacity);
void nlVectorResize(PtrArray *a, unsigned int size);
void nlVectorAppend(PtrArray *a, const void* data);
void nlVectorAppend(PtrArray *a, const PtrArray &ta);
void nlVectorInsert(PtrArray *a, unsigned int index, const void* data);
void nlVectorRemoveAt(PtrArray *a, unsigned int index);
int nlVectorExists(const PtrArray *a, const void* data);

template<class T> 
class nlVector
{
public:
    /*
     * @brief CRTを利用できないため、グローバルでのコンストラクタが呼ばれない。
     *        不用意にそのような状態を避けるためにコンストラクタではなく
     *        init関数を用意している。
     */
    /**/
    __forceinline void init(unsigned int size=0)
    {
        nlVectorInit((PtrArray *)this, sizeof(T), size);
    }
    /**/
    __forceinline void init(const nlVector &a)
    {
        nlVectorCopy((PtrArray *)this, (const PtrArray *)&a);
    }
    /**/
    __forceinline ~nlVector()
    {
        free();
    }
    /**/
    __forceinline bool empty() const
    {
        return (m_size == 0);
    }
    /**/
    __forceinline void resize(unsigned int size)
    {
        nlVectorResize((PtrArray *)this, size);
    }

    __forceinline void reserve(unsigned int capacity)
    {
        nlVectorReserve((PtrArray *)this, capacity);
    }

    __forceinline void clear()
    {
        nlVectorClear((PtrArray *)this);
    }

    __forceinline void free()
    {
        nlVectorFree((PtrArray *)this);
    }

    __forceinline void append(const T &data)
    {
        nlVectorAppend((PtrArray *)this, (ePtr *)&data);
    }

    __forceinline void append(const nlVector &a)
    {
        for (unsigned int i=0; i<a.size(); i++)
        {
            append(a[i]);
        }
    }

    __forceinline void insert(unsigned int index, const T &data)
    {
        nlVectorInsert((PtrArray *)this, index, (ePtr *)&data);
    }

    __forceinline void removeAt(unsigned int index)
    {
        nlVectorRemoveAt((PtrArray *)this, index);
    }

    __forceinline int exists(const T &data) const
    {
        return nlVectorExists((PtrArray *)this, (ePtr *)&data);
    }

    __forceinline unsigned int size() const
    {
        return size_;
    }

    __forceinline unsigned int capacity() const
    {
        return capacity_;
    }

    __forceinline void reverse()
    {
        for (unsigned int i=0; i<m_size/2; i++)
        {
            eSwap(m_data[i], m_data[m_size-1-i]);
        }
    }

    __forceinline T & push_back(const T &data)
    {
        nlVectorAppend((PtrArray *)this, (void**)&data);
		return this->lastElement();
    }

    /*__forceinline T & push()
    {
		nlVectorReserve((PtrArray *)this, this->size() + 1);
		this->m_size++;
		return this->lastElement();
    }*/

    /*__forceinline T & pushFast()
    {
        return this->m_data[this->m_size++];
    }*/

    __forceinline T pop()
    {
		T val = lastElement();
		removeLastElement();
		return val;
    }

    __forceinline void removeLastElement()
    {
		size_--;
    }

    __forceinline T& lastElement()
    {
		return (*this)[size()-1];
    }

    /**/
    __forceinline void sort(bool (*predicate)(const T &a, const T &b))
    {
        for (unsigned int j=1; j<data_; j++)
        {
            const T key = m_data[j];
            eInt i = (eInt)j-1;

            while (i >= 0 && predicate(data_[i], key))
            {
                data_[i+1] = data_[i];
                i--;
            }

            data_[i+1] = key;
        }
    }

    __forceinline nlVector & operator = (const nlVector &a)
    {
        if (this != &a)
        {
            nlVectorFree((PtrArray *)this);
            nlVectorCopy((PtrArray *)this, (const PtrArray *)&a);
        }

        return *this;
    }

    __forceinline T & operator [] (int index)
    {
        NL_ASSERT(index >= 0 && (unsigned int)index < size_ );
        return data_[index];
    }

    __forceinline const T & operator [] (int index) const
    {
		NL_ASSERT(index >= 0 && (unsigned int)index < m_size);
        return data_[index];
    }

public:
    T* data_;
    nlUint32 size_;
    nlUint32 capacity_;
    nlUint32 typeSize_;
};

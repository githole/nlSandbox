#pragma once

/**
 * @brief 
 */
template <class T, nlInt32 container_size>
class nlArray
{
public:
    /**/
    T& operator[](nlInt32 i )
    {
        NL_ASSERT( 0 <= i && i < container_size );
        return e_[i];
    }
    /**/
    const T& operator[](nlInt32 i )const
    {
        NL_ASSERT( 0 <= i && i < container_size );
        return e_[i];
    }
    /**/
    nlInt32 size()const
    {
        return ARRAY_LEN;
    }
public:
    enum { ARRAY_LEN = container_size };
    T e_[ARRAY_LEN];
};

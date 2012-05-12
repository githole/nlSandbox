#pragma once

#include "shared/nlTypedef.h"
#include "shared/math/nlMath.h"

/**
* @brief パス補間方法 
*/
enum nlCurveType
{
    nlCurveType_Line, 
    nlCurveType_CatmulRom,
};
/**
* @brief パスポイント 
*/
struct nlCurvePoint
{
    nlUint32 time_; /* xの値(最終的にはmsec時間に相当する) */
    nlArray<nlFloat32,3> xyz_;
};
/**
* @brief パスポイント集合
* @TODO nlVEctorで代用できるか検討する
*/
struct nlCurvePoints
{
    nlInt32 len_;
    nlArray<nlCurvePoint,MAX_PATHPOINT_NUM> points_;
    //nlCurvePoint points_[MAX_PATHPOINT_NUM]; /* HACK 数は適当。エディタの出力をみるようにする。恐らく0xff固定 */

    nlUint32 size()const
    { return len_; }

    void push_back( 
        const nlCurvePoint& newPoint )
    {
        points_[len_] = newPoint;
        ++len_;
    }
    nlCurvePoint& operator[](nlInt32 idx)
    {
        return points_[idx];
    }
    const nlCurvePoint& operator[](nlInt32 idx)const
    {
        return points_[idx];
    }
    void erase(nlInt32 idx)
    {
        for( nlInt32 i=idx;i<len_-1;++i)
        {
            points_[i] = points_[i+1];
        }
        --len_;
    }
    /* idxの後ろにinsertする */
    void insert(nlInt32 idx, const nlCurvePoint& newPoint )
    {
        /* TODO array::insert/vector::insertなどを用意してその中で処理するようにする */
        memmove( &points_[0]+(idx+2), &points_[0]+(idx+1), sizeof(nlCurvePoint)*(len_-idx-1) );
        points_[idx+1] = newPoint;
        ++len_;
    }
    /* 長さを取得する */
    nlFloat32 length()const
    {
        const nlInt32 max = size();
        return (nlFloat32)(points_[max-1].time_);
    }
};
/**
 * @brief 
 */
struct nlCurve
{
    nlCurveType type_;
    nlCurvePoints points_;
};
typedef nlArray<nlCurve,MAX_PATH_NUM> nlCurves;

/**
 * @brief 指定したカーブの現在の値を取得する
 */
nlVec3 nlGetCurveValue( const nlCurve& path, nlUint32 x );

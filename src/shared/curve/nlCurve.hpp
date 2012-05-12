#pragma once

#include "shared/nlTypedef.h"
#include "shared/math/nlMath.h"

/**
* @brief �p�X��ԕ��@ 
*/
enum nlCurveType
{
    nlCurveType_Line, 
    nlCurveType_CatmulRom,
};
/**
* @brief �p�X�|�C���g 
*/
struct nlCurvePoint
{
    nlUint32 time_; /* x�̒l(�ŏI�I�ɂ�msec���Ԃɑ�������) */
    nlArray<nlFloat32,3> xyz_;
};
/**
* @brief �p�X�|�C���g�W��
* @TODO nlVEctor�ő�p�ł��邩��������
*/
struct nlCurvePoints
{
    nlInt32 len_;
    nlArray<nlCurvePoint,MAX_PATHPOINT_NUM> points_;
    //nlCurvePoint points_[MAX_PATHPOINT_NUM]; /* HACK ���͓K���B�G�f�B�^�̏o�͂��݂�悤�ɂ���B���炭0xff�Œ� */

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
    /* idx�̌���insert���� */
    void insert(nlInt32 idx, const nlCurvePoint& newPoint )
    {
        /* TODO array::insert/vector::insert�Ȃǂ�p�ӂ��Ă��̒��ŏ�������悤�ɂ��� */
        memmove( &points_[0]+(idx+2), &points_[0]+(idx+1), sizeof(nlCurvePoint)*(len_-idx-1) );
        points_[idx+1] = newPoint;
        ++len_;
    }
    /* �������擾���� */
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
 * @brief �w�肵���J�[�u�̌��݂̒l���擾����
 */
nlVec3 nlGetCurveValue( const nlCurve& path, nlUint32 x );

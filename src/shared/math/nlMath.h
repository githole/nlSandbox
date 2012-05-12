#pragma once

#include "shared/nlTypedef.h"
#include "shared/container/nlArray.hpp"
#include "shared/nlTypedef.h"

/**/
#define MN(a,b) (((a)<(b))?(a):(b))
#define MX(a,b) (((a)>(b))?(a):(b))

/**
* @brief 二次元ベクトル
*/
struct nlVec2
{ 
    nlFloat32 x;
    nlFloat32 y;
};

/**
* @brief 3次元ベクトル
* @TODO メソッドはnlMath.hに移動させる。
*/
struct nlVec3
{
    union
    {
        struct
        {
            nlFloat32 x_;
            nlFloat32 y_;
            nlFloat32 z_;
        };
        nlArray<nlFloat32,3> e_;
    };
    static nlVec3 Create( nlFloat32 x, nlFloat32 y, nlFloat32 z )
    {
        return nlVec3(x,y,z);
    }
	nlVec3(nlFloat32 x, nlFloat32 y, nlFloat32 z)
	{
		x_ = x;
		y_ = y;
		z_ = z;
	}
	nlVec3(nlFloat32* v)
	{
		x_ = v[0];
		y_ = v[1];
		z_ = v[2];
	}
	nlVec3(){}

    nlFloat32 operator[](nlInt32 idx)
    {
        return *((&x_)+idx);
    }
    /**/
    nlVec3 operator + (const nlVec3& rhs) const
    {
        return nlVec3 (
            this->x_ + rhs.x_,
            this->y_ + rhs.y_,
            this->z_ + rhs.z_
        );
    }
    /**/
    nlVec3 operator -(const nlVec3& rhs) const
    {
        return nlVec3(
            this->x_ - rhs.x_,
            this->y_ - rhs.y_,
            this->z_ - rhs.z_ 
        );
    }
	void normalize()
	{
		extern nlFloat32 nlSqrt( nlFloat32 src );
		nlFloat32 d = nlSqrt(x_ * x_ + y_ * y_ + z_ * z_);
		x_ /= d;
		y_ /= d;
		z_ /= d;
	}

	nlVec3 cross(const nlVec3& t) const
	{
		return nlVec3(
			y_ * t.z_ - z_ * t.y_,
			z_ * t.x_ - x_ * t.z_,
			x_ * t.y_ - y_ * t.x_);
	}

	nlFloat32 dot(const nlVec3& t) const
	{
		return x_ * t.x_ + y_ * t.y_ + z_ * t.z_;
	}

	const nlVec3& operator+=(const nlVec3& t)
	{
		x_ += t.x_;
		y_ += t.y_;
		z_ += t.z_;
		return *this;
	}
	
};

/**
* @brief 4次元ベクトル
* @TODO メソッドはnlMath.hに移動させる。
*/
struct nlVec4
{
    union
    {
        struct  
        {
            nlFloat32 x_;
            nlFloat32 y_;
            nlFloat32 z_;
            nlFloat32 w_;
        };
        nlArray<nlFloat32,4>e_;
        //nlFloat32 e_[4];
    };    

    static nlVec4 create( nlFloat32 x, nlFloat32 y, nlFloat32 z, nlFloat32 w)
    {
        nlVec4 newV4 = {x,y,z,w};
        return newV4;
    }

    nlVec4& operator = ( const nlVec3& src )
    {
        this->x_ = src.x_;
        this->y_ = src.y_;
        this->z_ = src.z_;
        this->w_ = 1.0f;
        return *this;
    }

    nlVec3 xyz()const
    {
        return nlVec3(x_,y_,z_);
    }
    /**/
    nlVec4 operator+(const nlVec4& lhs )
    {
        const nlFloat32 x = this->x_ + lhs.x_;
        const nlFloat32 y = this->y_ + lhs.y_;
        const nlFloat32 z = this->z_ + lhs.z_;
        const nlFloat32 w = this->w_ + lhs.w_;
        return nlVec4::create(x,y,z,w);
    }
    /**/
    nlVec4 operator*( nlFloat32 value )
    {
        const nlFloat32 x = this->x_*value;
        const nlFloat32 y = this->y_*value;
        const nlFloat32 z = this->z_*value;
        const nlFloat32 w = this->w_*value;
        return nlVec4::create(x,y,z,w);
    }
};
/**
* @brief 4元数
* @TODO メソッドはnlMath.hに移動させる。
*/
struct nlQuaternion
{
    nlFloat32 x_;
    nlFloat32 y_;
    nlFloat32 z_;
    nlFloat32 w_;

    static nlQuaternion create(nlFloat32 x,nlFloat32 y,nlFloat32 z,nlFloat32 w)
    {
        nlQuaternion newQtn = {x,y,z,w};
        return newQtn;
    }
};

typedef D3DXMATRIX    nlMatrix;
extern const D3DMATRIX matIdentity;

/**/
nlVec2 nlVec2_create(nlFloat32 x, nlFloat32 y);
nlVec3 nlVec3_create(nlFloat32 x, nlFloat32 y, nlFloat32 z);
nlVec4 nlVec4_create(nlFloat32 x, nlFloat32 y, nlFloat32 z, nlFloat32 w);

/* Math Functions */
int nlClamp( int value, int min, int max );
nlFloat32 nlSqrt( nlFloat32 src );
nlFloat32 nlLen( const nlVec3& src );
void nlNorm( nlVec3& src );
nlFloat32 nlSin( nlFloat32 angle );
nlFloat32 nlCos( nlFloat32 angle );
nlFloat32 nlTan( nlFloat32 angle );
void nlSincos( nlFloat32 angle, nlFloat32* sinCos );
nlFloat32 nlAtan( nlFloat32 y );
nlFloat32 nlAtan2( nlFloat32 x, nlFloat32 y );
nlFloat32 nlAsin( nlFloat32 x );
nlFloat32 nlLog2f( const nlFloat32 x );
nlFloat32 nlExpf( const nlFloat32 x );
nlFloat32 nlFmodf( const nlFloat32 x, const nlFloat32 y );
nlFloat32 nlPowf( const nlFloat32 x, const nlFloat32 y );
int ifloorf( const nlFloat32 x );
int nlStrlen( const nlInt8 *ori );
/**
 * @brief strcmpオリジナル版。同値の場合は0を、そうでない場合は-1を返す仕様であることに注意。
 */
int nlStrcmp( const nlInt8* lhs, const nlInt8* rhs );
void nlStrcat( nlInt8* dst, const nlInt8* src );
void rotateVec3ByQuaternion( nlVec3* outPoint, const nlVec3* inPoint, const D3DXQUATERNION* rotation );
int nlAbs( int value );
nlFloat32 nlAbsf( nlFloat32 src );
/**/
void cartesianTospherical( const nlVec3& cart, nlFloat32* radius, nlFloat32* theta, nlFloat32* fai );
/**/
const int NLRAND_LOCAL_MAX = 32767;
void nlSetRandSeed_Local( nlUint32 seed );
unsigned long nlRand_Local(void);
unsigned long nlRand();
/**/
//nlUint32 nlCurvePoints_size(const nlCurvePoints* this_);
//void nlCurvePoints_push_back( const nlCurvePoint& newPoint, nlCurvePoints* this_ );
//void nlCurvePoints_erase(int idx,nlCurvePoints* this_);
//void nlCurvePoints_insert(int idx, const nlCurvePoint& newPoint, const nlCurvePoints* this_ );
//float nlCurvePoints_length(const nlCurvePoints* this_);

//------------------------------------------------------------
//These function use by enable (inline and __fastcall)
//float To Half-float Funtion
inline nlInt16 __fastcall ftohf(nlFloat32 fl)
{
	return ((nlInt16)((*(nlInt32*)(&fl))>>16));
}

//Half-float To float Funtion
inline nlFloat32 __fastcall hftof(short sh)
{
	int sfl = (sh<<16);
	return *((nlFloat32*)(&sfl));
}
//------------------------------------------------------------
//These function use by enable (inline and __fastcall)
//Degree To char
inline nlInt8 __fastcall dtoc(nlFloat32 degree)
{
	return (nlInt8)((degree/180.0f)*128);
}

//char To Degree
inline nlFloat32 __fastcall ctod(nlInt8 ch)
{
	return ((ch/128.0f)*180.0f);
}

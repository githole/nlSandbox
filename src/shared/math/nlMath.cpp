/**/
#include "main/stdafx.h"
#include "shared/math/nlMath.h"

#pragma warning(disable:4725) /**/

__declspec(align(16)) const float matIdentity2[16] = 
{
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f,
};
/**/
nlVec2 nlVec2_create(nlFloat32 x, nlFloat32 y)
{
    nlVec2 newVec = {x,y};
    return newVec;
}
/**/
nlVec3 nlVec3_create(nlFloat32 x, nlFloat32 y, nlFloat32 z)
{
    return nlVec3(x,y,z);
}
/**/
nlVec4 nlVec4_create(nlFloat32 x, nlFloat32 y, nlFloat32 z, nlFloat32 w)
{
    nlVec4 newVec = {x,y,z,w};
    return newVec;
}
/* TODO この関数を削除する。qoobに同様のものがある */
int nlClamp( int value, int min, int max )
{
    if( value < min )
    { return min; }
    else if( value < max)
    { return value; }
    else
    { return max; }
}
/**/
nlFloat32 nlSqrt( nlFloat32 src )
{
    _asm fld src;
    _asm fsqrt;
}
nlFloat32 nlLen( const nlVec3& src )
{
    const nlFloat32 x = src.x_;
    const nlFloat32 y = src.y_;
    const nlFloat32 z = src.z_;
    return nlSqrt(x*x+y*y+z*z);
}
/**/
void nlNorm( nlVec3& src )
{
    const nlFloat32 len = nlLen( src );
    src.x_ /= len;
    src.y_ /= len;
    src.z_ /= len;
}
/**/
nlFloat32 nlSin( nlFloat32 angle )
{
    _asm fld angle;
    _asm fsin;
}
/**/
nlFloat32 nlCos( nlFloat32 angle )
{
    _asm fld angle;
    _asm fcos;
}
/**/
nlFloat32 nlTan( nlFloat32 angle )
{
    _asm fld angle;
    _asm fptan;
    _asm fstp st(0); // st(0)は不要なので出す
}
/**/
void nlSincos( nlFloat32 angle, nlFloat32* sinCos ) 
{
    _asm fld dword ptr [angle]; 
    _asm fsincos;
    _asm fstp dword ptr [sinCos+0];
    _asm fstp dword ptr [sinCos+4]; 
}
/**/
nlFloat32 nlAtan( nlFloat32 y )
{
    _asm fld y;
    _asm fld1;
    _asm fpatan;
}
/**/
nlFloat32 nlAtan2( nlFloat32 x, nlFloat32 y )
{
    _asm fld y;
    _asm fld x;
    _asm fpatan;
}
/**/
nlFloat32 nlAsin( nlFloat32 x )
{
    nlFloat32 f1 = x;
    nlFloat32 f2 = nlSqrt( 1 - x * x );
    _asm fld f1;
    _asm fld f2;
    _asm fpatan;
}
/**/
nlFloat32 nlLog2f( const nlFloat32 x )
{
    nlFloat32 res;
    _asm fld    dword ptr [x];
    _asm fld1;
    _asm fxch   st(1);
    _asm fyl2x;
    _asm fstp   dword ptr [res];

    return res;
}
/**/
nlFloat32 nlExpf( const nlFloat32 x )
{
    nlFloat32 res;
    _asm fld     dword ptr [x];
    _asm fldl2e;
    _asm fmulp   st(1), st(0);
    _asm fld1;
    _asm fld     st(1);
    _asm fprem;
    _asm f2xm1;
    _asm faddp   st(1), st(0);
    _asm fscale;
    _asm fxch    st(1);
    _asm fstp    st(0);
    _asm fstp    dword ptr [res];
    return res;
}
/**/
nlFloat32 nlFmodf( const nlFloat32 x, const nlFloat32 y )
{
    nlFloat32 res;
    _asm fld     dword ptr [y];
    _asm fld     dword ptr [x];
    _asm fprem;
    _asm fxch    st(1);
    _asm fstp    st(0);
    _asm fstp    dword ptr [res];
    return res;
}
/**/
nlFloat32 nlPowf( const nlFloat32 x, const nlFloat32 y )
{
    nlFloat32 res;
    _asm fld     dword ptr [y];
    _asm fld     dword ptr [x];
    _asm fyl2x;
    _asm fld1;
    _asm fld     st(1);
    _asm fprem;
    _asm f2xm1;
    _asm faddp   st(1), st(0);
    _asm fscale;
    _asm fxch;
    _asm fstp    st(0);
    _asm fstp    dword ptr [res];;
    return res;
}
/**/
int ifloorf( const nlFloat32 x )
{
    static short opc1 = 0x043f ;     // floor
    int res;
    short tmp;
    _asm fstcw   word  ptr [tmp]
    _asm fld     dword ptr [x]
    _asm fldcw   word  ptr [opc1]
    _asm fistp   dword ptr [res]
    _asm fldcw   word  ptr [tmp]
    return res;
}
/**/
int nlStrlen( const nlInt8 *ori )
{
    int res;
    _asm mov esi, ori;
    _asm xor ecx, ecx;
    _asm myloop:;
    _asm mov al, [esi];
    _asm inc esi;
    _asm inc ecx;
    _asm test al, al;
    _asm jnz myloop;
    _asm dec ecx;
    _asm mov [res], ecx;

    return res;    
}
/**/
int nlStrcmp( const nlInt8* lhs, const nlInt8* rhs )
{
    /**/
    unsigned int lhsLen = nlStrlen(lhs);
    unsigned int rhsLen = nlStrlen(rhs);
    if( rhsLen != rhsLen )
    { return -1; }
    /**/
    for( int i=0;i<rhsLen;++i)
    {
        if( lhs[i]!=rhs[i] )
        {
            return -1;
        }
    }
    return 0;
}

/**/
void nlStrcat( nlInt8* dst, const nlInt8* src )
{
    nlInt8* start = dst + nlStrlen( dst );
    const unsigned int copyLen = nlStrlen( src ) + 1;
    for( int i=0;i<copyLen;++i)
    {
        start[i] = src[i];
    }
}

/**/
void rotateVec3ByQuaternion( 
                            nlVec3* outPoint, 
                            const nlVec3* inPoint, 
                            const D3DXQUATERNION* rotation )
{    
    const nlFloat32 vx  = inPoint->x_;
    const nlFloat32 vy  = inPoint->y_;
    const nlFloat32 vz  = inPoint->z_;
    const nlFloat32 rx  = rotation->x;
    const nlFloat32 ry  = rotation->y;
    const nlFloat32 rz  = rotation->z;
    const nlFloat32 rw  = rotation->w;
    const nlFloat32 xy  = rx*ry;
    const nlFloat32 xz  = rx*rz;
    const nlFloat32 xw  = rx*rw;
    const nlFloat32 yz  = ry*rz;
    const nlFloat32 yw  = ry*rw;
    const nlFloat32 zw  = rz*rw;
    const nlFloat32 x2  = rx*rx;
    const nlFloat32 y2  = ry*ry;
    const nlFloat32 z2  = rz*rz;
    const nlFloat32 w2  = rw*rw;
    const nlFloat32 x2y2= x2 - y2;
    const nlFloat32 w2z2= w2 - z2;
    const nlFloat32 invs = 1.0f / (x2 + y2 + z2 + w2);
    outPoint->x_ = (2.0f * ((yw + xz) * vz + (xy - zw) * vy) + (x2y2 + w2z2) * vx)*invs;
    outPoint->y_ = (2.0f * ((xy + zw) * vx + (yz - xw) * vz) + (w2z2 - x2y2) * vy)*invs;
    outPoint->z_ = (2.0f * ((yz + xw) * vy + (xz - yw) * vx) + (z2 + w2 - x2 - y2) * vz)*invs; 
}
/* TODO qoobに同様の関数があるのでこれは削除する */
int nlAbs( int value )
{
    if( value > 0 ){ return value; }
    else           { return -value; }
}
/* abs */
nlFloat32 nlAbsf( nlFloat32 src )
{
    nlFloat32 result;
    _asm fld src;
    _asm fabs;
    _asm fstp result;
    return result;
}
/**/
void cartesianTospherical( const nlVec3& cart, nlFloat32* radius, nlFloat32* theta, nlFloat32* fai )
{
    const nlFloat32 x = cart.x_;
    const nlFloat32 y = cart.y_;
    const nlFloat32 z = cart.z_;
    *theta = 0.0f;
    *fai = 0.0f;
    *radius = nlSqrt(x*x+y*y+z*z);
    if( *radius == 0.0f )
    {
        return ;
    }
    const nlFloat32 A = z/ *radius;
    *theta = nlAsin( nlSqrt(1-A*A) );
    /**/
    const nlFloat32 xyLen = nlSqrt(x*x+y*y);
    if( xyLen == 0.0f )
    {
        *fai = 0.0f;
        return;
    }
    *fai = nlAsin( y/xyLen );
}
/* random */
static unsigned int localRandSeed_ = 0;
void nlSetRandSeed_Local( unsigned int seed )
{
    localRandSeed_ = seed;
}
/**/
unsigned long nlRand_Local( void )
{
    localRandSeed_ = localRandSeed_ * 1103515245 + 12345;
    unsigned long result = ((unsigned)(localRandSeed_/65536) % 32768);
    return result;
}
/**/
unsigned long nlRand()
{
    static unsigned long x=123456789,y=362436069,z=521288629,w=88675123; 
    unsigned long t; 
    t=(x^(x<<11));x=y;y=z;z=w; 
    return( w=(w^(w>>19))^(t^(t>>8)) ); 
}

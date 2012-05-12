#include "main/stdafx.h"
#include "shared/curve/nlCurve.hpp"
#include "shared/math/nlMath.h"

/**/
nlVec3 nlGetPathValueAsLine( const nlCurve& path, nlUint32 t )
{
    nlVec3 retXyz;
    nlFloat32* xyz = (nlFloat32*)&retXyz;
    xyz[0] = 0.0f;
    xyz[1] = 0.0f;
    xyz[2] = 0.0f;

    const nlCurvePoints& points = path.points_;
    int endPos = 0;
    if( points.size() != 0 )
    {
        const nlCurvePoint& pathPoint = points.points_[points.size()-1];
        endPos = pathPoint.time_;
    }
    else
    { 
        return nlVec3::Create(0.0f,0.0f,0.0f); 
    }

    
    nlUint32 cx = MN( t,(nlUint32)(endPos-1) );
    nlUint32 idx=0;
    for( idx=0;idx<points.size();++idx )
    {
        if( cx < points[idx].time_ )
        { break; }
    }
    idx = idx%points.size();
    int   pi = (idx-1)%points.size();
    int   px = points[pi].time_;
    const nlArray<nlFloat32,3>& pv =  points[pi].xyz_;
    int   nx = points[idx].time_;
    const nlArray<nlFloat32,3>& nv = points[idx].xyz_;

    const nlFloat32 xpip = (nlFloat32)(cx-px)/(nlFloat32)(nx-px); /*xPosInPercentage*/
    for( int i=0;i<3;++i)
    {
        xyz[i] = (xpip*nv[i] + (1.0f-xpip)*pv[i]);
    }

    return retXyz;
}
/**/
nlVec3 nlGetPathValueAsCatmullRom( const nlCurve& path, nlUint32 t )
{
    nlVec3 retXyz;
    nlFloat32* xyz = (nlFloat32*)&retXyz;
    xyz[0] = 0.0f;
    xyz[1] = 0.0f;
    xyz[2] = 0.0f;

    const nlCurvePoints& points = path.points_;
    if( points.size() == 0 )
    {
        return nlVec3::Create(0.0f,0.0f,0.0f);
    }
    const nlUint32 lastFrame = points[points.size()-1].time_;
    if( lastFrame <= t )
    {
        const nlArray<nlFloat32,3>& lastXYZ = points[points.size()-1].xyz_;
        xyz[0] = lastXYZ[0];
        xyz[1] = lastXYZ[1];
        xyz[2] = lastXYZ[2];
        return retXyz;
    }

    /* ëOå„ÇÃÉtÉåÅ[ÉÄÇíTÇ∑ */
    nlArray<nlInt32,4> frames;
    nlFloat32 s = 0.0f;
    for( nlUint32 i=0;i<points.size(); ++i )
    {
        if( points[i].time_ > t )
        {
            
            const nlInt32 tmp  = i-2;
            const nlInt32 tmp2 = i-1;
            const nlInt32 tmp3 = i+1;
            frames[0] = MX(tmp,0);
            frames[1] = MX(tmp2,0);
            frames[2] = i;
            frames[3] = (nlInt32)MN(tmp3,(nlInt32)i);

            s = (nlFloat32)(t-points[frames[1]].time_)/(nlFloat32)( points[frames[2]].time_-points[frames[1]].time_);
            break;
        }
    }
    D3DXVec3CatmullRom( 
        (D3DXVECTOR3*)&retXyz, 
        (D3DXVECTOR3*)&points[frames[0]].xyz_, 
        (D3DXVECTOR3*)&points[frames[1]].xyz_, 
        (D3DXVECTOR3*)&points[frames[2]].xyz_, 
        (D3DXVECTOR3*)&points[frames[3]].xyz_,
        s );

    return retXyz;
}
/**/
nlVec3 nlGetCurveValue( const nlCurve& path, nlUint32 t )
{
    switch( path.type_ )
    {
    case nlCurveType_Line:
        return nlGetPathValueAsLine(path,t);
    case nlCurveType_CatmulRom:
        return nlGetPathValueAsCatmullRom(path,t);
    default:
        return nlVec3_create(0,0,0);
    }
}

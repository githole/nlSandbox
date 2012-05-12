#include "main/stdafx.h"
#include "tool/nlCurveSerializer.hpp"
#include "tool/nlUtil.hpp"
#include "shared/nlLog.hpp"
#include "shared/nlTypedef.h"
#include "nlLogComposer.hpp"

/**/
template<class Archive>
inline void serialize(
                      Archive & ar, 
                      nlCurvePoint & pathPoint, 
                      unsigned int ver )
{
    ar & pathPoint.time_;
    ar & pathPoint.xyz_[0];
    ar & pathPoint.xyz_[1];
    ar & pathPoint.xyz_[2];
}
/**/
template<class Archive>
inline void serialize(
                      Archive & ar, 
                      nlCurvePoints & pathPoints, 
                      unsigned int ver )
{
    switch( ver )
    {
    case 0:
        NL_ASSERT(!"このバージョンは読めません");
        break;
    case 1:
        ar & pathPoints.len_;
        /* この64をMAX_PATHPOINT_NUMにしてはいけない */
        for( int i=0;i<64;++i)
        {
            ar & pathPoints.points_[i];
        }
        break;
    default:
        break;
    }
}
BOOST_CLASS_VERSION(nlCurvePoints, 1);
/**/
template<class Archive>
inline void serialize(
                      Archive & ar, 
                      nlCurve & path, 
                      unsigned int ver )
{
    if( ver == 0 )
    {
        ar & path.type_;
        ar & path.points_;
    }
    else if( ver == 1 )
    {
        ar & path.type_;
        ar & path.points_;
    }
    else if( ver == 2 )
    {
        ar & path.type_;
        ar & path.points_;
        std::string dummy;
        ar & dummy;
    }
    else if( ver == 3 )
    {
        ar & path.type_;
        ar & path.points_;
    }
}
BOOST_CLASS_VERSION(nlCurve, 3);

/**/
static bool serializeAsBinary( const char* fileName, const nlCurve& newCurve );
static bool serializeAsKdf( const char* fileName, const nlCurve& curve );
static bool serializeAsXML( const char* fileName, const nlCurve& newCurve );
static bool deserializeAsBinary( const char* fileName, nlCurve& newCurve );
static bool deserializeAsKdf( const char* fileName, nlCurve& curve );
static bool deserializeAsXML( const char* fileName, nlCurve& newCurve );

/**/
bool serializeCurve( const char* fileName, const nlCurve& newCurve, nlCurveSeliazerType seliazerType )
{
    /*
     @note KDF形式の場合、予期したものと違う形で保存されることが分かったので一時停止
     */
    /**/
    switch( seliazerType )
    {
    case nlCST_Binary:  return serializeAsBinary( fileName, newCurve );
    case nlCST_KDF:     return serializeAsKdf( fileName, newCurve );
    case nlCST_XML:     return serializeAsXML( fileName, newCurve );
    default: NL_ASSERT(!"対応していないシリアライズ形式");return false;
    }

    return false;
}
/**/
static bool serializeAsBinary( const char* fileName, const nlCurve& newCurve )
{
    std::ofstream file( fileName );
    boost::archive::text_oarchive oa(file);
    oa << (const nlCurve&)newCurve;
    return true;
}
/**/
static bool serializeAsKdf( const char* fileName, const nlCurve& curve )
{
    /**/
    if( curve.points_.size() == 0 )
    {
        NL_ERR(ERR_024,fileName);
        return false;
    }
    const nlCurvePoints& points = curve.points_;
    
    const unsigned int finalFrameInMs = points[points.size()-1].time_;
    /**/
    QString strLine;
    QFile file(fileName);
    if( !file.open(QIODevice::WriteOnly|QIODevice::Text) )
    {
        NL_ERR(ERR_023,fileName);
        return false;
    }
    /* #scenetime */
    file.write("#scenetime\n");
    strLine.sprintf("%d\n",finalFrameInMs);
    file.write(strLine.toLocal8Bit());
    /* #curvetype */
    file.write("#curvetype\n");
    switch( curve.type_ )
    {
    case nlCurveType_Line:
        file.write("LINER\n");
        break;
    case nlCurveType_CatmulRom:
        file.write("CATMULROM\n");
        break;
    default:
        NL_ASSERT(!"存在しないカーブタイプ");
        break;
    }
    /* #numkeyframe */
    file.write("#numkeyframe\n");
    strLine.sprintf("%d\n",curve.points_.size());
    file.write(strLine.toLocal8Bit());
    /* #keyframes */
    file.write("#keyframes\n");
    for( int i=0;i<points.size();++i)
    {
        
        const float rate = (float)points[i].time_/(float)finalFrameInMs;
        strLine.sprintf("%f\n", rate );
        file.write(strLine.toLocal8Bit());
    }
    /* #positions */
    file.write("#positions\n");
    for( int i=0;i<points.size();++i)
    {
        const nlArray<float,3>& xyz = points[i].xyz_;
        strLine.sprintf("%f %f %f\n", xyz[0],xyz[1],xyz[2] );
        file.write(strLine.toLocal8Bit());
    }
    /**/
    return true;
}
/**/
static bool serializeAsXML( const char* fileName, const nlCurve& newCurve )
{ 
    return false;
}
/**/
bool deserializeCurve( const char* fileName, nlCurve& newCurve, nlCurveSeliazerType seliazerType )
{
    switch( seliazerType )
    {
    case nlCST_Binary:  return deserializeAsBinary( fileName, newCurve );
    case nlCST_KDF:     return deserializeAsKdf( fileName, newCurve );
    case nlCST_XML:     return deserializeAsXML( fileName, newCurve );
    default: NL_ASSERT(!"対応していないシリアライズ形式");return false;
    }
    return false;
}
/**/
static bool deserializeAsBinary( const char* fileName, nlCurve& newCurve )
{
    std::ifstream file( fileName );
    try
    {
        boost::archive::text_iarchive ia(file);
        ia >> newCurve;
        return true;
    }
    /* ファイルがない等などの場合は、適当なもので埋める。 */
    catch ( boost::archive::archive_exception& e)
    {
        NL_ERR(ERR_020,fileName);
        newCurve.type_ = nlCurveType_Line;
        for( int i=0;i<2;++i)
        {
            float x = 0.0f;
            float y = 0.0f;
            float z = 0.0f;
            nlCurvePoint point;
            point.time_ = 100.0f*i;
            point.xyz_[0] = x;
            point.xyz_[1] = y;
            point.xyz_[2] = z;
            newCurve.points_.push_back( point );
        }
    }
    return false;
}
/**/
static bool deserializeAsKdf( const char* fileName, nlCurve& curve )
{
    QFile file(fileName);
    QString line;
    nlCurve emptyCurve;
    emptyCurve.type_ = nlCurveType_Line;
    emptyCurve.points_.len_ = 0;
    /**/
    if ( !file.open(QIODevice::ReadOnly|QIODevice::Text) ) 
    { 
        NL_ERR(ERR_022,-1,fileName);
        curve = emptyCurve;
        return false;
    }
    /**/
    int sceneTimeInMs = 0;
    nlCurveType curveType;
    /**/
    int state = 0;
    int currentKeyIndex = 0;
    QTextStream text( &file );
    while ( !text.atEnd() ) 
    {
        QString lineStr = text.readLine();
        QByteArray line = lineStr.toLocal8Bit();
        //printf(line.data());

        switch( state )
        {
        case 0: /* #scenetime */
            if( lineStr != "#scenetime" )
            { 
                NL_ERR(ERR_022,state,fileName);
                curve = emptyCurve;
                return false;
            }
            /**/
            ++state;
            break;
        case 1: /* シーンタイム(%d) */
            sceneTimeInMs = atoi( line.data() );
            ++state;
            break;
        case 2: /* #curvetype */
            if( lineStr != "#curvetype" )
            { 
                NL_ERR(ERR_022,state,fileName);
                curve = emptyCurve;
                return false;
            }
            /**/
            ++state;
            break;
        case 3: /* 補完方法 */
            if( lineStr == "LINER" )
            { curve.type_ = nlCurveType_Line; }
            else if( lineStr =="CATMULROM" )
            { curve.type_ = nlCurveType_CatmulRom; }
            else
            {
                NL_ERR(ERR_022,state,fileName);
                curve = emptyCurve;
                return false;
            }
            ++state;
            break;
        case 4:/* #numkeyframe */
            if( lineStr != "#numkeyframe" )
            { 
                NL_ERR(ERR_022,state,fileName);
                curve = emptyCurve;
                return false;
            }
            ++state;
            break;
        case 5:/* キーフレーム数取得 */
            {
                const int numKeyFrame = atoi( line.data() );
                if( numKeyFrame == -1 )
                { 
                    NL_ERR(ERR_022,state,fileName);;
                    curve = emptyCurve;
                    return false;
                }
                curve.points_.len_ = numKeyFrame;
                ++state;
            }
            break;
        case 6: /* キーフレーム開始 */
            if( lineStr != "#keyframes" )
            { 
                NL_ERR(ERR_022,state,fileName);
                curve = emptyCurve;
                return false;
            }
            ++state;
            break;
        case 7: /* キーフレームを取得 */
            {
                /**/
                float timing = 0.0f;
                if( sscanf_s( line.data(), "%f", &timing ) != 1 )
                {
                    NL_ERR(ERR_022,state,fileName);
                    curve = emptyCurve;
                    return false;
                }
                /**/
                curve.points_.points_[currentKeyIndex].time_ = (int)(timing*sceneTimeInMs);
                /**/
                ++currentKeyIndex;
                if( currentKeyIndex == curve.points_.len_ )
                {
                    ++state;
                    currentKeyIndex = 0;
                }
            }
            break;
        case 8:
            if( lineStr != "#positions" )
            { 
                NL_ERR(ERR_022,state,fileName);
                curve = emptyCurve;
                return false;
            }
            ++state;
            break;
        case 9:/* 各位置取得 */
            {
                const nlArray<float,3>& xyz = curve.points_.points_[currentKeyIndex].xyz_;
                if( sscanf_s( line.data(), "%f %f %f", &xyz[0], &xyz[1], &xyz[2] ) != 3 )
                {
                    NL_ERR(ERR_022,state,fileName);
                    curve = emptyCurve;
                    return false;
                }
                /**/
                ++currentKeyIndex;
                if( currentKeyIndex == curve.points_.len_ )
                {
                    ++state;
                }
            }
            break;
        case 10:
            /* 特に何もしない */
            break;
        }
    }
    /**/
    if( state != 10 )
    {
        NL_ERR(ERR_022,-2,fileName);
        curve = emptyCurve;
        return false;
    }
    /**/
    return true;
}

/**/
static bool deserializeAsXML( const char* fileName, nlCurve& newCurve )
{
    return false; 
}
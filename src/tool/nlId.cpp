#include "main/stdafx.h"
#include "nlId.hpp"
#include "shared/nlTypedef.h"
#include "shared/nlLog.hpp"
#include "tool/nlLogComposer.hpp"

using namespace std;
using namespace boost;

namespace
{
    IdsGroups g_idsGroups;
};
/**/
class InitId
{
public:
    InitId()
    {
        const char* idTags[] =
        {
            "VS_NAME", "GS_NAME", "PS_NAME", "CS_NAME", "TS_NAME",
            "CURVE_NAME", "MESH_NAME"
        };
        const Ids emptyIds;
        BOOST_FOREACH( const char* id, idTags )
        {
            g_idsGroups.insert(IdsGroups::value_type( id, emptyIds ) );
        }
    }
};
static InitId initId;
/**/
void setId( const QString& groupName, const QString& tagName, int id )
{
    IdsGroups::iterator groupIte = g_idsGroups.find(groupName);
    if( groupIte == g_idsGroups.end() )
    {
        const Ids emptyIds;
        g_idsGroups.insert(IdsGroups::value_type( groupName, emptyIds ) );
        groupIte = g_idsGroups.find(groupName);
    }
    Ids& group = groupIte->second;
    /**/
    Ids::iterator idIte = group.find( tagName );
    if( idIte != group.end() )
    {
        QByteArray baTagName = tagName.toLocal8Bit();
        NL_LOG( ERR_008, baTagName.data() );
        return ;
    }
    group.insert( Ids::value_type(tagName,id) );

    /* ログ */
    QByteArray baGroupName = groupName.toLocal8Bit();
    QByteArray baTagName   = tagName.toLocal8Bit();
    NL_LOG( INF_004, baTagName.data(), baGroupName.data() );
}
/**/
void setId( const QString& groupName, const QString& tagName )
{
    IdsGroups::iterator groupIte = g_idsGroups.find(groupName);
    if( groupIte == g_idsGroups.end() )
    {
        const Ids emptyIds;
        g_idsGroups.insert(IdsGroups::value_type( groupName, emptyIds ) );
        groupIte = g_idsGroups.find(groupName);
    }
    Ids& group = groupIte->second;
    /**/
    Ids::iterator idIte = group.find( tagName );
    if( idIte != group.end() )
    {
        QByteArray baTagName = tagName.toLocal8Bit();
        NL_LOG( ERR_002, baTagName.data() );
        return ;
    }
    int lastIndex = -1;
    idIte = group.begin();
    while( idIte != group.end() )
    { 
        lastIndex = max( idIte->second, lastIndex ); 
        ++idIte;
    }
    /**/
    group.insert( Ids::value_type(tagName,lastIndex+1) );
    /* ログ */
    QByteArray baGroupName = groupName.toLocal8Bit();
    QByteArray baTagName   = tagName.toLocal8Bit();
    NL_LOG( INF_003, baTagName.data(), baGroupName.data() );
}
/**/
void clearId( const QString& groupName )
{
    IdsGroups::iterator groupIte = g_idsGroups.find(groupName);
    if( groupIte == g_idsGroups.end() )
    {
        QByteArray baGroupName = groupName.toLocal8Bit();
        NL_LOG(ERR_004, baGroupName.data() );
        return;
    }
    Ids& group = groupIte->second;
    group.clear();

    /* ログ */
    QByteArray baGroupName = groupName.toLocal8Bit();
    NL_LOG( INF_002, baGroupName.data() );
}
/**/
boost::optional<int> getId( const QString& groupName, const QString& tagName )
{
    /**/
    QByteArray baTagName = tagName.toLocal8Bit();
    QByteArray baGroupName = groupName.toLocal8Bit();
    /**/
    optional<int> retValue;
    IdsGroups::iterator groupIte = g_idsGroups.find(groupName);
    if( groupIte == g_idsGroups.end() )
    {
        NL_LOG( ERR_012, baGroupName.data() );
        return retValue;
    }
    Ids group = groupIte->second;
    /**/
    Ids::iterator idIte = group.find( tagName );
    if( idIte == group.end() )
    {
        NL_LOG( ERR_011, baTagName.data(), baGroupName.data() );
        return retValue;
    }
    /* グループの検索 */
    retValue = idIte->second;
    /* ログ */
    //NL_LOG( INF_001, baTagName.data(), baGroupName.data() );
    /**/
    return retValue;
}
/**/
optional<QString> getTag( const QString& groupName, int id )
{
     /**/
    QByteArray baGroupName = groupName.toLocal8Bit();
    /**/
    optional<QString> retValue;
    IdsGroups::iterator groupIte = g_idsGroups.find(groupName);
    if( groupIte == g_idsGroups.end() )
    {
        NL_LOG( ERR_012, baGroupName.data() );
        return retValue;
    }
    Ids group = groupIte->second;
    /**/
    Ids::iterator idIte = group.begin();
    while( idIte != group.end() )
    {
        if( idIte->second == id )
        {
            return idIte->first;
        }
        ++idIte;
    }
    return retValue;
}
/**/
const Ids& getIds( const QString& groupName )
{
    /**/
    QByteArray baGroupName = groupName.toLocal8Bit();
    /**/
    IdsGroups::iterator groupIte = g_idsGroups.find(groupName);
    if( groupIte == g_idsGroups.end() )
    {
        //NL_LOG( ERR_012, baGroupName.data() );
        static Ids nullgroup;
        return nullgroup;
    }
    return groupIte->second;
}
/**/
const IdsGroups& getAllIds()
{
    return g_idsGroups;
}

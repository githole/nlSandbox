#include "main/stdafx.h"
#include "tool/nlModelScripLoader.h"
#include "shared/mesh/nlPoly.h"
#include "shared/nlTypedef.h"


using namespace std; 

/**/
bool createMeshCommand( const QString& path, npCommands& commands )
{
    /* ファイルから読み込み */
    QFile file(path);
    file.open( QIODevice::Text | QIODevice::ReadOnly );
    QByteArray allData = file.readAll();
    QList<QByteArray> tmp = allData.split('\n');
    QList<QList<QByteArray>> lines;
    for( int i=0;i<tmp.size();++i)
    {
        QList<QByteArray> oneline = tmp[i].split(' ');
        lines.push_back(oneline);
    }
    /**/
    for(unsigned int i=0;i<lines.size();++i)
    {
        npCommand& cmd = commands[i];
        QList<QByteArray>& commandStr = lines[i];
        QString commandName;
        float props[NP_COMMAND_PROP] = {};
        if(commandStr.size() > 0 ){ commandName = commandStr[0]; } 
        if(commandStr.size() > 1 ){ props[0] = commandStr[1].toFloat(); } 
        if(commandStr.size() > 2 ){ props[1] = commandStr[2].toFloat(); } 
        if(commandStr.size() > 3 ){ props[2] = commandStr[3].toFloat(); } 
        if(commandStr.size() > 4 ){ props[3] = commandStr[4].toFloat(); } 

        if( commandName == "NOP" )
        {
            /* 終了？ */
            break;
        }
        else if( commandName == "move" )
        {            
            /* xyz */
            cmd.props[0] = props[0];
            cmd.props[1] = props[1];
            cmd.props[2] = props[2];
            /**/
            cmd.commandId = NP_COMMAND_ID_MOVE_POS;
        }
        else if( commandName == "resetPos" )
        {
            cmd.commandId = NP_COMMAND_ID_RESET_POS;
        }
        else if( commandName == "scale" )
        {
            /* xyz */
            cmd.props[0] = props[0];
            cmd.props[1] = props[1];
            cmd.props[2] = props[2];
            /**/
            cmd.commandId = NP_COMMAND_ID_SCALE;
        }
        else if( commandName == "resetScale" )
        {
            /**/
            cmd.commandId = NP_COMMAND_ID_RESET_SCALE;
        }
        else if( commandName == "rot" )
        {
            /* yaw pitch roll */
            cmd.props[0] = props[0];
            cmd.props[1] = props[1];
            cmd.props[2] = props[2];
            /**/
            cmd.commandId = NP_COMMAND_ID_ROTATE;
        }
        else if( commandName == "plane" )
        {
            /* num segment */
            cmd.props[0] = props[0];
            /**/
            cmd.commandId = NP_COMMAND_ID_PLANE;
        }
        else if( commandName == "box" )
        {
            /**/
            cmd.commandId = NP_COMMAND_ID_BOX;
        }
        else if( commandName == "shaft" )
        {
            /* seg */
            cmd.props[0] = props[0];
            /**/
            cmd.commandId = NP_COMMAND_ID_SHAFT;
        }
        else if( commandName == "" )
        {
            /* 空行はスキップ */
        }
        else
        {
            /* TODO ログにエラーを出す */
            return false;
        }
    }
    return true;
}
/**/
static const char* getCommandName( nlPolyCommandId id )
{
    struct
    {
        nlPolyCommandId id_;
        const char* name_;
    }idStr[] =
    {
        {NP_COMMAND_ID_END, "end"},
        {NP_COMMAND_ID_MOVE_POS, "move"},
        {NP_COMMAND_ID_RESET_POS, "resetPos"},
        {NP_COMMAND_ID_SCALE, "scale"},
        {NP_COMMAND_ID_RESET_SCALE, "resetScale"},
        {NP_COMMAND_ID_ROTATE, "rotate"},
        {NP_COMMAND_ID_PLANE, "plane"},
        {NP_COMMAND_ID_BOX, "box"},
        {NP_COMMAND_ID_BALL, "ball"},
        {NP_COMMAND_ID_SHAFT, "shaft"},
        {NP_COMMAND_ID_TAURUS, "taurus"},
        {NP_COMMAND_ID_TUBE, "tube"},
        {NP_COMMAND_ID_CONE, "cone"},
        {NP_COMMAND_ID_ROUND_BOX, "roundbox"},
        {NP_COMMAND_ID_ROUND_SHAFT, "roundshaft"},
        {NP_COMMAND_ID_SUBDIV,"subdiv"},
        {NP_COMMAND_ID_UV_FACE, "uvFace"},
        {NP_COMMAND_ID_UV_SPHEARE, "uvSphera"},
        {NP_COMMAND_ID_UV_CYLINDER, "uvCylinder"},
    };
    for( int i=0;i<sizeof(idStr)/sizeof(*idStr);++i)
    {
        if( idStr[i].id_ == id )
        {
            return idStr[i].name_;
        }
    }
    return "unknown";
}
/**/
void printMeshCommands( const npCommands& commands )
{
    //log(QString("load all command"));
    //for( unsigned int i=0;i<numCommand;++i)
    {
        QString logStr;
        //logStr.sprintf( " id[%s] props[%2.2f][%2.2f][%2.2f][%2.2f]",
            /*getCommandName(command[i].commandId),
            command[i].props[0], command[i].props[1],
            command[i].props[2], command[i].props[3] );*/
        //log(logStr);
    }
}
/**/
static vector<string> split(string src)
{
    vector<string> result;
    int index = 0;
    while( (index = src.find_first_of(" ")) != src.npos )
    {
        if(index>0)
        { result.push_back(src.substr(0, index)); }
        src = src.substr(index + 1);
    }
    if(src.length() > 0)
    { result.push_back(src); }

    return result;
}

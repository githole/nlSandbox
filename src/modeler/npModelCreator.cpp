#include "main/stdafx.h"
#include "npModelCreator.h"
#include "log.h"
#include "nlPoly.h"

using namespace std;

static void createCommand( vector<vector<string>> loadData, npCommands& commands );
static vector<vector<string>> loadAllCommand(const char* fileName );
static vector<string> split(string src);
static void printCommands( const npCommands& command, unsigned int numCommand );

void loadNewModel( QString path, npTriangleMesh* newMesh )
{
    /* デバッグ表示 */
    log(QString("start load")+path);

    /* ファイルを読み込む */
    std::string pathString =  path.toStdString();
    vector<vector<string>> loadData = loadAllCommand( pathString.c_str() );
    /* パースする */
    npCommands commands = {};
    createCommand( loadData, commands );
    printCommands( commands, loadData.size() );
    
    /* ポリゴンを生成する */
    npQuadMesh* quadMesh = new npQuadMesh();
    npCreate( commands, quadMesh );
    /* とりあえず面法線のみ */
    npConvertQuadToTriangleUseFaceNormal( quadMesh, newMesh );
    //npConvertQuadToTriangleUseIqTechnique( quadMesh, newMesh );
    delete quadMesh;
}

/**/
static void createCommand( vector<vector<string>> loadData, npCommands& commands )
{
    for(unsigned int i=0;i<loadData.size();++i)
    {
        npCommand& cmd = commands[i];
        vector<string> commandStr = loadData[i];
        string commandName = "";
        float props[NP_COMMAND_PROP] = {};
        if(commandStr.size() > 0 ){ commandName = commandStr[0]; } 
        if(commandStr.size() > 1 ){ props[0] = atof(commandStr[1].c_str()); } 
        if(commandStr.size() > 2 ){ props[1] = atof(commandStr[2].c_str()); } 
        if(commandStr.size() > 3 ){ props[2] = atof(commandStr[3].c_str()); } 
        if(commandStr.size() > 4 ){ props[3] = atof(commandStr[4].c_str()); } 

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
        else
        {
            /* TODO ログにエラーを出す */
        }
    }
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
static void printCommands( const npCommands& command, unsigned int numCommand )
{
    log(QString("load all command"));
    for( unsigned int i=0;i<numCommand;++i)
    {
        QString logStr;
        logStr.sprintf( " id[%s] props[%2.2f][%2.2f][%2.2f][%2.2f]",
            getCommandName(command[i].commandId),
            command[i].props[0], command[i].props[1],
            command[i].props[2], command[i].props[3] );
        log(logStr);
    }
}
/**/
static vector<vector<string>> loadAllCommand(const char* fileName )
{
    vector<vector<string>> result;
    FILE* file = fopen( fileName, "rt" );
    if( !file ){ return result; }
    /**/
    char lineBuffer[MAX_PATH] = {};
    while( fgets( lineBuffer, MAX_PATH, file ) )
    {
        /* remnove new line */
        if( lineBuffer[strlen(lineBuffer)-1] == 0x0a )
        { lineBuffer[strlen(lineBuffer)-1] = NULL; }
        /**/
        vector<string> line = split( lineBuffer );
        result.push_back( line );
    }
    /**/
    fclose( file );
    return result;
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

#pragma once

#include "shared/nlTypedef.h"
#include "shared/nlEngineContext.hpp"

/* max mesh vertex */
#define NP_MAX_VERTEX_NUM (8192)
/* max mesh face */
#define NP_MAX_FACE_NUM   (8192)
/* command props */
#define NP_COMMAND_PROP   (4)
/* max command */
#define NP_MAX_COMMNAD    (256)
/* PI */
#define NP_PI 3.1415926535f

/* command id */
enum nlPolyCommandId
{
    NP_COMMAND_ID_END = 0,
    NP_COMMAND_ID_MOVE_POS,
    NP_COMMAND_ID_RESET_POS,
    NP_COMMAND_ID_SCALE,
    NP_COMMAND_ID_RESET_SCALE,
    NP_COMMAND_ID_ROTATE,
    NP_COMMAND_ID_PLANE,
    NP_COMMAND_ID_BOX,
    NP_COMMAND_ID_BALL,
    NP_COMMAND_ID_SHAFT,
    NP_COMMAND_ID_TAURUS,
    NP_COMMAND_ID_TUBE,
    NP_COMMAND_ID_CONE,
    NP_COMMAND_ID_ROUND_BOX,
    NP_COMMAND_ID_ROUND_SHAFT,
    NP_COMMAND_ID_SUBDIV,
    NP_COMMAND_ID_UV_FACE,
    NP_COMMAND_ID_UV_SPHEARE,
    NP_COMMAND_ID_UV_CYLINDER,
};

/* all command */
struct npCommand
{
    nlPolyCommandId commandId;
    nlFloat32 props[NP_COMMAND_PROP];
};
typedef npCommand npCommands[NP_MAX_COMMNAD];

/**/
struct MeshVertex
{
    nlFloat32 pos[4];
    nlFloat32 normal[4];
    nlFloat32 uv[4];
};

/**
* @brief quad poly mesh 
*/
struct npQuadMesh
{
    unsigned int numVert;
    unsigned int numFace;
    /* vertex */
    struct npVertex
    {
        nlFloat32 position[3];
        nlFloat32 uv[3];
    }vertex[NP_MAX_VERTEX_NUM];
    /* face normals */
    nlFloat32 normal[NP_MAX_FACE_NUM][3];
    unsigned int indexes[NP_MAX_FACE_NUM][4];
};

/**
* @brief triangle poly mesh 
*/
struct npTriangleMesh
{
    unsigned int numVert;
    unsigned int numFace;
    struct npVertex
    {
        nlFloat32 position[3];
        nlFloat32 normal[3];
        nlFloat32 uv[3];
    }vertex[NP_MAX_VERTEX_NUM];
    unsigned int indexes[NP_MAX_FACE_NUM][3];
};

/**
* @brief generate polygon
*/
void npCreateQuad( const npCommands& commands, npQuadMesh* mesh );
/**
* @brief convert quad poly to triangle poly
*/
//void npConvertQuadToTriangle( const npQuadMesh* quad, npTriangleMesh* triangle );

/* convert to triangles use face normal */
void npConvertQuadToTriangleUseFaceNormal( const npQuadMesh* aQuad, npTriangleMesh* aTriangle );

/* use iq technique http://www.iquilezles.org/www/articles/normals/normals.htm */
void npConvertQuadToTriangleUseIqTechnique( const npQuadMesh* aQuad, npTriangleMesh* aTriangle );

/**/
void npConstructD3DMesh( const npTriangleMesh* triMesh, nlMesh* newMesh, nlEngineContext* cxt );

#include "main/stdafx.h"
#include "nlPoly.h"

/**/
#if 1
#define npSin sin
#define npCos cos
#include <math.h>

#endif

/**/
static void npAssign3( nlFloat32 dst[3], const nlFloat32 src[3] );
static void npCross3( const nlFloat32 v0[3], const nlFloat32 v1[3], nlFloat32 result[3] );
static void npNormal3( nlFloat32 v[3] );
static __forceinline void createPlane( unsigned int aSeg, npQuadMesh* poly );
static __forceinline void createBox( npQuadMesh* poly );
static __forceinline void createBall( unsigned int segLongitude, unsigned int segLatitude, npQuadMesh* poly );
static __forceinline void createShaft( unsigned int aSeg, npQuadMesh* poly );
static void scaleRotTranceVertex( nlFloat32 trance[3], nlFloat32 scale[3], nlFloat32 rot[3], npQuadMesh* poly, unsigned int startIdx, unsigned int endIndex );
/**/
static void npAssign3( nlFloat32 dst[3], const nlFloat32 src[3] )
{
    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
}
/**/
static void npCross3( const nlFloat32 v0[3], const nlFloat32 v1[3], nlFloat32 result[3] )
{
    result[0] = v0[1]*v1[2] - v0[2]*v1[1];
    result[1] = v0[2]*v1[0] - v0[0]*v1[2];
    result[2] = v0[0]*v1[1] - v0[1]*v1[0];
}
/**/
static void npNormal3( nlFloat32 v[3] )
{
    const nlFloat32 len = sqrt( v[0]*v[0] + v[1]*v[1] + v[2]*v[2] );
    v[0] /= len;
    v[1] /= len;
    v[2] /= len;
}
/**/
void npCreateQuad( const npCommands& commands, npQuadMesh* poly )
{
    /**/
    nlFloat32 xyz[3]   = {0.0f,0.0f,0.0f};
    nlFloat32 rot[3]   = {0.0f,0.0f,0.0f};
    nlFloat32 scale[3] = {1.0f,1.0f,1.0f};
    poly->numFace = 0;
    poly->numVert = 0;
    /* current index of vertexbuffer */
    //unsigned int vertIndex = 0;
    /* current index of face */
    //unsigned int faceIndex = 0;
    /* process all commands */
    for( int i=0;i<NP_MAX_COMMNAD;++i)
    {
        const npCommand& cmd = commands[i];
        switch( cmd.commandId )
        {
        case NP_COMMAND_ID_END:
            /* do nothing */
            break;
        case NP_COMMAND_ID_MOVE_POS:
            xyz[0] += cmd.props[0];
            xyz[1] += cmd.props[1];
            xyz[2] += cmd.props[2];
            break;
        case NP_COMMAND_ID_RESET_POS:
            xyz[0] = 0.0f;
            xyz[1] = 0.0f;
            xyz[2] = 0.0f;
            break;
        case NP_COMMAND_ID_SCALE:
            scale[0] = cmd.props[0];
            scale[1] = cmd.props[1];
            scale[2] = cmd.props[2];
            break;
        case NP_COMMAND_ID_RESET_SCALE:
            scale[0] = 1.0f;
            scale[1] = 1.0f;
            scale[2] = 1.0f;
            break;
        case NP_COMMAND_ID_ROTATE:
            rot[0] = cmd.props[0];
            rot[1] = cmd.props[1];
            rot[2] = cmd.props[2];
            break;
        case NP_COMMAND_ID_PLANE:
            {
                const unsigned int startIdx = poly->numVert;
                createPlane( (unsigned int)cmd.props[0], poly );
                const unsigned int endIdx = poly->numVert;
                scaleRotTranceVertex( xyz, scale, rot, poly, startIdx, endIdx);
            }
            break;
        case NP_COMMAND_ID_BOX:
            {
                const unsigned int startIdx = poly->numVert;
                createBox( poly );
                const unsigned int endIdx = poly->numVert;
                scaleRotTranceVertex( xyz, scale, rot, poly, startIdx, endIdx);
            }
            break;
        case NP_COMMAND_ID_BALL:
            {
                const unsigned int segLongitude  = (unsigned int)cmd.props[0];
                const unsigned int segLatitude   = (unsigned int)cmd.props[1];
                createBall( segLongitude, segLatitude, poly );
            }
            break;
        case NP_COMMAND_ID_SHAFT:
            {
                const unsigned int startIdx = poly->numVert;
                createShaft( (unsigned int)cmd.props[0], poly );
                const unsigned int endIdx = poly->numVert;
                scaleRotTranceVertex( xyz, scale, rot, poly, startIdx, endIdx);
            }
            break;
        case NP_COMMAND_ID_TAURUS:
            break;
        case NP_COMMAND_ID_TUBE:
            break;
        case NP_COMMAND_ID_CONE:
            break;
        case NP_COMMAND_ID_ROUND_BOX:
            break;
        case NP_COMMAND_ID_ROUND_SHAFT:
            break;
        case NP_COMMAND_ID_SUBDIV:
            //subdiv(poly);
            break;
        case NP_COMMAND_ID_UV_FACE:
            break;
        case NP_COMMAND_ID_UV_SPHEARE:
            break;
        case NP_COMMAND_ID_UV_CYLINDER:
            break;
        default:
            /* TODO insert assert */
            break;
        }
    }

    /* compute normal */
    npQuadMesh& quad = *poly;
    for( unsigned int fi=0;fi<quad.numFace;++fi)
    {
        npQuadMesh::npVertex& v0 = quad.vertex[quad.indexes[fi][0]];
        npQuadMesh::npVertex& v1 = quad.vertex[quad.indexes[fi][1]];
        npQuadMesh::npVertex& v2 = quad.vertex[quad.indexes[fi][2]];
        npQuadMesh::npVertex& v3 = quad.vertex[quad.indexes[fi][3]];
        nlFloat32 subV0[3], subV1[3];
        subV0[0] = v0.position[0]-v2.position[0];
        subV0[1] = v0.position[1]-v2.position[1];
        subV0[2] = v0.position[2]-v2.position[2];
        subV1[0] = v1.position[0]-v3.position[0];
        subV1[1] = v1.position[1]-v3.position[1];
        subV1[2] = v1.position[2]-v3.position[2];
        npCross3( subV0, subV1, quad.normal[fi] );
        npNormal3(quad.normal[fi]);
    }
}
/**/
static void scaleRotTranceVertex( nlFloat32 trance[3], nlFloat32 scale[3], nlFloat32 rot[3], npQuadMesh* poly, unsigned int startIdx, unsigned int endIndex )
{
    for( unsigned int vi=startIdx;vi<endIndex;++vi)
    {
        nlFloat32 tmp[3];
        nlFloat32* v = poly->vertex[vi].position;
        /* scale */
        v[0] *= scale[0];
        v[1] *= scale[1];
        v[2] *= scale[2];

        /* rotation. yaw->pitch->roll = Z->X->Y */
        const nlFloat32 yaw   = rot[2];
        const nlFloat32 pitch = rot[0];
        const nlFloat32 roll  = rot[1];
        /* yaw */
        tmp[0] = v[0]*npCos(yaw) - v[1]*npSin(yaw);
        tmp[1] = v[0]*npSin(yaw) + v[1]*npCos(yaw);
        v[0] = tmp[0];
        v[1] = tmp[1];
        /* pitch */
        tmp[1] = v[1]*npCos(pitch) - v[2]*npSin(pitch);
        tmp[2] = v[1]*npSin(pitch) + v[2]*npCos(pitch);
        v[1] = tmp[1];
        v[2] = tmp[2];
        /* roll */
        tmp[0] =  v[0]*npCos(roll) + v[2]*npSin(roll);
        tmp[2] = -v[0]*npSin(roll) + v[2]*npCos(roll);
        v[0] = tmp[0];
        v[2] = tmp[2];

        /* trance */
        v[0] += trance[0];
        v[1] += trance[1];
        v[2] += trance[2];
        v[3] = 1.0f;
    }
}
/**/
/**/
static __forceinline void createPlane( unsigned int aSeg, npQuadMesh* poly )
{
    /* ------------------------------------- */
    /* FIXME: aVertexIndex base vertexindex  */
    /* ------------------------------------- */
    /**/
    unsigned int segI = aSeg;
    nlFloat32 segF = (nlFloat32)segI;
    /* set vertex */
    for( unsigned int y=0;y<segI+1;++y)
    {
        for( unsigned int x=0;x<segI+1;++x)
        {
            const nlFloat32 fx =  (nlFloat32)x/segF - 0.5f;
            const nlFloat32 fy =  (nlFloat32)y/segF - 0.5f;
            const unsigned int vi = x+y*(segI+1);
            poly->vertex[vi].position[0] =  fx;
            poly->vertex[vi].position[1] =  0.0f;
            poly->vertex[vi].position[2] =  fy;
        }
    }
    /* set index */
    /* NOTE ignore last line, so doesn't "y<segI+1" but "y<segI". */
    for( unsigned int y=0;y<segI;++y)
    {
        for( unsigned int x=0;x<segI;++x)
        {
            const unsigned int startIdx = x+(y*segI);
            poly->indexes[startIdx][0] = startIdx+1;
            poly->indexes[startIdx][1] = startIdx;
            poly->indexes[startIdx][2] = startIdx+segI+1;
            poly->indexes[startIdx][3] = startIdx+segI+2;
        }
    }
    /**/
    poly->numVert += (segI+1)*(segI+1);
    poly->numFace += segI*segI; 
}
/**/
static __forceinline void createBox( npQuadMesh* poly )
{
    /* set vertex */
    for( int vi=0;vi<8;++vi)
    {
        const unsigned int svi = poly->numVert;
        poly->vertex[svi].position[0] =  ((nlFloat32)(((vi+1)/2)%2)-0.5f);
        poly->vertex[svi].position[1] =  ((nlFloat32)(vi/4)-0.5f);
        poly->vertex[svi].position[2] =  ((nlFloat32)((vi/2)%2)-0.5f);
    }

    /* HACK more smart index */
    /* set index */
    const unsigned int fi = poly->numFace;
    poly->indexes[fi][0] = 0;
    poly->indexes[fi][1] = 1;
    poly->indexes[fi][2] = 2;
    poly->indexes[fi][3] = 3;

    poly->indexes[fi+1][0] = 0;
    poly->indexes[fi+1][1] = 4;
    poly->indexes[fi+1][2] = 5;
    poly->indexes[fi+1][3] = 1;

    poly->indexes[fi+2][0] = 1;
    poly->indexes[fi+2][1] = 5;
    poly->indexes[fi+2][2] = 6;
    poly->indexes[fi+2][3] = 2;

    poly->indexes[fi+3][0] = 2;
    poly->indexes[fi+3][1] = 6;
    poly->indexes[fi+3][2] = 7;
    poly->indexes[fi+3][3] = 3;

    poly->indexes[fi+4][0] = 3;
    poly->indexes[fi+4][1] = 7;
    poly->indexes[fi+4][2] = 4;
    poly->indexes[fi+4][3] = 0;

    poly->indexes[fi+5][0] = 7;
    poly->indexes[fi+5][1] = 6;
    poly->indexes[fi+5][2] = 5;
    poly->indexes[fi+5][3] = 4;

    /**/
    poly->numVert += 8;
    poly->numFace += 6;
}
/**/
static __forceinline void createBall( unsigned int segLongitude, unsigned int segLatitude, npQuadMesh* poly )
{
    (void)segLongitude;
    (void)segLatitude;
    (void)poly;
    /* set vertex */

    /* set index */

    /* update vertex/face num */
}
/**/
static __forceinline void createShaft( unsigned int aSeg, npQuadMesh* poly )
{
    /**/
    const unsigned int& vertIndex = poly->numVert;
    const unsigned int& faceIndex = poly->numFace;
    /* set segment lower bound 0 -> 3 */
    unsigned int segI = aSeg+3;
    nlFloat32 seg = (nlFloat32)(segI);
    /* make upper/lower cover vertex*/
    for( unsigned int i=0;i<segI*2;++i)
    {
        const nlFloat32 fi = (nlFloat32)i;
        const nlFloat32 theta = NP_PI/seg * fi * 2.0f;
        poly->vertex[vertIndex+i].position[0] = npSin( theta );
        /**/
        if(i < segI ){ poly->vertex[vertIndex+i].position[1] =  0.5f; }
        else         { poly->vertex[vertIndex+i].position[1] = -0.5f; }
        //poly->vertex[vertIndex+i].position[1] = ((nlFloat32)((i+seg-1)/seg))-0.5f;
        /**/
        poly->vertex[vertIndex+i].position[2] = npCos( theta );
    }
    /* make center vertex */
    poly->vertex[vertIndex+segI*2].position[0] =  0.0f;
    poly->vertex[vertIndex+segI*2].position[1] =  0.5f;
    poly->vertex[vertIndex+segI*2].position[2] =  0.0f;
    poly->vertex[vertIndex+segI*2+1].position[0] =  0.0f;
    poly->vertex[vertIndex+segI*2+1].position[1] = -0.5f;
    poly->vertex[vertIndex+segI*2+1].position[2] =  0.0f;

    /* make index(upper cover) */
    for( int i=0;i<seg;i++)
    {
        unsigned int* idx = poly->indexes[faceIndex+i];
        idx[0] = vertIndex+segI*2;
        idx[1] = vertIndex+(i);
        idx[2] = vertIndex+(i+1)%(segI);
        idx[3] = idx[2]; /* this index will be removed at polygonize process */
    }

    /* make index(lower cover) */
    for( int i=0;i<seg;i++)
    {
        unsigned int* idx = poly->indexes[faceIndex+segI+i];
        idx[0] = vertIndex+segI*2+1;
        idx[1] = vertIndex+segI+(i);
        idx[2] = vertIndex+segI+(i+1)%(segI);
        idx[3] = idx[2]; /* this index will be removed at polygonize process */
    }

    /* make side index */
    for( int i=0;i<seg;i++)
    {
        unsigned int* idx = poly->indexes[faceIndex+segI*2+i];
        idx[0] = vertIndex+0   +(i);
        idx[1] = vertIndex+0   +(i+1)%(segI);
        idx[2] = vertIndex+segI+(i+1)%(segI);
        idx[3] = vertIndex+segI+(i);
    }
    /**/
    poly->numVert  += segI*2+2;
    poly->numFace  += segI*3;
}
/**/
static __forceinline void subdiv(npQuadMesh* poly)
{
    (void)poly;
    /*
    NOTE
    Because of diffuclty of subdiv of triangle poly.
    Implimatation stoped.
    */
}
/**/
void npConvertQuadToTriangleUseFaceNormal( const npQuadMesh* aQuad, npTriangleMesh* aTriangle )
{
    aTriangle->numFace = 0;
    aTriangle->numVert = 0;
    /* current vertex index */
    unsigned int& cvi = aTriangle->numVert;
    /* vertex list-nize */
    for( unsigned int fi=0;fi<aQuad->numFace;++fi)
    {
        const unsigned int* idx = aQuad->indexes[fi];
        /* have Degenerate Polygon */
        const bool isDeg = (idx[2]==idx[3]);
        const nlFloat32* normal = aQuad->normal[fi]; 
        /* simply copy first 3 vertex */
        for(unsigned int ii=0;ii<3;++ii)
        {
            npTriangleMesh::npVertex& tv = aTriangle->vertex[cvi];
            const npQuadMesh::npVertex& qv = aQuad->vertex[idx[ii]];
            npAssign3( tv.position, qv.position );
            npAssign3( tv.normal, normal); 
            npAssign3( tv.uv, qv.uv );
            ++cvi;
        }
        /* if divide to 2 polygons, add more one polygon. */
        if( !isDeg )
        {
            npTriangleMesh::npVertex* tv = aTriangle->vertex;
            const npQuadMesh::npVertex* qv = aQuad->vertex;
            /**/
            unsigned int iis[] = {0,2,3};
            for( unsigned int ii=0;ii<3;++ii)
            {
                npAssign3( tv[cvi].position, qv[idx[iis[ii]]].position );
                npAssign3( tv[cvi].normal, normal); 
                npAssign3( tv[cvi].uv, qv[idx[iis[ii]]].uv );
                ++cvi;
            }
        }
    }
    /**/
    aTriangle->numFace = aTriangle->numVert/3;
    /**/
    for( unsigned int vi=0;vi<aTriangle->numVert;++vi)
    {
        aTriangle->indexes[vi/3][vi%3] = vi;
    }
}
/**/
void npConvertQuadToTriangleUseIqTechnique( const npQuadMesh* aWuad, npTriangleMesh* aTriangle )
{
    /**/
    const npQuadMesh& quad = *aWuad;
    npTriangleMesh& triMesh = *aTriangle;
    /* copy all vertex */
    for( unsigned int i=0;i<quad.numVert;++i)
    {
        triMesh.vertex[i].position[0] = quad.vertex[i].position[0];
        triMesh.vertex[i].position[1] = quad.vertex[i].position[1];
        triMesh.vertex[i].position[2] = quad.vertex[i].position[2];
        triMesh.vertex[i].position[3] = 1.0;
    }
    /* copy index */
    unsigned int triII  = 0;
    for( unsigned int i=0;i<quad.numFace;i++)
    {
        /* 
        when include dummy index, remove it.
        ex)
        0
        /   \
        /     \
        1-------2
        and index data is (0,1,2,2).
        get rid of double of 2 and simply triangle is (0,1,2).
        */
        if( quad.indexes[i][2] == quad.indexes[i][3] )
        {
            const unsigned int idx0 = quad.indexes[i][0];
            const unsigned int idx1 = quad.indexes[i][1];
            const unsigned int idx2 = quad.indexes[i][2];
            triMesh.indexes[triII][0] = idx0;
            triMesh.indexes[triII][1] = idx1;
            triMesh.indexes[triII][2] = idx2;
            ++triII;
        }
        /* simply copy index */
        else
        {
            triMesh.indexes[triII][0] = quad.indexes[i][0];
            triMesh.indexes[triII][1] = quad.indexes[i][1];
            triMesh.indexes[triII][2] = quad.indexes[i][2];
            ++triII;
            /**/
            triMesh.indexes[triII][0] = quad.indexes[i][0];
            triMesh.indexes[triII][1] = quad.indexes[i][2];
            triMesh.indexes[triII][2] = quad.indexes[i][3];
            ++triII;
        }
    }

    /* set num vert/face */
    triMesh.numVert = quad.numVert;
    triMesh.numFace = triII;

    /* calc normal */
    npTriangleMesh::npVertex* v = aTriangle->vertex;
    unsigned int(*idx)[3] =  aTriangle->indexes;
    /**/
    for( unsigned int vi=0;vi<aTriangle->numVert;++vi)
    {
        v[vi].normal[0] = 0.0f;
        v[vi].normal[1] = 0.0f;
        v[vi].normal[2] = 0.0f;
    }
    /**/
    for( unsigned fi=0;fi<aTriangle->numFace;++fi)
    {
        const unsigned int ia = idx[fi][0];
        const unsigned int ib = idx[fi][1];
        const unsigned int ic = idx[fi][2];
        /**/
        const nlFloat32 e1[3] =
        {
            v[ia].position[0] - v[ib].position[0],
            v[ia].position[1] - v[ib].position[1],
            v[ia].position[2] - v[ib].position[2],
        };
        const nlFloat32 e2[3] =
        {
            v[ic].position[0] - v[ib].position[0],
            v[ic].position[1] - v[ib].position[1],
            v[ic].position[2] - v[ib].position[2],
        };
        nlFloat32 normal[3];
        npCross3( e1, e2, normal);
        /**/
        for(unsigned int j=0;j<3;++j)
        {
            v[ia].normal[j] += normal[j];
            v[ib].normal[j] += normal[j];
            v[ic].normal[j] += normal[j];
        }
    }
    /**/
    for( unsigned int vi=0;vi<aTriangle->numVert;++vi)
    {
        npNormal3(v[vi].normal);
    }
}



/**/
void npConstructD3DMesh( const npTriangleMesh* triMesh, nlMesh* newMesh, nlEngineContext* cxt )
{
    /* ’¸“_”/–Ê”‚ª0‚Ìê‡‚Í‰½‚à‚µ‚È‚¢ */
    if( triMesh->numVert == 0 || triMesh->numFace == 0 )
    {
        return;
    }
    /**/
    ID3D11Device* d3dDevice  = cxt->d3dDevice;
    ID3D11DeviceContext* d3dContext = cxt->d3dContext;
    /* create vertex buffer */
    D3D11_BUFFER_DESC descVB =
    {
        triMesh->numVert*sizeof(MeshVertex),
        D3D11_USAGE_DYNAMIC,
        D3D11_BIND_VERTEX_BUFFER,
        D3D11_CPU_ACCESS_WRITE,
        0, 0
    };
    D3D11_SUBRESOURCE_DATA* subResourceVB = NULL;
    NL_HR_VALID( d3dDevice->CreateBuffer( &descVB, subResourceVB, &newMesh->vretexes_ ) );

    /* create index buffer */
    D3D11_BUFFER_DESC descIB =
    {
        sizeof(unsigned int)*triMesh->numFace*3,
        D3D11_USAGE_DYNAMIC,
        D3D11_BIND_INDEX_BUFFER,
        D3D11_CPU_ACCESS_WRITE,
        0, 0
    };
    D3D11_SUBRESOURCE_DATA* pSubResourceIB = NULL;
    NL_HR_VALID( d3dDevice->CreateBuffer(&descIB, pSubResourceIB, &newMesh->indices_ ) );

    /* copy vertex buffer */
    D3D11_MAPPED_SUBRESOURCE mappedVB;
    NL_HR_VALID( d3dContext->Map( newMesh->vretexes_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedVB ) );
    MeshVertex* dstVertex = (MeshVertex*)mappedVB.pData;
    for( unsigned int vi=0;vi<triMesh->numVert;++vi)
    {
        const npTriangleMesh::npVertex& srcVertex = triMesh->vertex[vi];
        dstVertex->pos[0] = srcVertex.position[0];
        dstVertex->pos[1] = srcVertex.position[1];
        dstVertex->pos[2] = srcVertex.position[2];
        dstVertex->pos[3] = 1.0f;
        /**/
        dstVertex->normal[0] = srcVertex.normal[0];
        dstVertex->normal[1] = srcVertex.normal[1];
        dstVertex->normal[2] = srcVertex.normal[2];
        dstVertex->normal[3] = 1.0f;
        /**/
        ++dstVertex;
    }
    d3dContext->Unmap( newMesh->vretexes_, 0);

    /* copy index */
    D3D11_MAPPED_SUBRESOURCE mappedIB;
    NL_HR_VALID( d3dContext->Map( newMesh->indices_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedIB ) );
    unsigned int* dstIndex = (unsigned int*)mappedIB.pData;
    for( unsigned int ii=0;ii<triMesh->numFace;++ii)
    {
        dstIndex[ii*3+0] = triMesh->indexes[ii][0];
        dstIndex[ii*3+1] = triMesh->indexes[ii][1];
        dstIndex[ii*3+2] = triMesh->indexes[ii][2];
    }
    d3dContext->Unmap( newMesh->indices_, 0);
    newMesh->numIndex_ = triMesh->numFace * 3;
    /**/
    QString finishMes;
    finishMes.sprintf( "vert:[%d] face:[%d]", triMesh->numVert, triMesh->numFace );
    //log(finishMes);
    //log("finish construct mesh");
}

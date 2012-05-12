#include "main/stdafx.h"
#include "shared/mesh/nlMesh.hpp"
#include "shared/nlTypedef.h"
#include "shared/nlMemory.h"
#include "shared/nlEngineContext.hpp"
#include "kmdLoader.hpp"

/**
 * @brief
 */
class NLMLoader
{
private:
	   
	NLMVertex* m_vertexbuf;
	unsigned int* m_indexbuf;
	unsigned int m_vertex_num;
	unsigned int m_index_num;
	typedef unsigned int Index;
public:
    NLMLoader()
    {
        m_vertexbuf = 0;
        m_indexbuf  = 0;
    }
    ~NLMLoader()
    {
        nlFree( m_vertexbuf );
        nlFree( m_indexbuf );
    }
    bool Load(const void* buf, const unsigned int size)
    {
		KMDObject kmd;
		bool r = kmd.Load(static_cast<const nlUint8*>(buf));
		if (!r)
			return false;
        
		unsigned int vertexNum = kmd.GetVertexNum();
		m_vertex_num = vertexNum;

		// copy real vertex format
        m_vertexbuf = (NLMVertex*)nlMalloc(sizeof(NLMVertex)*vertexNum);
		const nlVec3* v = kmd.GetVertex();
		const nlUint8* id = kmd.GetID();
        for (unsigned int i = 0; i < vertexNum; ++i)
        {
			m_vertexbuf[i].pos[0]    = v[i].x_;
            m_vertexbuf[i].pos[1]    = v[i].y_;
            m_vertexbuf[i].pos[2]    = v[i].z_;
            m_vertexbuf[i].pos[3]    = 1.0f;
            m_vertexbuf[i].color     = 0xFF080808;
            m_vertexbuf[i].normal[0] = 0.0f;
            m_vertexbuf[i].normal[1] = 0.0f;
            m_vertexbuf[i].normal[2] = 0.0f;
            m_vertexbuf[i].normal[3] = 0.0f;
            m_vertexbuf[i].materialID= id[i];
        }
        
        // copy real index format
		unsigned int indexNum = kmd.GetIndexNum();
		m_index_num = indexNum;
		m_indexbuf = (Index*)nlMalloc(sizeof(Index)*indexNum);
		nlMemcpy(m_indexbuf, kmd.GetIndex(), indexNum * sizeof(Index));
        
		
		// alloc buffers
		nlVec3* faceNormal = (nlVec3*)nlMalloc(sizeof(nlVec3)*indexNum/3);
		
        // calc normal
        for (unsigned int i = 0; i < indexNum; i += 3)
        {
            const nlVec3* v0 = reinterpret_cast<const nlVec3*>(&m_vertexbuf[m_indexbuf[i  ]].pos);
            const nlVec3* v1 = reinterpret_cast<const nlVec3*>(&m_vertexbuf[m_indexbuf[i+1]].pos);
            const nlVec3* v2 = reinterpret_cast<const nlVec3*>(&m_vertexbuf[m_indexbuf[i+2]].pos);
            const nlVec3 ea = *v1 - *v0;
            const nlVec3 eb = *v2 - *v0;
            nlVec3 n  = ea.cross(eb);
			n.normalize();
			faceNormal[i/3] = n;
            for (unsigned int d = 0; d < 3; ++d)
            {
                m_vertexbuf[m_indexbuf[i + d]].normal[0] += n.x_;
                m_vertexbuf[m_indexbuf[i + d]].normal[1] += n.y_;
                m_vertexbuf[m_indexbuf[i + d]].normal[2] += n.z_;
            }
        }
        for (unsigned int i = 0; i < vertexNum; ++i)
            reinterpret_cast<nlVec3*>(&m_vertexbuf[i].normal)->normalize();
        
		// realloc vertexbuffer
		NLMVertex* nvert = (NLMVertex*)nlMalloc(sizeof(NLMVertex) * indexNum);
		for (unsigned int i = 0; i < indexNum; ++i)
		{
			nvert[i] = m_vertexbuf[m_indexbuf[i]];
			m_indexbuf[i] = i;
			nlVec3* vn = (nlVec3*)(nvert[i].normal);
			const nlFloat32 dt = vn->dot(faceNormal[i/3]);
			if (dt < 1.0f/1.414213f)
				*vn = faceNormal[i/3];
		}
		nlFree(m_vertexbuf);
		m_vertexbuf = nvert;
		m_vertex_num = indexNum;

		nlFree(faceNormal);

        return true;
    }
    
    const NLMVertex* GetVertex()const  { return m_vertexbuf; }
    const unsigned int* GetIndex()const{ return m_indexbuf;  }
    unsigned int GetVertexNum()const   { return m_vertex_num; }
    unsigned int GetIndexNum()const    { return m_index_num;  }
    unsigned int GetVertexBufferSize()const { return m_vertex_num * sizeof(NLMVertex); };
    unsigned int GetIndexBufferSize()const  { return m_index_num  * sizeof(Index); };
};
/**/
void nlMeshLoad( const nlInt8* data, unsigned int dataSize, nlMesh* model, nlEngineContext* cxt )
{
    /* メッシュ展開 */
    NLMLoader loader;
    NL_VALID( loader.Load(data,dataSize) );
    const NLMVertex*    vx = loader.GetVertex();
    const unsigned int* ix = loader.GetIndex();
    const unsigned int  vn = loader.GetVertexNum();
    const unsigned int  in = loader.GetIndexNum();
    const unsigned int  vs = loader.GetVertexBufferSize();
    const unsigned int  is = loader.GetIndexBufferSize();
    /**/
    model->numIndex_ = in;
    /* 頂点バッファの生成 */
    const D3D11_BUFFER_DESC vertexBufferDesc =
    {
        vs, D3D11_USAGE_DYNAMIC,
        D3D11_BIND_VERTEX_BUFFER,
        D3D11_CPU_ACCESS_WRITE,
        0,0
    };
    D3D11_SUBRESOURCE_DATA* vertexSubResource = NULL;
    NL_HR_VALID( cxt->d3dDevice->CreateBuffer( &vertexBufferDesc, vertexSubResource, &model->vretexes_ ) );
    /* インデックスバッファの作成 */
    const D3D11_BUFFER_DESC indexBufferDesc =
    {
        is,D3D11_USAGE_DYNAMIC,
        D3D11_BIND_INDEX_BUFFER,
        D3D11_CPU_ACCESS_WRITE,
        0,0
    };
    D3D11_SUBRESOURCE_DATA* indexSubResource = NULL;
    NL_HR_VALID( cxt->d3dDevice->CreateBuffer( &indexBufferDesc, indexSubResource, &model->indices_ ) );
    /* 頂点コピー */
    D3D11_MAPPED_SUBRESOURCE mappedVertexe;
    NL_HR_VALID( cxt->d3dContext->Map( model->vretexes_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedVertexe ) );
    nlMemcpy( mappedVertexe.pData, vx, vs );
    cxt->d3dContext->Unmap( model->vretexes_, 0 );
    /* インデックスコピー */
    D3D11_MAPPED_SUBRESOURCE mappedIndex;
    NL_HR_VALID( cxt->d3dContext->Map( model->indices_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedIndex ) );
    nlMemcpy( mappedIndex.pData, ix, is );
    cxt->d3dContext->Unmap( model->indices_, 0 );
}

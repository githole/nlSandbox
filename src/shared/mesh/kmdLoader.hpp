#pragma once

#include "../nlTypedef.h"
#include "../Math/nlMath.h"
#include <d3dx9.h>

#pragma warning(disable:4127) /* 条件式が定数式 */


class KObject
{
private:
	unsigned long kobject_size;
	char* object_name;
	unsigned short vertex_num;		//4 byte
	unsigned short face_num[2];		//8 byte
	unsigned short* face;
	
	unsigned int index_num;
	nlVec3* fvertex;
	unsigned int* index;

public:
	KObject()
	{
	}
	~KObject()
	{
	}
	const nlVec3* GetVertex()
	{
		return fvertex;
	}
	
	unsigned int* GetIndex()
	{
		return index;
	}

	unsigned int GetIndexNum()
	{
		return index_num;
	}

	void countVBIB(unsigned int& vbcount, unsigned int& ibcount)
	{
		vbcount = vertex_num;
		ibcount = face_num[0] * 3 + face_num[1] * 6;
	}
	
	// KMB format only
	const nlUint8* Load(const nlUint8* dptr)
	{
		const nlUint8* st_dptr = dptr;
		//"KMB"
		dptr+=3;
		nlUint8 versioninfo = *(nlUint8*)(dptr);dptr++;
		object_name = (char*)(dptr);
		int obj_name_size = nlStrlen(object_name)+1;
		dptr += obj_name_size;
		vertex_num  = *(unsigned short*)(dptr); dptr += sizeof(unsigned short);
		face_num[0] = *(unsigned short*)(dptr); dptr += sizeof(unsigned short);
		face_num[1] = *(unsigned short*)(dptr); dptr += sizeof(unsigned short);
					
		//vertex load
		short* svertex;
		svertex = (short*)(dptr); dptr+=sizeof(short)*3*vertex_num;
		face	= (unsigned short*)(dptr); dptr+=(sizeof(unsigned short)*(face_num[0]*3+face_num[1]*4));
				
		//vertex and normal buffer alloc
		fvertex = (nlVec3*)nlMalloc(sizeof(nlVec3) * vertex_num);
		
		index_num = face_num[0] * 3 + face_num[1] * 6;
		index = (unsigned int*)nlMalloc(sizeof(unsigned int)*index_num);
		
		int vn,i;
		const int vtmax = vertex_num;
		for(i=0; i<vtmax; i++){
			fvertex[i].x_ = hftof(svertex[i*3+0]);
			fvertex[i].y_ = hftof(svertex[i*3+1]);
			fvertex[i].z_ = hftof(svertex[i*3+2]);
		}
		unsigned int pface = 0;
		unsigned int ip = 0;
		for(vn = 3; vn <= 4; vn++)
		{		
			for(i=0; i<face_num[vn-3]; i++)
			{
				if (vn == 3)
				{
					index[ip++] = face[pface  ];
					index[ip++] = face[pface+1];
					index[ip++] = face[pface+2];			
				}
				else
				{
					index[ip++] = face[pface  ];
					index[ip++] = face[pface+1];
					index[ip++] = face[pface+2];
					index[ip++] = face[pface  ];
					index[ip++] = face[pface+2];
					index[ip++] = face[pface+3];
				}
				pface += vn;
			}
		}
		
		//size
		kobject_size = (unsigned long)(dptr - st_dptr);
		return dptr;
	}
};

class KClone;
class KCloneData{
	public:
		nlUint8 primitive_id;
		nlUint8 material_id;
		nlVec3 pos;
		nlVec3 rot;
		nlVec3 scale;
		nlFloat32 alpha;
		char* clone_name;
		KClone* copyclone;//クローン・・・
		nlUint8 clonemode;//0でノーマル 1で単純クローン
		KCloneData()//とりあえずコンストラクタ
		{
			alpha = 1.0f;	
			scale = nlVec3(1.0f,1.0f,1.0f);
		}
};

class KClone{							//	  this---->child--->
	public:							    //		|        |
		KClone* sibling;				//	 sibling
		KClone* child;					//		|
		KCloneData clone_data;
		int GetTreeNum(){return 0;};
	
};

class KMDObject
{
private:
	const static int KMD_PRIMITIVE_NUM = 256;
	
	KObject* pobject[KMD_PRIMITIVE_NUM];
	//nlUint8* pobject_data[KMD_PRIMITIVE_NUM];
	KClone* tree;
	KClone* clone_alloc;
	unsigned int clone_alloc_num;

	// maked VB/IB
	nlVec3* m_vertex;
	nlUint8* m_id;
	unsigned int* m_index;
	unsigned m_vertex_num;
	unsigned m_index_num;
private:
	nlFloat32 ToRadian(nlFloat32 degree)
	{
		return degree * 3.1415926f / 180.0f;
	}
	void countVBIB(KClone* node, unsigned int& vbcount, unsigned int& ibcount, bool clonemode = false)
	{
		if (!node)
			return;

		unsigned int pid;
		if (node->clone_data.clonemode)
		{
			countVBIB(node->clone_data.copyclone, vbcount, ibcount, true);
		}
		else
		{
			pid = node->clone_data.primitive_id;
			unsigned int vb, ib;
			pobject[pid]->countVBIB(vb,ib);
			vbcount += vb;
			ibcount += ib;
			countVBIB(node->child, vbcount, ibcount);
		}
		if (!clonemode)
			countVBIB(node->sibling, vbcount, ibcount);
	}

	void makeVBIB(KClone* node, nlVec3* vertex, nlUint8* id, unsigned int* index, unsigned int& vbcnt, unsigned int& ibcnt, const D3DXMATRIX& mat, bool clonemode = false)
	{
		if (!node)
			return;

	
		nlMatrix trans, rot,rotx,rotyz, scale, world;
		D3DXMatrixTranslation(&trans, node->clone_data.pos.x_, node->clone_data.pos.y_, node->clone_data.pos.z_);
		//D3DXMatrixRotationYawPitchRoll(&rot, ToRadian(node->clone_data.rot.y_), ToRadian(node->clone_data.rot.x_), ToRadian(node->clone_data.rot.z_));
		D3DXMatrixRotationYawPitchRoll(&rotx, 0, ToRadian(node->clone_data.rot.x_), 0);
		D3DXMatrixRotationYawPitchRoll(&rotyz, ToRadian(node->clone_data.rot.y_), 0, ToRadian(node->clone_data.rot.z_));
		rot = rotyz * rotx;
		D3DXMatrixScaling(&scale, node->clone_data.scale.x_, node->clone_data.scale.y_, node->clone_data.scale.z_);
		
		unsigned int pid;
		if (node->clone_data.clonemode)
		{	
			world = trans * mat;
			world = rot   * world;
			world = scale * world;
			makeVBIB(node->clone_data.copyclone, vertex, id, index, vbcnt, ibcnt, world, true);
		}
		else
		{
			nlMatrix mworld;
#if 0 
			world =  trans * mat;
			mworld = rot   * world;
			mworld = scale * mworld;
#else
			if (clonemode)
				world =  mat;
			else
				world =  trans * mat;

			mworld = scale * rot * world;
#endif
		
			pid = node->clone_data.primitive_id;
			unsigned int vb, ib;
			pobject[pid]->countVBIB(vb,ib);
			for (unsigned int v = 0; v < vb; ++v)
			{
				const nlVec3& v3 = pobject[pid]->GetVertex()[v];
				D3DXVECTOR4 v4 = D3DXVECTOR4(v3.x_, v3.y_, v3.z_, 1.0f);
				D3DXVec4Transform(&v4, &v4, &mworld); // v4 = mat * v4
				vertex[vbcnt + v].x_ = v4.x;
				vertex[vbcnt + v].y_ = v4.y;
				vertex[vbcnt + v].z_ = v4.z;
				id[vbcnt + v] = node->clone_data.material_id;
			}
			for (unsigned int i = 0; i< ib; ++i)
				index[ibcnt + i] = vbcnt + pobject[pid]->GetIndex()[i];
			
			vbcnt += vb;
			ibcnt += ib;
			makeVBIB(node->child, vertex, id, index, vbcnt, ibcnt, world);
		}
		if (!clonemode)
			makeVBIB(node->sibling, vertex, id, index, vbcnt, ibcnt, mat);	
	}

public:
	KMDObject()
	{
	}

	~KMDObject()
	{
	}

	const nlVec3* GetVertex()
	{
		return m_vertex;
	}
	const nlUint8* GetID()
	{
		return m_id;
	}
	unsigned int GetVertexNum()
	{
		return m_vertex_num;
	}

	unsigned int GetIndexNum()
	{
		return m_index_num;
	}

	unsigned int* GetIndex()
	{
		return m_index;
	}

	void MakeVBIB()
	{
		unsigned int vbcount = 0;
		unsigned int ibcount = 0;
		countVBIB(tree, vbcount, ibcount);

		m_vertex_num = vbcount;
		m_index_num = ibcount;
		m_vertex = (nlVec3*)nlMalloc(sizeof(nlVec3)*vbcount);
		m_id     = (nlUint8*)nlMalloc(sizeof(nlUint8)*vbcount);
		m_index  = (unsigned int*)nlMalloc(sizeof(unsigned int)*ibcount);
		unsigned int vbcnt = 0, ibcnt = 0;
		D3DXMATRIX world;
		D3DXMatrixIdentity(&world);
		makeVBIB(tree, m_vertex, m_id, m_index, vbcnt, ibcnt, world);
	}

	int GetCloneAllocNum()
	{	
		if(clone_alloc!=NULL)
			return clone_alloc_num;//((int)GlobalSize(clone_alloc)/sizeof(KClone));
		else
			return 0;
	}

	KClone* GetCloneCopy(char* clnname)
	{
		int ci=0;
		int cnmax = GetCloneAllocNum();
		while(1)
		{
			if(ci == cnmax)
				break;
			if((clone_alloc[ci].clone_data.clonemode!=0)
			|| (lstrcmpA(clone_alloc[ci].clone_data.clone_name,clnname)!=0))
				ci++;
			else
				break;
		}
		if(ci==cnmax)
			return 0;
		else
			return &clone_alloc[ci];
	}

	bool Load(const nlUint8* buf)
	{
		//'K''M''D'
		const nlUint8* doffset = (&buf[3]);
		nlUint8 versioninfo = *(nlUint8*)(doffset++);
		unsigned int material_num  = *(nlUint8*)(doffset++);

		if (material_num != 0)
			return false;
		
		//primitive
		int primitive_num = *(nlUint8*)(doffset++);
		int i;
		for(i = 0; i < KMD_PRIMITIVE_NUM; i++)
		{
			//pobject_data[i] = NULL;
			pobject[i] = NULL;
		}
		for(i = 0; i < primitive_num; i++)
		{
			long object_number = *(nlUint8*)(doffset++);
			pobject[object_number] = (KObject*)nlMalloc(sizeof(KObject));
			//pobject_data[object_number] = (nlUint8*)doffset;
			doffset = pobject[object_number]->Load(doffset);
		}
		
		//tree
		unsigned short tree_num  = *(unsigned short*)(doffset); 
		doffset+=2;
		KClone* temp_tree;
		if(tree_num!=0) temp_tree = (KClone*)nlMalloc(sizeof(KClone)*tree_num);
		else			temp_tree = NULL;
		clone_alloc = temp_tree;
		clone_alloc_num = tree_num;

		nlUint8 bef_node=0;
		unsigned int clonenodesize = 256 * sizeof(KClone*);
		KClone** node_ptr = (KClone**)nlMalloc(clonenodesize);
		for(i=0; i<tree_num; i++){
			nlUint8 node = *(nlUint8*)(doffset++);
			if(bef_node< node){
				if(node_ptr[node]!=NULL) node_ptr[bef_node]->child = &temp_tree[i];
				if(bef_node>=0) node_ptr[bef_node]->child = &temp_tree[i];
				node_ptr[node] = &temp_tree[i];
			}else{
				if(node_ptr[node]!=NULL) node_ptr[node]->sibling = &temp_tree[i];
				node_ptr[node] = &temp_tree[i];
			}
			bef_node = node;
		}
		
		//data
		for(i=0; i<tree_num; i++){
			temp_tree[i].clone_data.primitive_id = *(nlUint8*)(doffset);
			doffset++;
		}
		for(i=0; i<tree_num; i++){
			temp_tree[i].clone_data.material_id = *(nlUint8*)(doffset);
			doffset++;
		}
		for(i=0; i<tree_num; i++){
			temp_tree[i].clone_data.pos.x_ = hftof(*(short*)(doffset));
			doffset+=2;
		}
		for(i=0; i<tree_num; i++){
			temp_tree[i].clone_data.pos.y_ = hftof(*(short*)(doffset));
			doffset+=2;
		}
		for(i=0; i<tree_num; i++){
			temp_tree[i].clone_data.pos.z_ = hftof(*(short*)(doffset));
			doffset+=2;
		}
		for(i=0; i<tree_num; i++){
			temp_tree[i].clone_data.rot.x_ = ctod (*(char*)(doffset));
			doffset++;
		}
		for(i=0; i<tree_num; i++){
			temp_tree[i].clone_data.rot.y_ = ctod (*(char*)(doffset));
			doffset++;
		}
		for(i=0; i<tree_num; i++){
			temp_tree[i].clone_data.rot.z_ = ctod (*(char*)(doffset)); 
			doffset++;
		}
		for(i=0; i<tree_num; i++){
			temp_tree[i].clone_data.scale.x_ = hftof(*(short*)(doffset));
			doffset+=2;
		}
		for(i=0; i<tree_num; i++){
			temp_tree[i].clone_data.scale.y_ = hftof(*(short*)(doffset));
			doffset+=2;
		}
		for(i=0; i<tree_num; i++){
			temp_tree[i].clone_data.scale.z_ = hftof(*(short*)(doffset)); 
			doffset+=2;
		}
		for(i=0; i<tree_num; i++){
			temp_tree[i].clone_data.clone_name = (char*)(doffset);
			doffset += nlStrlen(temp_tree[i].clone_data.clone_name)+1;
		}
		for(i=0; i<tree_num; i++){
			temp_tree[i].clone_data.clonemode = *(nlUint8*)(doffset);
			doffset++;
		}
		//set
		tree = temp_tree;
		nlFree(node_ptr);

		//CopyCloneSet
		for(i=0; i<tree_num; i++){
			if(temp_tree[i].clone_data.clonemode!=0){
				KClone* kcl = GetCloneCopy(temp_tree[i].clone_data.clone_name);
				temp_tree[i].clone_data.copyclone = kcl;
			}else{
				temp_tree[i].clone_data.copyclone=NULL;
			}
		}

		MakeVBIB();

		return true;
	}

};

#pragma once

#include <windows.h>
#include <windowsx.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx9.h>

#ifndef INTROMODE
#include <DxErr.h>
#pragma comment(lib,"DxErr.lib")
#endif

#pragma warning(disable:4201) /* �����̍\���� */
#pragma warning(disable:4996) /* ��������Ȃ��֐� */
#pragma warning(disable:4244) /* ���l�̕ϊ� */
#pragma warning(disable:4100) /* �Q�Ƃ���Ȃ����� */
#pragma warning(disable:4305) /* ���l�̐؂�l�� */
#pragma warning(disable:4189) /* �Q�Ƃ���Ȃ����[�J���ϐ� */
#pragma warning(disable:4389) /**/
#pragma warning(disable:4101) /**/
#pragma warning(disable:4800) /**/
#pragma warning(disable:4245) /**/
#pragma warning(disable:4996) /* ��������Ȃ��֐� */
#pragma warning(disable:4100) /* ���������p����Ȃ����� */
#pragma warning(disable:4201) /**/
#pragma warning(disable:4018) /**/
#pragma warning(disable:4725) /**/
#pragma warning(disable:4702) /* ���䂪�n��Ȃ��R�[�h */

#ifdef __cplusplus
extern "C" {
#endif
/* �v���~�e�B�u�^ */
typedef        char        nlInt8;
typedef        short       nlInt16;
typedef        int         nlInt32;
typedef        long long   nlInt64;
typedef unsigned char      nlUint8;
typedef unsigned short     nlUint16;
typedef unsigned int       nlUint32;
typedef unsigned long long nlUint64;
typedef float              nlFloat32;
typedef double             nlFloat64;
#ifdef __cplusplus
}
#endif


/* HACK ���̂������� */
#define MAX_MESH_NUM  512
#define MAX_PATH_NUM 128
#define MAX_PATHPOINT_NUM 256
#define MAX_SHADER_NUM 256
//#define MAX_TEXTURE_NUM MAX_SHADER_NUM /* @NOTE �V�F�[�_������ƃe�N�X�`��������͓����łȂ��Ƃ����Ȃ� */
#define MAX_RENDERTARGET_NUM 32
#define MAX_SCENENODE_NUM 256
#define MAX_SCENE_NUM 32

#define NL_FLT_MIN 1.175494351e-38F  /* min positive value *//* TODO minifi16bit */
#define NL_FLT_MAX 3.402823466e+38F  /* max value *//* TODO minifi16bit */
#define NL_INT_MAX 2147483647
#define NL_PI      3.141592654f      /* TODO minifi16bit */
#define NL_2PI     6.283185308f      /* TODO minifi16bit */
#define PI 3.14159265 /* @TODO ���̂������� */

/**
 * @brief �V�[���^�C���̍ŏ��l
 * 0�b�ɐݒ肷��ƃG�f�B�^�ォ�猩���Ȃ�����1�b�Ƃ���
 */
#define MIN_SCENE_TIME (1000)

#ifdef INTROMODE
#if 1 /* �G���[�_�C�A���O�Ȃ����[�h */
#define NL_ASSERT(expr)
#define NL_VALID(expr) (expr)
#define NL_HR_ASSSERT(expr)
#define NL_HR_VALID(expr) expr
#else /* �G���[�_�C�A���O���胂�[�h */
#define NL_ASSERT(expr)     if(!(expr)){MessageBox(NULL,#expr,"ASSERT!(1)",MB_OK);}
#define NL_VALID(expr)      if(!(expr)){MessageBox(NULL,#expr,"VALID!(1)",MB_OK);}
#define NL_HR_ASSSERT(expr) if(FAILED(expr)){MessageBox(NULL,#expr,"ASSSERT!(2)",MB_OK);}
#define NL_HR_VALID(expr)   if(FAILED(expr)){MessageBox(NULL,#expr,"VALID!(2)",MB_OK);}
#endif
#else
#define NL_ASSERT(expr)  \
if(!(expr))\
{\
    QString __buffer;\
    __buffer.sprintf("%s[%s:%u]\n", #expr,__FILE__,__LINE__ );\
    QByteArray __bufferBA = __buffer.toLocal8Bit();\
    MessageBoxA(NULL,__bufferBA.data(),"ASSERTION FAILED!!",MB_OK);\
    DebugBreak();\
}
#define NL_VALID(expr) NL_ASSERT((expr))
#define NL_HR_ASSSERT(expr) NL_ASSERT(SUCCEEDED((expr)))
#define NL_HR_VALID(expr) NL_HR_ASSSERT((expr))
#endif
#define NL_SAFE_RELEASE(ptr) if(ptr){ptr->Release();ptr=NULL;}

/**/
#define WINDOW_TITLE  "nlSandbox  ver 2.0 (Build " __DATE__ ")"

/**
* @brief ���f��
*/
struct nlMesh
{
    ID3D11Buffer* vretexes_;
    ID3D11Buffer* indices_;
    nlUint32      numIndex_;
};
/**
* @brief ���_�V�F�[�_
*/
struct nlVertexShader
{
    ID3D11VertexShader*	shader_;
    ID3D11InputLayout*  inputLayout_;
};
/**
* @brief �W�I���g���V�F�[�_
*/
struct nlGeometoryShader
{
    ID3D11GeometryShader* shader;
};
/**
* * @brief �s�N�Z���V�F�[�_
*/
struct nlPixelShader
{
    ID3D11PixelShader* shader_;
};
/**
* @brief �e�N�X�`�� 
*/
struct nlTexture
{
	ID3D11Texture2D* texture;
    ID3D11RenderTargetView*   rtView;
    ID3D11ShaderResourceView* shaderView;
    /**/
    nlUint32 height;
    nlUint32 width;
};

/**
* @brief �����_�[�^�[�Q�b�g
*/
struct nlRenderTarget
{
    ID3D11Texture2D*          renderTarget_;
    ID3D11RenderTargetView*   renderTargetView_;
    ID3D11ShaderResourceView* renderTargetShaderResource_;
    nlUint32 width_;
    nlUint32 height_;
};
/**
 * @brief �f�v�X�X�e���V��
 */
struct nlDepthStencil
{
    ID3D11Texture2D* tex;
    ID3D11DepthStencilView* view;
    nlUint32 width;
    nlUint32 height;
};

struct ShaderScript
{
    nlInt8* name;
    nlInt8* script;
};

struct ShaderScriptGroup
{
    ShaderScript scripts[MAX_SHADER_NUM];
    nlUint32 numScript;
};
/**/
struct ShaderScripts
{
    ShaderScript commonScripts;
    ShaderScriptGroup vsScripts;
    ShaderScriptGroup psScripts;
    ShaderScriptGroup gsScripts;
    ShaderScriptGroup tsScripts;
};
/**/
enum ShaderType
{
    ShaderType_VS,
    ShaderType_GS,
    ShaderType_PS,
    ShaderType_TS,
    ShaderType_CS,
    ShaderType_NONE,
};

/**
 * @brief �T�E���h
 */
struct nlSound
{
    /* ��ɂ��Ă��� */
};

/* �X�N���[���G�t�F�N�g�pVS */
static const nlInt8 screenVSScript[] = 
{
    "struct V"
    "{"
    "float4 p:SV_POSITION;"
    "float2 u:TEXCOORD0;"
    "};"
    "V vs(uint i:SV_VertexID)"
    "{"
    "V r;"
    "r.p=float4((((int)i%2)*2-1),((int)i/2*2-1),0,1);"
    "r.u=float2(i%2,1-i/2);"
    "return r;"
    "}"
};

/**
 * @brief ���b�V���p���_�`��
 */
struct NLMVertex
{
    nlFloat32 pos[4];
    nlUint32 color;
    nlFloat32 normal[4];
    nlUint32 materialID;
};
/* InputElementDescription */
static D3D11_INPUT_ELEMENT_DESC NLM_INPUT_ELEMENT[] = 
{
    { "P", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "C", 0, DXGI_FORMAT_R8G8B8A8_UNORM,     0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "N", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "M", 0, DXGI_FORMAT_R32_UINT,           0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

/**
 * @brief
 */
struct RenderTargetDesc
{
    /* �o�b�N�o�b�t�@�T�C�Y�ɑ΂���傫���B0:x1 1:x1/2 2:x1/4 3:x1/8....���Ӗ����܂� */
    nlUint32 scaleByBackbuffer; 
    /* �t�H�[�}�b�g */
    DXGI_FORMAT format;
};
/**
 * @brief
 */
struct RenderTargetDescs
{
    /**/
    RenderTargetDesc rtDescs[MAX_RENDERTARGET_NUM];
    nlUint32 numRT;
    /**/
    RenderTargetDesc dsDescs[MAX_RENDERTARGET_NUM];
    nlUint32 numDS;
};


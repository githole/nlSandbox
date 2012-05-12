#ifndef _NLRENDER_H_
#define _NLRENDER_H_

#include "shared/nlTypedef.h"
#include "shared/scene/nlScene.hpp"
#include "shared\/container/nlArray.hpp"

/**/
#define MAX_CONST_BUFFER 16
extern nlArray<ID3D11Buffer*,MAX_CONST_BUFFER>cbVsMatrix;
extern nlArray<ID3D11Buffer*,MAX_CONST_BUFFER>cbGsMatrix;
extern nlArray<ID3D11Buffer*,MAX_CONST_BUFFER>cbPsMatrix;
/**/
typedef void(*DebugRenderFunc)(struct nlEngineContext*, void* userData);
/**/
struct nlRenderCommand
{
    /**/
    enum Type
    {
        /* RenderTarget���N���A */
        RC_CLEAR_RT,
        /* DepthStencil���N���A */
        RC_CLEAR_DS,
        /**/
        RC_Z_STATE,
        /**/
        RC_ALPHA_STATE,
        /* RenderTarget��ݒ� */
        RC_SET_RENDERTARGET,
        /* ConstantBuffer��ݒ� */
        RC_SET_CB,
        /* �e�N�X�`����ݒ� */
        RC_SET_TS,
        /**/
        RC_SET_TS_RT,
        /* �X�N���[���|���S����`�� */
        RC_RENDER_SCREEN_POLY2,
        RC_RENDER_SCREEN_POLY3,
        /* ���b�V���O���[�v��`�� */
        RC_RENDER_DRAW_GROUP,
        /* �A���t�@�X�e�[�g��ݒ� */
        //RS_SET_ALPHA_STATE,
        /* �f�o�b�O�p�̃����_�[��ǉ� */
        RS_DEBUG_RENDER,
    }type_;
    /**/
    union Prop
    {
        int valI;
        nlFloat32 valF;
    };
    union
    {
        Prop props[8];
    };
};
/**
 * @brief 
 */
struct ObjectBlob
{
    int groupId;
    int meshId;
    int numObject;
    nlNodeValveArray* constVS0;
    nlNodeValveArray* constVS1;
    nlNodeValveArray* constPS0;
    nlNodeValveArray* constPS1;
};
/**
 * @brief Z�X�e�[�g
 */
enum ZState
{
    ZS_ReadWrite, /* �ǂݏ��� */
    ZS_OnlyRead,  /* �ǂ݂̂�(�������܂Ȃ�) */
    ZS_NotUse,    /* �ǂݏ������Ȃ� */
};
/**
 * @brief �A���t�@�X�e�[�g
 */
enum AlphaState
{
    AS_Disable,
    AS_Add,
    AS_Alpha,
};
/**
* @brief �`��R�}���h
*/
struct nlRenderCommands
{
public:
    nlRenderCommands()
    {
        clear();
    }
    /* �`��R�}���h���N���A���� */
    void clear()
    {
        commandIndex_ = 0;
        objBlobIndex_ = 0;
    }
    /* �R�}���h���t���ɂ��� */
    void reverse()
    {
        for( unsigned int i=0;i<commandIndex_/2;++i)
        {
            nlRenderCommand tmp = renderData[i];
            renderData[i] = renderData[commandIndex_-i-1];
            renderData[commandIndex_-i-1] = tmp;
        }
    }
    /**/
    void clearRT( 
        int index,
        int red,
        int green,
        int blue )
    {
        renderData[commandIndex_].type_ = nlRenderCommand::RC_CLEAR_RT;
        renderData[commandIndex_].props[0].valI = index;
        renderData[commandIndex_].props[1].valI = red;
        renderData[commandIndex_].props[2].valI = green;
        renderData[commandIndex_].props[3].valI = blue;
        ++commandIndex_;
    }
    /**/
    void clearDS(
        int index )
    {
        renderData[commandIndex_].type_ = nlRenderCommand::RC_CLEAR_DS;
        renderData[commandIndex_].props[0].valI = index;
        ++commandIndex_;
    }
    void setZState(
        ZState zState )
    {
        renderData[commandIndex_].type_ = nlRenderCommand::RC_Z_STATE;
        renderData[commandIndex_].props[0].valI = (int)zState;
        ++commandIndex_;
    }
    /**/
    void setAlphaState(
        AlphaState alphaState )
    {
        renderData[commandIndex_].type_ = nlRenderCommand::RC_ALPHA_STATE;
        renderData[commandIndex_].props[0].valI = (int)alphaState;
        ++commandIndex_;
    }
    /**/
    void setRenderTarget(
        unsigned int rt0Index,
        unsigned int rt1Index,
        unsigned int rt2Index,
        unsigned int dsIndex )
    {
        renderData[commandIndex_].type_ = nlRenderCommand::RC_SET_RENDERTARGET;
        renderData[commandIndex_].props[0].valI = rt0Index;
        renderData[commandIndex_].props[1].valI = rt1Index;
        renderData[commandIndex_].props[2].valI = rt2Index;
        renderData[commandIndex_].props[3].valI = dsIndex;
        ++commandIndex_;
    }
    /**/
    void setConstantBuffer1(
        ShaderType shaderType,
        int startIndex,
        int numConst,
        const nlNodeValveArray* c0,
        const nlNodeValveArray* c1,
        const nlNodeValveArray* c2,
        const nlNodeValveArray* c3 )
    {
        /**/
        startIndex = MN(startIndex,12);
        numConst   = MN(numConst,4);
        /**/
        renderData[commandIndex_].type_ = nlRenderCommand::RC_SET_CB;
        renderData[commandIndex_].props[0].valI = shaderType;
        renderData[commandIndex_].props[1].valI = startIndex;
        renderData[commandIndex_].props[2].valI = numConst;
        renderData[commandIndex_].props[3].valI = reinterpret_cast<int>( c0 );
        renderData[commandIndex_].props[4].valI = reinterpret_cast<int>( c1 );
        renderData[commandIndex_].props[5].valI = reinterpret_cast<int>( c2 );
        renderData[commandIndex_].props[6].valI = reinterpret_cast<int>( c3 );
        ++commandIndex_;
    }
    /**/
    void setTex(
        unsigned int texIndex,
        unsigned int index )
    {
        renderData[commandIndex_].type_ = nlRenderCommand::RC_SET_TS;
        renderData[commandIndex_].props[0].valI = texIndex;
        renderData[commandIndex_].props[1].valI = index;
        ++commandIndex_;
    }
    /**/
    void setTexRt(
        unsigned int rtIndex,
        unsigned int index )
    {
        renderData[commandIndex_].type_ = nlRenderCommand::RC_SET_TS_RT;
        renderData[commandIndex_].props[0].valI = rtIndex;
        renderData[commandIndex_].props[1].valI = index;
        ++commandIndex_;
    }
    /**/
    void renderScreenPoly2( unsigned int psId )
    {
        nlRenderCommand& cmd = renderData[commandIndex_];
        cmd.type_ = nlRenderCommand::RC_RENDER_SCREEN_POLY2;
        renderData[commandIndex_].props[0].valI = psId;
        ++commandIndex_;
    }
    /**/
    void renderScreenPoly3( unsigned int psId, unsigned int vsId )
    {
        nlRenderCommand& cmd = renderData[commandIndex_];
        cmd.type_ = nlRenderCommand::RC_RENDER_SCREEN_POLY3;
        renderData[commandIndex_].props[0].valI = psId;
        renderData[commandIndex_].props[1].valI = vsId;
        ++commandIndex_;
    }
    /**/
    void drawGroup(
        int groupId,
        unsigned int vsIndex,
        unsigned int gsIndex,
        unsigned int psIndex,
        unsigned int tpIndex )
    {
        nlRenderCommand& cmd = renderData[commandIndex_];
        cmd.type_ = nlRenderCommand::RC_RENDER_DRAW_GROUP;
        cmd.props[0].valI = groupId;
        cmd.props[1].valI = vsIndex;
        cmd.props[2].valI = gsIndex;
        cmd.props[3].valI = psIndex;
        cmd.props[4].valI = tpIndex;
        ++commandIndex_;
    }
    /* �I�u�W�F�N�g��o�^���� */
    void addObject( 
        int groupId,
        int meshId,
        int numObject,
        nlNodeValveArray* constVS0,
        nlNodeValveArray* constVS1,
        nlNodeValveArray* constPS0,
        nlNodeValveArray* constPS1 )
    {
        /**/
        NL_ASSERT( constVS0 );
        NL_ASSERT( constVS1 );
        NL_ASSERT( constPS0 );
        NL_ASSERT( constPS1 );
        /**/
        ObjectBlob& obj = objectBlobs[objBlobIndex_];
        obj.groupId = groupId;
        obj.meshId  = meshId;
        obj.numObject = numObject;
        obj.constVS0 = constVS0;
        obj.constVS1 = constVS1;
        obj.constPS0 = constPS0;
        obj.constPS1 = constPS1;
        ++objBlobIndex_;
    }
    /* nlRender�ɂ͂Ȃ��`������̃R�}���h��}�������^�C�~���O�ōs���B��Ƀf�o�b�O�p�r */
    void renderDebug( DebugRenderFunc renderFunc, void* userData )
    {
        nlRenderCommand& cmd = renderData[commandIndex_];
        cmd.type_ = nlRenderCommand::RS_DEBUG_RENDER;
        cmd.props[0].valI = (int)renderFunc;
        cmd.props[1].valI = (int)userData;
        ++commandIndex_;
    }
    /* �w�肵���R�}���h���폜����B�f�o�b�O�p�r�B */
    void eraseByType( nlRenderCommand::Type type )
    {
        int diff = 0;
        for( unsigned int i=0;i<commandIndex_;++i)
        {
            if( renderData[i].type_ == type )
            {
                ++diff; 
                --commandIndex_;
                if( commandIndex_ <= i )
                {
                    break;
                }
            }
            /**/
            renderData[i].type_ = renderData[i+diff].type_;
        }
    }
public:
    nlArray<nlRenderCommand,128>renderData;
    int commandIndex_;
    nlArray<ObjectBlob,128>objectBlobs;
    int objBlobIndex_;
};

/**
* @brief �����_�[��������
*/
void nlRenderInit( struct nlEngineContext* cxt );
/**
* @brief �`��
*/
void nlRender( const nlRenderCommands& renderCommands, struct nlEngineContext* cxt );

#endif /* #ifndef _NLRENDER_H_ */

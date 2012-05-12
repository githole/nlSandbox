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
        /* RenderTargetをクリア */
        RC_CLEAR_RT,
        /* DepthStencilをクリア */
        RC_CLEAR_DS,
        /**/
        RC_Z_STATE,
        /**/
        RC_ALPHA_STATE,
        /* RenderTargetを設定 */
        RC_SET_RENDERTARGET,
        /* ConstantBufferを設定 */
        RC_SET_CB,
        /* テクスチャを設定 */
        RC_SET_TS,
        /**/
        RC_SET_TS_RT,
        /* スクリーンポリゴンを描画 */
        RC_RENDER_SCREEN_POLY2,
        RC_RENDER_SCREEN_POLY3,
        /* メッシュグループを描画 */
        RC_RENDER_DRAW_GROUP,
        /* アルファステートを設定 */
        //RS_SET_ALPHA_STATE,
        /* デバッグ用のレンダーを追加 */
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
 * @brief Zステート
 */
enum ZState
{
    ZS_ReadWrite, /* 読み書き */
    ZS_OnlyRead,  /* 読みのみ(書き込まない) */
    ZS_NotUse,    /* 読み書きしない */
};
/**
 * @brief アルファステート
 */
enum AlphaState
{
    AS_Disable,
    AS_Add,
    AS_Alpha,
};
/**
* @brief 描画コマンド
*/
struct nlRenderCommands
{
public:
    nlRenderCommands()
    {
        clear();
    }
    /* 描画コマンドをクリアする */
    void clear()
    {
        commandIndex_ = 0;
        objBlobIndex_ = 0;
    }
    /* コマンドを逆順にする */
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
    /* オブジェクトを登録する */
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
    /* nlRenderにはない描画をこのコマンドを挿入したタイミングで行う。主にデバッグ用途 */
    void renderDebug( DebugRenderFunc renderFunc, void* userData )
    {
        nlRenderCommand& cmd = renderData[commandIndex_];
        cmd.type_ = nlRenderCommand::RS_DEBUG_RENDER;
        cmd.props[0].valI = (int)renderFunc;
        cmd.props[1].valI = (int)userData;
        ++commandIndex_;
    }
    /* 指定したコマンドを削除する。デバッグ用途。 */
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
* @brief レンダーを初期化
*/
void nlRenderInit( struct nlEngineContext* cxt );
/**
* @brief 描画
*/
void nlRender( const nlRenderCommands& renderCommands, struct nlEngineContext* cxt );

#endif /* #ifndef _NLRENDER_H_ */

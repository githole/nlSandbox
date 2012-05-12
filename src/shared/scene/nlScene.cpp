#include "main/stdafx.h"
#include "shared/scene/nlScene.hpp"
#include "shared/nlMemory.h"
#include "shared/math/nlMath.h"
#include "shared/render/nlRender.h"
#include "shared/container/nlVector.hpp"
#include "shared/container/nlArray.hpp"
#include "shared/curve/nlCurve.hpp"

/**/
static void processThatNode( nlScene& scene, int targetNodeIdx, unsigned int nowInMs, nlRenderCommands& rc, nlEngineContext& cxt );
static const nlNodeValveArray& getPrevNodeOutput( const ProcessBuffer& allProcessBuffer, const nlSceneNode& node, unsigned int prevIdx );

static void constructRenderpipe( nlRenderCommands& rc, const nlArray<nlSceneNode,MAX_SCENENODE_NUM>& sceneMap, int numSceneMap, ProcessBuffer& processBuffer );

/**/
static SceneArray* allScenes;
static ProcessBuffer* g_processBuffer;
nlNodeValveArray* nullValveArray;

static const D3DMATRIX matIdentity2 = 
{
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f,
};
const D3DMATRIX& hoge()
{
    return matIdentity2;
}

/*----------------------------------------------------------------------------------------------------*/
// nlNodeValveArray
/*----------------------------------------------------------------------------------------------------*/
nlNodeValveArray::nlNodeValveArray(
                                   unsigned int lenAsVec4 )
                                   :lenAsVec4_(lenAsVec4)
{
    array_ = (nlVec4*)nlMalloc(lenAsVec4_*sizeof(nlVec4));
}
/**/
nlNodeValveArray::nlNodeValveArray()
:lenAsVec4_(0),
array_(0)
{

}
/**/
nlNodeValveArray::nlNodeValveArray( 
                                   const nlNodeValveArray& other )
                                   :array_(NULL),
                                   lenAsVec4_(0)
{
    lenAsVec4_ = other.lenAsVec4_;
    array_ = (nlVec4*)nlMalloc(lenAsVec4_*sizeof(nlVec4));

    for( unsigned int i=0;i<lenAsVec4_;++i)
    {
        array_[i] = other.array_[i];
    }
}
/**/
nlNodeValveArray::~nlNodeValveArray()
{
    nlFree(array_);
}
/**/
nlVec4 nlNodeValveArray::atAsVec4( unsigned int idx )const
{
    /* in case "out of range", return "last value" */
    if( lenAsVec4_ == 0 )
    {
        return nlVec4::create(0.0f,0.0f,0.0f,0.0f);
    }
    else if( lenAsVec4_ <= idx )
    { 
        return array_[lenAsVec4_-1]; 
    }

    return array_[idx];
}

/**/
const nlMatrix nlNodeValveArray::atAsMatrix( unsigned int idx )const
{
    nlMatrix* matArray = (nlMatrix*)array_;
    /* in case "out of range", return "last value" */
    if( lenAsVec4_ < 4 )
    {
        return matIdentity2;
    }
    else if( lenAsVec4_/4 <= idx )
    { 
        return matArray[lenAsVec4_/4-1]; 
    }
    return matArray[idx];
}
/**/
const nlMatrix* nlNodeValveArray::atAsMatrix2( unsigned int idx )const
{
    nlMatrix* matArray = (nlMatrix*)array_;
    /* in case "out of range", return "last value" */
    if( lenAsVec4_ < 4 )
    {
        return NULL;
    }
    else if( lenAsVec4_/4 <= idx )
    { 
        return &matArray[lenAsVec4_/4-1]; 
    }
    return &matArray[idx];
}
/**/
void nlNodeValveArray::writeAsVec4( unsigned int idx, const nlVec4& value )
{
    /* in case "out of range", do noting. */
    if( lenAsVec4_ <= idx )
    { return; }

    array_[idx] = value;
}
/**/
void nlNodeValveArray::writeAsMatrix( unsigned int idx, const nlMatrix& value )
{
    /* in case "out of range", do noting. */
    if( lenAsVec4_/4 <= idx )
    { return; }

    nlMatrix* matArray = (nlMatrix*)array_;
    matArray[idx] = value;
}
/**/
unsigned int nlNodeValveArray::sizeAsVec4()const
{
    return lenAsVec4_;
}
/**/
unsigned int nlNodeValveArray::sizeAsMatrix()const
{
    return lenAsVec4_/4;
}
/**/
void nlNodeValveArray::resizeAsVec4( unsigned int newLen )
{
    /* 現在より小さい場合はメモリは触らない */
    if( newLen <= lenAsVec4_ )
    {
        lenAsVec4_ = newLen;
    }
    /* 現在より大きい場合は、メモリ確保後、現在の値を移動させる */
    else
    {
        nlVec4* oldArray = array_;
        const unsigned int oldLen = lenAsVec4_;
        array_ = (nlVec4*)nlMalloc(newLen*sizeof(nlVec4));
        for( unsigned int i=0;i<oldLen;++i)
        {
            array_[i] = oldArray[i];
        }
        nlFree( oldArray );
        lenAsVec4_ = newLen;
    }
}
/**/
void nlNodeValveArray::resizeAsMatrix( unsigned int newLen )
{
    /* 現在より小さい場合はメモリは触らない */
    if( newLen * 4 <= lenAsVec4_ )
    {
        lenAsVec4_ = newLen * 4;
    }
    /* 現在より大きい場合は、メモリ確保後、現在の値を移動させる */
    else
    {
        nlVec4* oldArray = array_;
        const unsigned int oldLen = lenAsVec4_;
        array_ = (nlVec4*)nlMalloc(newLen*4*sizeof(nlVec4));
        for( unsigned int i=0;i<oldLen;++i)
        {
            array_[i] = oldArray[i];
        }
        nlFree( oldArray );
        lenAsVec4_ = newLen * 4;
    }
}
/**/
void nlNodeValveArray::operator=(const nlNodeValveArray& other)
{
    if( this == &other )
    {
        return ;
    }
    lenAsVec4_ = other.lenAsVec4_;
    if( array_ ){ nlFree( array_ ); }
    array_ = (nlVec4*)nlMalloc(lenAsVec4_*sizeof(nlVec4));

    for( unsigned int i=0;i<lenAsVec4_;++i)
    {
        array_[i] = other.array_[i];
    }
}

/**/
void nlScene_init()
{
    /**/
    nullValveArray = (nlNodeValveArray*)nlMalloc(sizeof(nlNodeValveArray));
    nullValveArray->nlNodeValveArray::nlNodeValveArray();
    /**/
    allScenes = (SceneArray*)nlMalloc( sizeof(SceneArray) );
    /* 全てのシーンマップ用のアロケートを行う */
    for( int i=0;i<allScenes->size();++i )
    {
        nlScene& scene = (*allScenes)[i];
        scene.sceneTimeInMs = 0;
        /**/
        nlArray<nlSceneNode,MAX_SCENENODE_NUM>& sceneNodes = scene.sceneMaps_;
        /**/
        for( int j=0;j<sceneNodes.size();++j)
        {
            sceneNodes[j].type_ = SNT_INVALID; 
        }
    }
    /* プロセスバッファの初期化 */
    g_processBuffer = (ProcessBuffer*)nlMalloc( sizeof(ProcessBuffer) );
    for( int i=0;i<g_processBuffer->size();++i)
    { (*g_processBuffer)[i].isProcessed_ = false; }
}
/**/
void nlScene_init( const SceneArray* scenes )
{
    /**/
    allScenes = const_cast<SceneArray*>(scenes);
    /**/
    nullValveArray = (nlNodeValveArray*)nlMalloc(sizeof(nlNodeValveArray));
    nullValveArray->nlNodeValveArray::nlNodeValveArray();
    /* プロセスバッファの初期化 */
    g_processBuffer = (ProcessBuffer*)nlMalloc( sizeof(ProcessBuffer) );
    for( int i=0;i<g_processBuffer->size();++i)
    { 
        (*g_processBuffer)[i].isProcessed_ = false; 
        /* 1024(プロエスバッファ数)*4(byte/nlFloat32)*16(nlFloat32/matrix)*256(matrix/node) = 16MB程度なので気にしない */
        (*g_processBuffer)[i].output_.resizeAsMatrix(64*3);
    }
}
/**/
void nlScene_Update(
                    unsigned int nowInMs,
                    nlRenderCommands* renderCommands,
                    nlEngineContext& cxt )
{
    /* シーンを更新する */
    SceneArray& scenes = *allScenes;
    const int sceneIndex = nlScene_sceneIndex(nowInMs);
    nlScene& scene = nlScene_scene(nowInMs);
    /* プロセスバッファを確保 */
    ProcessBuffer& processBuffer = *g_processBuffer;
    for( int i=0;i<scene.numSceneMap_;++i)
    {
        nlSceneNode& sceneNode = scene.sceneMaps_[i];
        /* 出力ノードをリサイズ */
        const unsigned int numValve = sceneNode.numValve_;
        switch( sceneNode.outEdgeType )
        {
        case NL_ET_Matrix:
            processBuffer[i].output_.resizeAsMatrix(numValve);
            break;
        case NL_ET_Vec4:
            processBuffer[i].output_.resizeAsVec4(numValve);
            break;
        case NL_ET_None:
        case NL_ET_Render:
            /* 特に何もしない */
            break;
        default:
            NL_ASSERT(!"存在しないノードバルブタイプ");
            break;
        }
    }
    /* シーンを更新する */
    for( int nodeIdx=0;nodeIdx<scene.sceneMaps_.size();++nodeIdx)
    {
        processThatNode( scene, nodeIdx, nowInMs, *renderCommands, cxt );
    }
    /*
     * 全ての計算処理をクリア 
     * nlScene_Update()に入る前に処理してしまったものを上書きしてしまうため
     * この処理はprocessThatNode()が終了してから呼ぶこと。
     */
    for( int i=0;i<processBuffer.size();++i)
    {
        processBuffer[i].isProcessed_ = false;
    }
    /* 描画コマンドを作成する */
    constructRenderpipe( *renderCommands, scene.sceneMaps_, scene.numSceneMap_, *g_processBuffer );
}
/**/
static void processThatNode( nlScene& scene, int targetNodeIdx, unsigned int nowInMs, nlRenderCommands& rc, nlEngineContext& cxt )
{
    const nlSceneNode& curNode       = scene.sceneMaps_[targetNodeIdx];
    ProcessBuffer& processBuffer     = (*g_processBuffer);
    ProcessInfoPerNode& curProcess   = processBuffer[targetNodeIdx];
    nlNodeValveArray& curOutput      = curProcess.output_;

    if( curNode.type_ == SNT_INVALID )
    { return; }
    /**/
    if( curProcess.isProcessed_ )
    { return; }

#ifndef INTROMODE
    /* イントロ中ではない(ツールで編集中)であれば事前にソートされていることを仮定できないため再帰的に更新する */
    for(int inputIdx=0;inputIdx<SCENENODE_INPUT_NUM;++inputIdx)
    {
        const unsigned int inputNodeIdx = curNode.prevNode_[inputIdx];
        /**/
        if( inputNodeIdx == 0xffffffff )
        {continue;}
        /**/
        if( scene.sceneMaps_[inputNodeIdx].type_ == SNT_INVALID )
        {continue;}
        /**/
        ProcessInfoPerNode& inputNode = processBuffer[inputNodeIdx];
        if( !inputNode.isProcessed_ )
        {
            processThatNode( scene, inputNodeIdx, nowInMs, rc, cxt );
        }
    }
#endif

#if 0
    /* check and process All Input nodes */
    for(int inputIdx=0;inputIdx<SCENENODE_INPUT_NUM;++inputIdx)
    {
        const unsigned int inputNodeIdx = curNode.prevNode_[inputIdx];
        /**/
        if( inputNodeIdx == 0xffffffff )
        {continue;}
        /**/
        if( scene.sceneMaps_[inputNodeIdx].type_ == SNT_INVALID )
        { continue;}
        /**/
        ProcessInfoPerNode& inputNode = scene.processBuffer[inputNodeIdx];
        if( !inputNode.isProcessed_ )
        {
            processThatNode( scene, inputNodeIdx, cxt );
        }
    }
#endif

#if 1
    /**/
    const nlNodeValveArray& input0 = getPrevNodeOutput(processBuffer,curNode,0);
    const nlNodeValveArray& input1 = getPrevNodeOutput(processBuffer,curNode,1);
    const nlNodeValveArray& input2 = getPrevNodeOutput(processBuffer,curNode,2);
    const nlNodeValveArray& input3 = getPrevNodeOutput(processBuffer,curNode,3);
    const nlNodeValveArray& input4 = getPrevNodeOutput(processBuffer,curNode,4);

#endif
    /* process this node */
    switch( curNode.type_ )
    {
    case SNT_NOP:
        {       
            for( unsigned int valveIdx=0;valveIdx<curOutput.sizeAsVec4();++valveIdx)
            {
                curProcess.output_.writeAsVec4(valveIdx,input0.atAsVec4(valveIdx));
            }
        }
        break;
    case SNT_MAT_NOP:
        {
            for( unsigned int valveIdx=0;valveIdx<curOutput.sizeAsMatrix();++valveIdx)
            {
                curProcess.output_.writeAsMatrix(valveIdx,input0.atAsMatrix(valveIdx));
            }
        }
        break;
    case SNT_ADD:
        {            
            for( unsigned int valveIdx=0;valveIdx<curOutput.sizeAsVec4();++valveIdx)
            {
                const nlVec4 i0 = input0.atAsVec4(valveIdx);
                const nlVec4 i1 = input1.atAsVec4(valveIdx);
                const nlVec4 writeValue = nlVec4::create( i0.x_+i1.x_, i0.y_+i1.y_, i0.z_+i1.z_, i0.w_+i1.w_ );
                curProcess.output_.writeAsVec4(valveIdx,writeValue);
            }
        }
        break;
    case SNT_SUB:
        {
            for( unsigned int valveIdx=0;valveIdx<curOutput.sizeAsVec4();++valveIdx)
            {
                const nlVec4 i0 = input0.atAsVec4(valveIdx);
                const nlVec4 i1 = input1.atAsVec4(valveIdx);
                const nlFloat32 x = i0.x_-i1.x_;
                const nlFloat32 y = i0.y_-i1.y_;
                const nlFloat32 z = i0.z_-i1.z_;
                const nlFloat32 w = i0.w_-i1.w_;
                const nlVec4 writeValue = nlVec4::create( x, y, z, w);
                curProcess.output_.writeAsVec4(valveIdx,writeValue);
            }
        }
        break;
    case SNT_DIV:
        {
            for( unsigned int valveIdx=0;valveIdx<curOutput.sizeAsVec4();++valveIdx)
            {
                const nlVec4 i0 = input0.atAsVec4(valveIdx);
                const nlVec4 i1 = input1.atAsVec4(valveIdx);
                /* this is nlFloat32 div. so dont worry about 0 div exception. */
                const nlFloat32 x = i0.x_/i1.x_;
                const nlFloat32 y = i0.y_/i1.y_;
                const nlFloat32 z = i0.z_/i1.z_;
                const nlFloat32 w = i0.w_/i1.w_;
                curProcess.output_.writeAsVec4(valveIdx, nlVec4::create( x, y, z, w ) );
            }
        }
        break;
    case SNT_MUL:
        {
            for( unsigned int valveIdx=0;valveIdx<curOutput.sizeAsVec4();++valveIdx)
            {
                const nlVec4 i0 = input0.atAsVec4(valveIdx);
                const nlVec4 i1 = input1.atAsVec4(valveIdx);
                const nlVec4 writeValue = nlVec4::create( i0.x_*i1.x_, i0.y_*i1.y_, i0.z_*i1.z_, i0.w_*i1.w_ );
                curProcess.output_.writeAsVec4(valveIdx,writeValue);
            }
        }
        break;
    case SNT_MINMAX:
        {
            const bool isMax = (((int)curNode.prop_[0])!=0);
            for( unsigned int valveIdx=0;valveIdx<curOutput.sizeAsVec4();++valveIdx)
            {
                const nlVec4 i0 = input0.atAsVec4(valveIdx);
                const nlVec4 i1 = input1.atAsVec4(valveIdx);
                if( isMax )
                {
                    const nlFloat32 x = MX( i0.x_, i1.x_ );
                    const nlFloat32 y = MX( i0.y_, i1.y_ );
                    const nlFloat32 z = MX( i0.z_, i1.z_ );
                    const nlFloat32 w = MX( i0.w_, i1.w_ );
                    const nlVec4 writeValue = nlVec4::create( x, y, z, w );
                    curProcess.output_.writeAsVec4(valveIdx,writeValue);
                }
                else
                {
                    const nlFloat32 x = MN( i0.x_, i1.x_ );
                    const nlFloat32 y = MN( i0.y_, i1.y_ );
                    const nlFloat32 z = MN( i0.z_, i1.z_ );
                    const nlFloat32 w = MN( i0.w_, i1.w_ );
                    const nlVec4 writeValue = nlVec4::create( x, y, z, w );
                    curProcess.output_.writeAsVec4(valveIdx,writeValue);
                }
            }
        }
        break;
    case SNT_DIF:
        {
            const unsigned int offset  = (unsigned int)(curNode.prop_[0]);
            for( unsigned int valveIdx=0;valveIdx<curOutput.sizeAsVec4();++valveIdx)
            {
                const nlVec4 c = input0.atAsVec4(valveIdx);
                const nlVec4 n = input0.atAsVec4(valveIdx+offset);
                const nlVec4 writeValue = nlVec4::create( n.x_-c.x_,n.y_-c.y_,n.z_-c.z_,n.w_-c.w_ );
                curProcess.output_.writeAsVec4(valveIdx,writeValue);
            }
        }
        break;
    case SNT_ROT2:
        {            
            /* input0:元のデータ input1:回転作用を記述したもの */
            for( unsigned int valveIdx=0;valveIdx<curOutput.sizeAsVec4();++valveIdx)
            {
                const nlVec4 src  = input0.atAsVec4(valveIdx);
                const nlVec4 ypr = input1.atAsVec4(valveIdx);
                D3DXMATRIX srcVector;
                D3DXMatrixIdentity( &srcVector );
                srcVector._11 = src.x_;
                srcVector._12 = src.y_;
                srcVector._13 = src.z_;
                D3DXMATRIX rotMatrix, result;
                D3DXMatrixRotationYawPitchRoll( &rotMatrix, ypr.x_, ypr.y_, ypr.z_ );
                D3DXMatrixMultiply( &result, &srcVector, &rotMatrix );
                const nlVec4 writeValue = nlVec4::create( result._11, result._12, result._13, 0.0f );
                curProcess.output_.writeAsVec4(valveIdx,writeValue);
            }
        }
        break;
    case SNT_TRANCE:
        {
            const nlFloat32 x = curNode.prop_[0];
            const nlFloat32 y = curNode.prop_[1];
            const nlFloat32 z = curNode.prop_[2];
            /* simply add */
            for( unsigned int valveIdx=0;valveIdx<curOutput.sizeAsVec4();++valveIdx)
            {
                const nlVec4 input = input0.atAsVec4(valveIdx);
                const nlVec4 writeValue = nlVec4::create(input.x_+x, input.y_+y, input.z_+z, input.w_ );
                curProcess.output_.writeAsVec4(valveIdx,writeValue);
            }
        }
        break;
    case SNT_TRANCE2:
        {
            const nlFloat32 x = curNode.prop_[0];
            const nlFloat32 y = curNode.prop_[1];
            const nlFloat32 z = curNode.prop_[2];
            const nlFloat32 w = curNode.prop_[3];
            /* simply add */
            for( unsigned int valveIdx=0;valveIdx<curOutput.sizeAsVec4();++valveIdx)
            {
                const nlVec4 input = input0.atAsVec4(valveIdx);
                const nlVec4 writeValue = nlVec4::create(input.x_+x, input.y_+y, input.z_+z, input.w_+w );
                curProcess.output_.writeAsVec4(valveIdx,writeValue);
            }
        }
        break;
    case SNT_SCALE:
        {
            const nlFloat32 scale  = curNode.prop_[0];
            for( unsigned int valveIdx=0;valveIdx<curOutput.sizeAsVec4();++valveIdx)
            {
                const nlVec4 i0 = input0.atAsVec4(valveIdx);
                const nlVec4 writeValue = nlVec4::create( i0.x_*scale, i0.y_*scale, i0.z_*scale, i0.w_*scale );
                curProcess.output_.writeAsVec4(valveIdx,writeValue);
            }
        }
        break;
#if 0
    case SNT_CLAMP:
        {
            const nlFloat32 lower  = curNode.prop_[0];
            const nlFloat32 upper  = curNode.prop_[1];
            for( unsigned int valveIdx=0;valveIdx<curOutput.sizeAsVec4();++valveIdx)
            {
                const nlVec4 i0 = input0.atAsVec4(valveIdx);
                const nlVec4 writeValue 
                    = nlVec4::create( 
                    MN( MX(i0.x_, lower), upper ),
                    MN( MX(i0.y_, lower), upper ),
                    MN( MX(i0.z_, lower), upper ),
                    MN( MX(i0.w_, lower), upper ) );
                curProcess.output_.writeAsVec4(valveIdx,writeValue);
            }
        }
        break;
#endif
    case SNT_SIN:
        {
            const nlFloat32 radius  = curNode.prop_[0];
            /* Xだけに入れる */            
            for( unsigned int valveIdx=0;valveIdx<curOutput.sizeAsVec4();++valveIdx)
            {
                const nlFloat32 t = input0.atAsVec4(valveIdx).x_;
                const nlFloat32 result = radius*nlSin(t);
                const nlVec4 writeValue = nlVec4::create(result,0.0f,0.0f,0.0f);
                curProcess.output_.writeAsVec4(valveIdx,writeValue);
            }
        }
        break;
    case SNT_COS:
        {
            const nlFloat32 radius  = curNode.prop_[0];
            /* Xだけに入れる */            
            for( unsigned int valveIdx=0;valveIdx<curOutput.sizeAsVec4();++valveIdx)
            {
                const nlFloat32 t = input0.atAsVec4(valveIdx).x_;
                const nlFloat32 result = radius*nlCos(t);
                nlVec4 writeValue = nlVec4::create(result,0.0f,0.0f,0.0f);
                curProcess.output_.writeAsVec4(valveIdx,writeValue);
            }
        }
        break;
    case SNT_COSSIN:
        {
            const nlFloat32 radius  = curNode.prop_[0];
            /* X,Yだけに入れる */            
            for( unsigned int valveIdx=0;valveIdx<curOutput.sizeAsVec4();++valveIdx)
            {
                const nlFloat32 t = input0.atAsVec4(valveIdx).x_;
                const nlFloat32 x = radius*nlCos(t);
                const nlFloat32 y = radius*nlSin(t);
                const nlVec4 writeValue = nlVec4::create(x,y,0.0f,0.0f);
                curProcess.output_.writeAsVec4(valveIdx,writeValue);
            }
        }
        break;
    case SNT_ATAN:
        {
            for( unsigned int valveIdx=0;valveIdx<curOutput.sizeAsVec4();++valveIdx)
            {
                nlFloat32 result = nlAtan( input0.atAsVec4(valveIdx).x_ );
                const nlVec4 writeValue = nlVec4::create(result,0.0f,0.0f,0.0f);
                curProcess.output_.writeAsVec4(valveIdx,writeValue);
            }
        }
        break;
    case SNT_NORM:
        {
            const nlFloat32 scale  = curNode.prop_[0];
            for( unsigned int valveIdx=0;valveIdx<curOutput.sizeAsVec4();++valveIdx)
            {
                const nlVec4& v4 = input0.atAsVec4(valveIdx);
                nlFloat32 len = nlSqrt( v4.x_*v4.x_+v4.y_*v4.y_+v4.z_*v4.z_+v4.w_*v4.w_ );
                if( nlAbsf(len) <= FLT_MIN*10.0f )
                {
                    len = FLT_MIN*10.0f;
                }
                len /= scale;
                const nlVec4 writeValue = nlVec4::create(v4.x_/len,v4.y_/len,v4.z_/len,v4.w_/len);
                curProcess.output_.writeAsVec4(valveIdx,writeValue);
            }
        }
        break;
    case SNT_TAN:
        {
            const nlFloat32 radius  = curNode.prop_[0];
            /* Xだけに入れる */            
            for( unsigned int valveIdx=0;valveIdx<curOutput.sizeAsVec4();++valveIdx)
            {
                const nlFloat32 t = input0.atAsVec4(valveIdx).x_;
                const nlFloat32 result = radius*nlTan(t);
                const nlVec4 writeValue = nlVec4::create(result,0.0f,0.0f,0.0f);
                curProcess.output_.writeAsVec4(valveIdx,writeValue);
            }
        }
        break;
    case SNT_EXP:
        {
            const nlFloat32 a = curNode.prop_[0];
            const nlFloat32 b = curNode.prop_[1];
            /* Xにだけ入れる */
            for( unsigned int valveIdx=0;valveIdx<curOutput.sizeAsVec4();++valveIdx)
            {
                const nlVec4& v4 = input0.atAsVec4(valveIdx);
                const nlFloat32 x = b*nlExpf( a * v4.x_ );
                const nlVec4 writeValue = nlVec4::create(x,0.0f,0.0f,0.0f);
                curProcess.output_.writeAsVec4(valveIdx,writeValue);
            }
        }
        break;
    case SNT_EXPSTEP:
        {
            const nlFloat32 k = curNode.prop_[0];
            const nlFloat32 n = curNode.prop_[1];
            /* Xにだけ入れる */
            for( unsigned int valveIdx=0;valveIdx<curOutput.sizeAsVec4();++valveIdx)
            {
                const nlVec4& v4 = input0.atAsVec4(valveIdx);
                nlFloat32 x ;
                if( nlAbsf(v4.x_) < NL_FLT_MIN )
                { x = 1.0f; }
                else
                { x = nlExpf( -k*nlPowf(v4.x_,n) ); }

                const nlVec4 writeValue = nlVec4::create(x,0.0f,0.0f,0.0f);
                curProcess.output_.writeAsVec4(valveIdx,writeValue);
            }
        }
        break;
    case SNT_GAMMA:
        {
            const nlFloat32 start = curNode.prop_[0];
            const nlFloat32 end   = curNode.prop_[1];
            const nlFloat32 power = curNode.prop_[2];
            /**/
            for( unsigned int valveIdx=0;valveIdx<curOutput.sizeAsVec4();++valveIdx)
            {
                const nlVec4& v4 = input0.atAsVec4(valveIdx);
                /* 正規化 */
                nlFloat32 nx = (v4.x_ - start)/(end-start);
                nlFloat32 ny = (v4.y_ - start)/(end-start);
                nlFloat32 nz = (v4.z_ - start)/(end-start);
                nlFloat32 nw = (v4.w_ - start)/(end-start);
                /* クランプ */
                nx = MN( MX(nx,0.0f), 1.0f );
                ny = MN( MX(ny,0.0f), 1.0f );
                nz = MN( MX(nz,0.0f), 1.0f );
                nw = MN( MX(nw,0.0f), 1.0f );
                /* ガンマ */
                nx = nlPowf( nx , power );
                ny = nlPowf( ny , power );
                nz = nlPowf( nz , power );
                nw = nlPowf( nw , power );
                const nlVec4 writeValue = nlVec4::create(nx,ny,nz,nw);
                curProcess.output_.writeAsVec4(valveIdx,writeValue);
            }
        }
        break;

    case SNT_MOD:
        {
            const nlFloat32 cutValue  = curNode.prop_[0];
            for( unsigned int valveIdx=0;valveIdx<curOutput.sizeAsVec4();++valveIdx)
            {
                const nlVec4 orig = input0.atAsVec4(valveIdx);
                const nlFloat32 x = nlFmodf( orig.x_, cutValue );
                const nlFloat32 y = nlFmodf( orig.y_, cutValue );
                const nlFloat32 z = nlFmodf( orig.z_, cutValue );
                const nlFloat32 w = nlFmodf( orig.w_, cutValue );
                const nlVec4 writeValue = nlVec4::create(x,y,z,w);
                curProcess.output_.writeAsVec4(valveIdx,writeValue);
            }
        }
        break;
    case SNT_LENGTH:
        {
            for( unsigned int valveIdx=0;valveIdx<curOutput.sizeAsVec4();++valveIdx)
            {
                const nlVec4 orig = input0.atAsVec4(valveIdx);
                const nlFloat32 len = nlSqrt( orig.x_*orig.x_ + orig.y_*orig.y_ + orig.z_*orig.z_ + orig.w_*orig.w_ );
                const nlVec4 writeValue = nlVec4::create( len, 0, 0, 0 );
                curProcess.output_.writeAsVec4(valveIdx,writeValue);
            }
        }
        break;
    case SNT_DISTANCE:
        {
            const nlFloat32 x  = curNode.prop_[0];
            const nlFloat32 y  = curNode.prop_[1];
            const nlFloat32 z  = curNode.prop_[2];
            const nlFloat32 w  = curNode.prop_[3];
            for( unsigned int valveIdx=0;valveIdx<curOutput.sizeAsVec4();++valveIdx)
            {
                const nlVec4 orig = input0.atAsVec4(valveIdx);
                const nlFloat32 dx = orig.x_-x;
                const nlFloat32 dy = orig.y_-y;
                const nlFloat32 dz = orig.z_-z;
                const nlFloat32 dw = orig.w_-w;
                const nlFloat32 dist = nlSqrt( dx*dx+dy*dy+dz*dz+dw*dw );
                const nlVec4 writeValue = nlVec4::create( dist, 0, 0, 0 );
                curProcess.output_.writeAsVec4(valveIdx,writeValue);
            }
        }
        break;
    case SNT_REPEAT:
        {
            for( unsigned int valveIdx=0;valveIdx<curOutput.sizeAsVec4();++valveIdx)
            {
                if( input0.sizeAsVec4() == 0 )
                {
                    const nlVec4 v = {0.0f,0.0f,0.0f,0.0f};
                    curProcess.output_.writeAsVec4(valveIdx,v);
                }
                else
                {
                    const unsigned int index = (valveIdx%input0.sizeAsVec4());
                    curProcess.output_.writeAsVec4(valveIdx,input0.atAsVec4(index));
                }
            }
        }
        break;
#if 0
    case SNT_SMOOTHSTEP:
        {
            /* only for x element. */
            const nlFloat32 start = curNode.prop_[0];
            const nlFloat32 end   = curNode.prop_[1];
            for( unsigned int valveIdx=0;valveIdx<curOutput.sizeAsVec4();++valveIdx)
            {
                const nlFloat32 x = input0.atAsVec4(valveIdx).x_;
                const nlFloat32 tmp = (x-start)/(end-start);
                const nlFloat32 tmp2 = MX( MN(tmp,1.0f), 0.0f);
                const nlFloat32 result = tmp2*tmp2*(3-2*tmp2);
                const nlVec4 writeValue = nlVec4::create(result,0.0f,0.0f,0.0f);
                curProcess.output_.writeAsVec4(valveIdx,writeValue);
            }
        }
        break;
#endif
    case SNT_CONSTV4:
        {
            /* 全ての要素を指定した要素で埋める。 */
            for( unsigned int valveIdx=0;valveIdx<curOutput.sizeAsVec4();++valveIdx)
            {
                const nlVec4 writeValue = nlVec4::create(curNode.prop_[0],curNode.prop_[1],curNode.prop_[2], curNode.prop_[3] );
                curOutput.writeAsVec4(valveIdx,writeValue);
            }
        }
        break;
    case SNT_MAKEV4:
        {
            /* それぞれの入力を要素に持つVector4を作成する */
            const nlNodeValveArray& xs = input0;
            const nlNodeValveArray& ys = input1;
            const nlNodeValveArray& zs = input2;
            const nlNodeValveArray& ws = input3;
            for( unsigned int valveIdx=0;valveIdx<curOutput.sizeAsVec4();++valveIdx)
            {
                const nlVec4 writeValue = nlVec4::create(
                    xs.atAsVec4(valveIdx).x_,
                    ys.atAsVec4(valveIdx).x_,
                    zs.atAsVec4(valveIdx).x_,
                    ws.atAsVec4(valveIdx).x_ );
                curOutput.writeAsVec4(valveIdx,writeValue);
            }
        }
        break;
    case SNT_SWIZZLE2:
        {
            const nlNodeValveArray& valveInVals = input0;
            unsigned int xSwitch = (int)(curNode.prop_[0]);
            unsigned int ySwitch = (int)(curNode.prop_[1]);
            unsigned int zSwitch = (int)(curNode.prop_[2]);
            unsigned int wSwitch = (int)(curNode.prop_[3]);
            xSwitch = MN(MX(xSwitch,0),4);
            ySwitch = MN(MX(ySwitch,0),4);
            zSwitch = MN(MX(zSwitch,0),4);
            wSwitch = MN(MX(wSwitch,0),4);
            /* Swizzleing */
            for( unsigned int valveIdx=0;valveIdx<curOutput.sizeAsVec4();++valveIdx)
            {
                nlVec4 tmp = valveInVals.atAsVec4(valveIdx);
                const nlFloat32 x = xSwitch==4?0.0f:tmp.e_[xSwitch];
                const nlFloat32 y = ySwitch==4?0.0f:tmp.e_[ySwitch];
                const nlFloat32 z = zSwitch==4?0.0f:tmp.e_[zSwitch];
                const nlFloat32 w = wSwitch==4?0.0f:tmp.e_[wSwitch];
                const nlVec4 writeValue = nlVec4::create(x,y,z,w);
                curProcess.output_.writeAsVec4(valveIdx,writeValue);
            }
        }
        break;
#if 0
    case SNT_BINARIZE:
        { 
            /* 一定の値を超えていたら1を、そうでなければ0を出力する */
            const nlFloat32 threshold = curNode.prop_[0];
            for( unsigned int valveIdx=0;valveIdx<curOutput.sizeAsVec4();++valveIdx)
            {
                if( input0.atAsVec4(valveIdx).x_ > threshold )
                {
                    curProcess.output_.writeAsVec4(valveIdx, nlVec4::create(1.0f,1.0f,1.0f,1.0f));
                }
                else
                {
                    curProcess.output_.writeAsVec4(valveIdx, nlVec4::create(0.0f,0.0f,0.0f,0.0f));
                }

            }
        }
        break;
#endif
#if 0
    case SNT_BLEND:
        { 
            for( unsigned int valveIdx=0;valveIdx<curOutput.sizeAsVec4();++valveIdx)
            {
                const nlVec4 s1 = input0.atAsVec4(valveIdx);
                const nlVec4 s2 = input1.atAsVec4(valveIdx);
                nlFloat32 f   = input2.atAsVec4(valveIdx).x_;
                f = MN(MX(f,0.0f),1.0f);
                const nlFloat32 inf = 1.0f-f;
                nlFloat32 x = s1.x_*f + s2.x_*inf;
                nlFloat32 y = s1.y_*f + s2.y_*inf;
                nlFloat32 z = s1.z_*f + s2.z_*inf;
                nlFloat32 w = s1.w_*f + s2.w_*inf;
                curProcess.output_.writeAsVec4(valveIdx, nlVec4::create(x,y,z,w) );
            }
        }
        break;
#endif
    case SNT_SCENETIME:
        {
            /* シーン開始からの経過フレーム数を全要素に書き込む */
            const int sceneIdx = nlScene_sceneIndex(nowInMs);
            const int startTimeInMs = nlScene_timeInMs(sceneIdx);
            const int elapseTimeInMs = nowInMs - startTimeInMs;
            const nlFloat32 scale  = curNode.prop_[0];
            const nlFloat32 offset = curNode.prop_[1];
            const nlFloat32 value  = (elapseTimeInMs+offset)*scale;
            for( unsigned int valveIdx=0;valveIdx<curOutput.sizeAsVec4();++valveIdx)
            {
                const nlVec4 writeValue = nlVec4::create(value,value,value,value);
                curProcess.output_.writeAsVec4(valveIdx,writeValue);
            }
        }
        break;
    case SNT_SYSTEM_CONST:
        {
            /**/
            nlVec4 value = {0.0f,0.0f,0.0f,0.0f};
            const unsigned int type = curNode.prop_[0];
            switch( type )
            {
            case 0:/* バックバッファ解像度 */
                value.x_ = cxt.rendertargets[0].width_;
                value.y_ = cxt.rendertargets[0].height_;
                break;
            case 1:/* アスペクト比 */
                value.x_ = (nlFloat32)cxt.rendertargets[0].width_/(nlFloat32)cxt.rendertargets[0].height_;
                break;
            default:
                break;
            }
            /* 書き込み */
            for( unsigned int valveIdx=0;valveIdx<curOutput.sizeAsVec4();++valveIdx)
            {
                curProcess.output_.writeAsVec4(valveIdx,value);
            }
        }
        break;
    case SNT_RANGE:
        {
            const unsigned int prop0 = (unsigned int)curNode.prop_[0];
            const unsigned int prop1 = (unsigned int)curNode.prop_[1];
            const unsigned int loopX = prop0 ? prop0 : 1;
            const unsigned int loopY = prop1 ? prop1 : 1;
            /* ループさせる */
            nlFloat32 loopZ = (nlFloat32)(curOutput.sizeAsVec4()/(nlFloat32)(loopX*loopY));
            const nlFloat32 difX  = curNode.prop_[2];
            const nlFloat32 difY  = curNode.prop_[3];
            const nlFloat32 difZ  = curNode.prop_[4];
            for( unsigned int valveIdx=0;valveIdx<curOutput.sizeAsVec4();++valveIdx)
            {
                const int xOffset = (valveIdx%loopX)*2         - (loopX-1);
                const int yOffset = ((valveIdx/loopX)%loopY)*2 - (loopY-1);
                const int zOffset = (valveIdx/(loopX*loopY))*2 - ((int)loopZ-1);
                const nlFloat32 x = xOffset*difX*0.5f;
                const nlFloat32 y = yOffset*difY*0.5f;
                const nlFloat32 z = zOffset*difZ*0.5f;
                const nlVec4 writeValue = nlVec4::create(x,y,z,0.0f);
                curProcess.output_.writeAsVec4(valveIdx,writeValue);
            }
        }
        break;
    case SNT_SUBARRAY:
        {
            const unsigned int startIdx = (unsigned int)curNode.prop_[0];
            for( unsigned int valveIdx=0;valveIdx<curOutput.sizeAsVec4();++valveIdx)
            {
                const nlVec4 writeValue = input0.atAsVec4(startIdx+valveIdx);
                curProcess.output_.writeAsVec4(valveIdx,writeValue);
            }
        }
        break;
#if 0
    case SNT_ADRESS:
        {
            const nlFloat32 start  = curNode.prop_[0];
            const nlFloat32 end    = curNode.prop_[1];
            const unsigned int mode = (unsigned int)curNode.prop_[2];
            for( unsigned int valveIdx=0;valveIdx<curOutput.sizeAsVec4();++valveIdx)
            {
                const nlVec4 val = input0.atAsVec4(valveIdx);
                switch( mode )
                {
                case 0:/* clamp */
                    {
                        const nlFloat32 x = MN( MX( val.x_, start ), end );
                        const nlFloat32 y = MN( MX( val.y_, start ), end );
                        const nlFloat32 z = MN( MX( val.z_, start ), end );
                        const nlFloat32 w = MN( MX( val.w_, start ), end );
                        const nlVec4 writeValue = nlVec4::create(x,y,z,w);
                        curProcess.output_.writeAsVec4(valveIdx,writeValue);
                    }
                    break;
                case 1:/* wrap */
                    {
                        const nlFloat32 x = nlFmodf( val.x_ - start, end-start ) + start;
                        const nlFloat32 y = nlFmodf( val.y_ - start, end-start ) + start;
                        const nlFloat32 z = nlFmodf( val.z_ - start, end-start ) + start;
                        const nlFloat32 w = nlFmodf( val.w_ - start, end-start ) + start;
                        const nlVec4 writeValue = nlVec4::create(x,y,z,w);
                        curProcess.output_.writeAsVec4(valveIdx,writeValue);
                    }
                    break;
                case 2:/* mirror */
                    {
                        const nlFloat32 x = nlAbsf( val.x_ - start ) + start;
                        const nlFloat32 y = nlAbsf( val.y_ - start ) + start;
                        const nlFloat32 z = nlAbsf( val.z_ - start ) + start;
                        const nlFloat32 w = nlAbsf( val.w_ - start ) + start;
                        const nlVec4 writeValue = nlVec4::create(x,y,z,w);
                        curProcess.output_.writeAsVec4(valveIdx,writeValue);
                    }
                    break;
                }
            }
        }
        break;
#endif
    case SNT_RANDOM:
        {
            /* val = (rand()%(max-min))+min */
            const nlFloat32 seed = curNode.prop_[0] * 8388607;
            const nlFloat32 min  = curNode.prop_[1];
            const nlFloat32 max  = curNode.prop_[2];
            //NL_ASSERT( min <= max );
            nlSetRandSeed_Local( (unsigned int)seed );
            for( unsigned int valveIdx=0;valveIdx<curOutput.sizeAsVec4();++valveIdx)
            {
                nlFloat32 factor = (nlFloat32)nlRand_Local()/(nlFloat32)NLRAND_LOCAL_MAX;
                const nlFloat32 x = min+(max-min)*factor;
                factor = (nlFloat32)nlRand_Local()/(nlFloat32)NLRAND_LOCAL_MAX;
                const nlFloat32 y = min+(max-min)*factor;
                factor = (nlFloat32)nlRand_Local()/(nlFloat32)NLRAND_LOCAL_MAX;
                const nlFloat32 z = min+(max-min)*factor;
                factor = (nlFloat32)nlRand_Local()/(nlFloat32)NLRAND_LOCAL_MAX;
                const nlFloat32 w = min+(max-min)*factor;
                nlVec4 writeValue = nlVec4::create(x,y,z,w);
                curProcess.output_.writeAsVec4(valveIdx,writeValue);
            }
        }
        break;
    case SNT_CURVE:
        {
            const unsigned int id = (unsigned int)curNode.prop_[0];
            const nlFloat32 scale          = curNode.prop_[1];
            const unsigned int isWrap  = (unsigned int)curNode.prop_[2];
            const unsigned int isIntegral  = (unsigned int)curNode.prop_[3];
            const nlCurve& path    = cxt.paths[id];
            nlFloat32 len             = path.points_.length();
            /*  */
            for( unsigned int valveIdx=0;valveIdx<curOutput.sizeAsVec4();++valveIdx)
            {
                nlFloat32 x = input0.atAsVec4(valveIdx).x_;
                if( isWrap )
                { x = nlFmodf( x, len ); }
                else
                { x = MN( MX(x,0.0f), len ); }

                nlVec3 xyz = nlVec3::Create(0,0,0);

                if( isIntegral )
                {
                    const int goal = (unsigned int)x;
                    for( int i=0;i<goal;++i )
                    {
                        nlVec3 tmp = nlGetCurveValue( path, i );
                        xyz.x_ += tmp.x_;
                        xyz.y_ += tmp.y_;
                        xyz.z_ += tmp.z_;
                    }
                }
                else
                {
                    xyz = nlGetCurveValue( path, (unsigned int)x );
                }
                const nlVec4 xyzw = nlVec4::create( xyz.x_*scale, xyz.y_*scale, xyz.z_*scale, 0.0f );
                curProcess.output_.writeAsVec4(valveIdx,xyzw);
            }
        }
        break;
    case SNT_OBJECT:
        {
            const int groupId = (int)curNode.prop_[0];
            const int meshId  = (int)curNode.prop_[1];
            const int numObj  = (int)curNode.prop_[2];
            nlNodeValveArray* vs0 = const_cast<nlNodeValveArray*>( &input0 );
            nlNodeValveArray* vs1 = const_cast<nlNodeValveArray*>( &input1 );
            nlNodeValveArray* ps0 = const_cast<nlNodeValveArray*>( &input2 );
            nlNodeValveArray* ps1 = const_cast<nlNodeValveArray*>( &input3 );
            rc.addObject( groupId, meshId, numObj, vs0, vs1, ps0, ps1 );
        }
        break;
    case SNT_DEBUG_CAMERA:
        {
            /* 入力をそのまま入力する。DebugCameraを利用する場合はこの後に出力を変更している。 */
            for( unsigned int valveIdx=0;valveIdx<curOutput.sizeAsMatrix();++valveIdx)
            {
                nlMatrix viewMatrix = input0.atAsMatrix(valveIdx);
                curProcess.output_.writeAsMatrix( valveIdx, viewMatrix );
            }
        }
        break;
    case SNT_DEBUG_CAMERA_POS:
        {
            /* 入力をそのまま入力する。DebugCameraを利用する場合はこの後に出力を変更している。 */
            for( unsigned int valveIdx=0;valveIdx<curOutput.sizeAsMatrix();++valveIdx)
            {
                curProcess.output_.writeAsMatrix( valveIdx, curOutput.atAsMatrix(valveIdx) );
            }
        }
        break;
    case SNT_MATRIX_VIEW:
        {
            for( unsigned int valveIdx=0;valveIdx<curOutput.sizeAsVec4();++valveIdx)
            {
                nlMatrix view;
                const nlVec4 eyePos = input0.atAsVec4(valveIdx);
                const nlVec4 lookAt = input1.atAsVec4(valveIdx);
                const nlVec4 upDir  = input2.atAsVec4(valveIdx);
                D3DXMatrixLookAtRH( &view, (D3DXVECTOR3*)&eyePos, (D3DXVECTOR3*)&lookAt, (D3DXVECTOR3*)&upDir );
                curProcess.output_.writeAsMatrix( valveIdx, view );
            }
        }
        break;
    case SNT_MATRIX_PROJ:
        {
            for( unsigned int valveIdx=0;valveIdx<curOutput.sizeAsMatrix();++valveIdx)
            {
                nlMatrix proj;
                const nlFloat32 zn     = input0.atAsVec4(valveIdx).x_;
                const nlFloat32 zf     = input1.atAsVec4(valveIdx).x_;
                const nlFloat32 aspect = input2.atAsVec4(valveIdx).x_;
                const nlFloat32 fovy   = input3.atAsVec4(valveIdx).x_;
                D3DXMatrixPerspectiveFovRH( &proj, fovy, aspect, zn, zf );
                curProcess.output_.writeAsMatrix( valveIdx, proj );
            }
        }
        break;
    case SNT_MATRIX_PROJ_ORTHO:
        {
            for( unsigned int valveIdx=0;valveIdx<curOutput.sizeAsMatrix();++valveIdx)
            {
                nlMatrix proj;
                const nlFloat32 zn     = input0.atAsVec4(valveIdx).x_;
                const nlFloat32 zf     = input1.atAsVec4(valveIdx).x_;
                const nlFloat32 w      = input2.atAsVec4(valveIdx).x_;
                const nlFloat32 h      = input3.atAsVec4(valveIdx).x_;
                D3DXMatrixOrthoRH( &proj, w, h, zn, zf );
                curProcess.output_.writeAsMatrix( valveIdx, proj );
            }
        }
        break;
    case SNT_MATRIX_WORLD:
        {
            for( unsigned int valveIdx=0;valveIdx<curOutput.sizeAsMatrix();++valveIdx)
            {
                nlVec4 trance   = input0.atAsVec4(valveIdx);
                nlVec4 rotation = input1.atAsVec4(valveIdx);
                nlVec4 scale    = input2.atAsVec4(valveIdx);
                if( !input2.sizeAsVec4() )
                { scale = nlVec4_create(1,1,1,1); }
                /**/
                nlMatrix matTrance, matRotation, matScale, matTmp,matWorld;
                D3DXMatrixTranslation( &matTrance, trance.x_, trance.y_, trance.z_ );
                D3DXMatrixRotationYawPitchRoll( &matRotation, rotation.x_, rotation.y_, rotation.z_ );
                D3DXMatrixScaling( &matScale, scale.x_, scale.y_, scale.z_ );
                /**/
                D3DXMatrixMultiply( &matTmp, &matScale, &matRotation );
                D3DXMatrixMultiply( &matWorld, &matTmp, &matTrance );
                /**/
                curProcess.output_.writeAsMatrix( valveIdx, matWorld );
            }
        }
        break;
    case SNT_MATRIX_SCALE:
        {
            for( unsigned int valveIdx=0;valveIdx<curOutput.sizeAsMatrix();++valveIdx)
            {
                nlVec4 scale = input0.atAsVec4(valveIdx);
                nlMatrix matScale;
                D3DXMatrixScaling( &matScale, scale.x_, scale.y_, scale.z_ );
                curProcess.output_.writeAsMatrix( valveIdx, matScale );
            }
        }
        break;
    case SNT_MATRIX_TRANCE:
        {
            for( unsigned int valveIdx=0;valveIdx<curOutput.sizeAsMatrix();++valveIdx)
            {
                nlVec4 trance   = input0.atAsVec4(valveIdx);
                nlMatrix matTrance;
                D3DXMatrixTranslation( &matTrance, trance.x_, trance.y_, trance.z_ );
                curProcess.output_.writeAsMatrix( valveIdx, matTrance );
            }
        }
        break;
    case SNT_MATRIX_ROT:
        {
            for( unsigned int valveIdx=0;valveIdx<curOutput.sizeAsMatrix();++valveIdx)
            {
                nlVec4 rotation = input0.atAsVec4(valveIdx);
                nlMatrix matRotation;
                D3DXMatrixRotationYawPitchRoll( &matRotation, rotation.x_, rotation.y_, rotation.z_ );
                curProcess.output_.writeAsMatrix( valveIdx, matRotation );
            }
        }
        break;
    case SNT_MATRIX_ROTZ:
        {
            for( unsigned int valveIdx=0;valveIdx<curOutput.sizeAsMatrix();++valveIdx)
            {
                const nlMatrix& origMat = input0.atAsMatrix(valveIdx);
                nlFloat32 angle = input1.atAsVec4(valveIdx).x_;
                nlMatrix matRotation, matResult;
                D3DXMatrixRotationZ(&matRotation,angle);
                D3DXMatrixMultiply( &matResult, &origMat, &matRotation );
                curProcess.output_.writeAsMatrix( valveIdx, matResult );
            }
        }
        break;
    case SNT_MATRIX_MUL:
        {
            for( unsigned int valveIdx=0;valveIdx<curOutput.sizeAsMatrix();++valveIdx)
            {
                nlMatrix result;
                nlMatrix src0 = input0.atAsMatrix(valveIdx);
                nlMatrix src1 = input1.atAsMatrix(valveIdx);
                D3DXMatrixMultiply( &result, &src0, &src1 );
                curProcess.output_.writeAsMatrix( valveIdx, result );
            }
        }
        break;
    case SNT_MATRIX_INV:
        {
            for( unsigned int valveIdx=0;valveIdx<curOutput.sizeAsMatrix();++valveIdx)
            {
                nlMatrix result;
                nlMatrix src = input0.atAsMatrix(valveIdx);
                D3DXMatrixInverse( &result, NULL, &src );
                curProcess.output_.writeAsMatrix( valveIdx, result );
            }
        }
        break;
    case SNT_MATRIX_PACK:
        {
            for( unsigned int valveIdx=0;valveIdx<curOutput.sizeAsMatrix();++valveIdx)
            {
                const nlVec4 src0 = input0.atAsVec4(valveIdx);
                const nlVec4 src1 = input1.atAsVec4(valveIdx);
                const nlVec4 src2 = input2.atAsVec4(valveIdx);
                const nlVec4 src3 = input3.atAsVec4(valveIdx);
                nlMatrix result(
                    src0.x_, src0.y_, src0.z_, src0.w_, 
                    src1.x_, src1.y_, src1.z_, src1.w_, 
                    src2.x_, src2.y_, src2.z_, src2.w_, 
                    src3.x_, src3.y_, src3.z_, src3.w_ );
                curProcess.output_.writeAsMatrix( valveIdx, result );
            }
        }
        break;
        /* レンダーコマンド関係はここでは何もしないことに注意 */
    case SNT_BEGINRENDER:
    case SNT_ENDRENDER:
    case SNT_PASS:
    case SNT_CLEARRT:
    case SNT_CLEARDS:
    case SNT_SET_Z_STATE:
    case SNT_SET_ALPHA_STATE:
    case SNT_SETRENDERTARGET:
    case SNT_SET_CB:
    case SNT_SET_TEX:
    case SNT_SET_TEX_RT:
    case SNT_RENDER_SCREEN2:
    case SNT_RENDER_SCREEN3:
    case SNT_DRAWGROUP:
        break;
    default:
        NL_ASSERT(!"存在しないノードです");
        break;
    }/* switch( curNode.type_ ) */

    curProcess.isProcessed_ = true;
}


/**/
static const nlNodeValveArray& getPrevNodeOutput( 
    const ProcessBuffer& allProcessBuffer,
    const nlSceneNode& node, unsigned int prevIdx )
{
    const  unsigned int nodeIdx = node.prevNode_[prevIdx];
    NL_ASSERT( (nodeIdx==0xffffffff) || (nodeIdx < MAX_SCENENODE_NUM) );
    /**/
    if( nodeIdx != 0xffffffff )
    {
        return allProcessBuffer[nodeIdx].output_;
        //return processBuffer[nodeIdx].output_;
    }
    else
    {
        return *nullValveArray;
    }
}

/**/
static void constructRenderpipe( nlRenderCommands& rc, const SceneMap& sceneMap, int numSceneMap, ProcessBuffer& processBuffer )
{
    /* RenderEndを探す */
    unsigned int nextIndex = -1;
    for( int i=0;i<numSceneMap;++i)
    {
        const nlSceneNode& node = sceneMap[i];
        if( node.type_ == SNT_ENDRENDER )
        {
            nextIndex = node.prevNode_[0];
        }
    }

    /* 後ろからRenderCommandをたどっていく */
    while( nextIndex != -1 )
    {
        /**/
        const nlSceneNode& node = sceneMap[nextIndex];
        //const nlNodeValveArray& input0 = getPrevNodeOutput(processBuffer,curNode,0); /* これは利用されない */
        const nlNodeValveArray& input1 = getPrevNodeOutput(processBuffer,node,1);
        const nlNodeValveArray& input2 = getPrevNodeOutput(processBuffer,node,2);
        const nlNodeValveArray& input3 = getPrevNodeOutput(processBuffer,node,3);
        const nlNodeValveArray& input4 = getPrevNodeOutput(processBuffer,node,4);
        /* 次のコマンド */
        nextIndex = node.prevNode_[0];
        const nlArray<nlFloat32,SCENENODE_PROP_NUM>& props = node.prop_;
        /**/
        switch( node.type_ )
        {
        case SNT_PASS:
            /* 特に何もしない */
            break;
        case SNT_CLEARRT:
            rc.clearRT( node.prop_[0], node.prop_[1], node.prop_[2], node.prop_[3] );
            break;
        case SNT_CLEARDS:
            rc.clearDS( node.prop_[0] );
            break;
        case SNT_SET_Z_STATE:
            rc.setZState( (ZState)((int)node.prop_[0]) );
            break;
        case SNT_SET_ALPHA_STATE:
            rc.setAlphaState( (AlphaState)((int)node.prop_[0]) );
            break;
        case SNT_SETRENDERTARGET:
            rc.setRenderTarget( node.prop_[0], node.prop_[1], node.prop_[2], node.prop_[3] );
            break;
        case SNT_SET_CB:
            {
                const int shaderTypeInt = node.prop_[0];
                const ShaderType shaderType = static_cast<ShaderType>( shaderTypeInt );
                rc.setConstantBuffer1( shaderType, node.prop_[1], node.prop_[2], &input1, &input2, &input3, &input4 );
            }
            break;
        case SNT_SET_TEX:
            rc.setTex( node.prop_[0], node.prop_[1] );
            break;
        case SNT_SET_TEX_RT:
            rc.setTexRt( node.prop_[0], node.prop_[1] );
            break;
        case SNT_RENDER_SCREEN2:
            rc.renderScreenPoly2(props[0]);
            break;
        case SNT_RENDER_SCREEN3:
            rc.renderScreenPoly3(props[0],props[1]);
            break;
        case SNT_DRAWGROUP:
            rc.drawGroup( props[0],props[1],props[2],props[3], props[4] );
            break;
            /* レンダーコマンド以外、もしくは特別なレンダーコマンドが来たら途中でも中断 */
        case SNT_BEGINRENDER:
        case SNT_ENDRENDER:
        default:
            nextIndex = -1;
            break;
        }
    }
    /* 最後に全てのコマンドを逆順にする */
    rc.reverse();
}
/**/
SceneArray& nlScene_getSceneData()
{
    return *allScenes;
}
ProcessBuffer& nlScene_getProcessBuffer()
{
    return *g_processBuffer;
}
/**/
int nlScene_sceneIndex( unsigned int timeInMs )
{
    int totalTimeInMs = 0;
    int lastNonZero = 0;
    for( int i=0;i<(*allScenes).size();++i)
    {
        /**/
        const int sceneTimeInsMs = (*allScenes)[i].sceneTimeInMs;
        totalTimeInMs += sceneTimeInsMs;
        /**/
        if( sceneTimeInsMs != 0 )
        { lastNonZero = i; }
        /**/
        if( timeInMs < totalTimeInMs )
        {
            return i;
        }
    }
    return lastNonZero;
}
nlScene& nlScene_scene( unsigned int timeInMs )
{
    const int sceneIndex = nlScene_sceneIndex(timeInMs);
    return (*allScenes)[sceneIndex];
}
/**/
unsigned int nlScene_timeInMs( unsigned int sceneInde )
{
    NL_ASSERT( sceneInde < MAX_SCENE_NUM );
    int totalTimeInMs = 0;
    for( int i=0;i<sceneInde;++i)
    {
        totalTimeInMs += (*allScenes)[i].sceneTimeInMs;
    }
    return totalTimeInMs;
}
/**/
unsigned int nlScene_totalSceneNum()
{
    /* フレーム数が0でない最後のシーンが全体の最後のシーン */
    int sceneIndex = 0;
    while( (*allScenes)[sceneIndex].sceneTimeInMs ) 
    { ++sceneIndex; }
    /**/
    return sceneIndex;
}
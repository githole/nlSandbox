#include "main/stdafx.h"
#include "nlRender.h"
#include "shared/nlTypedef.h"
#include "shared/nlMemory.h"
#include "shared/d3d11resource/d3d11resource.hpp"
#include "shared/nlLog.hpp"

#ifndef INTROMODE
#include "tool/nlLogComposer.hpp"
#endif


namespace
{
    /* DepthStencil設定 */
    ID3D11DepthStencilState* depthStencilStates[3];
    /* スクリーンエフェクト用VS */
    nlVertexShader screenVS;
    /* アルファブレンディング */
    ID3D11BlendState* blendStateNoBlend = NULL;
    ID3D11BlendState* blendStateAddBlend = NULL;
    ID3D11BlendState* blendStateAlphaBlend = NULL;
}
/* ヘッダを通して公開される */
nlArray<ID3D11Buffer*,MAX_CONST_BUFFER>cbVsMatrix;
nlArray<ID3D11Buffer*,MAX_CONST_BUFFER>cbGsMatrix;
nlArray<ID3D11Buffer*,MAX_CONST_BUFFER>cbPsMatrix;

static void renderRenderObject( int meshId, const nlMatrix& constVs2, const nlMatrix& constVs3, const nlMatrix& constPs2, const nlMatrix& constPs3, nlEngineContext& cxt );
static void renderScreenPoly( unsigned int psId, nlVertexShader& vs, nlEngineContext* cxt );
/**/
void nlRenderInit( nlEngineContext* cxt )
{
    /**/
    ID3D11Device* d3dDevice = cxt->d3dDevice;
    ID3D11DeviceContext* d3dCxt = cxt->d3dContext;
    /* DepthStencil設定の作成 */
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    nlMemset( &depthStencilDesc, 0, sizeof(depthStencilDesc) );
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
    depthStencilDesc.DepthEnable = TRUE;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc      = D3D11_COMPARISON_LESS;
    d3dDevice->CreateDepthStencilState(&depthStencilDesc, &depthStencilStates[0] );
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    depthStencilDesc.DepthFunc      = D3D11_COMPARISON_LESS;
    d3dDevice->CreateDepthStencilState(&depthStencilDesc, &depthStencilStates[1] );
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    depthStencilDesc.DepthFunc      = D3D11_COMPARISON_ALWAYS;
    d3dDevice->CreateDepthStencilState(&depthStencilDesc, &depthStencilStates[2] );
    /* アルファブレンドの作成 */
    D3D11_BLEND_DESC blendDesc;
    nlMemset(&blendDesc, 0, sizeof(D3D11_BLEND_DESC));
    blendDesc.RenderTarget[0].BlendEnable = FALSE;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    d3dDevice->CreateBlendState(&blendDesc, &blendStateNoBlend);
    nlMemset(&blendDesc, 0, sizeof(D3D11_BLEND_DESC));
    blendDesc.RenderTarget[0].BlendEnable = TRUE;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].BlendOp  = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    d3dDevice->CreateBlendState(&blendDesc, &blendStateAddBlend);
    nlMemset(&blendDesc, 0, sizeof(D3D11_BLEND_DESC));
    blendDesc.RenderTarget[0].BlendEnable = TRUE;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOp  = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    d3dDevice->CreateBlendState(&blendDesc, &blendStateAlphaBlend);
    /**/
    screenVS = nlCreateVertexShader( screenVSScript, sizeof(screenVSScript), "vs", *cxt );   
    /**/
    NL_ASSERT( (cbVsMatrix.size() == cbGsMatrix.size()) && (cbGsMatrix.size() == cbPsMatrix.size() ) );
    for( int i=0;i<cbVsMatrix.size();++i)
    { 
        cbVsMatrix[i] = nlCreateConstBuffer( sizeof(nlMatrix), *cxt );
        cbGsMatrix[i] = nlCreateConstBuffer( sizeof(nlMatrix), *cxt );
        cbPsMatrix[i] = nlCreateConstBuffer( sizeof(nlMatrix), *cxt );
    }
    /**/
    d3dCxt->VSSetConstantBuffers( 0, cbVsMatrix.size()-1, &cbVsMatrix[0] );
    d3dCxt->GSSetConstantBuffers( 0, cbGsMatrix.size()-1, &cbGsMatrix[0] );
    d3dCxt->PSSetConstantBuffers( 0, cbPsMatrix.size()-1, &cbPsMatrix[0] );
}

/**/
void nlRender( const nlRenderCommands& renderCommands, nlEngineContext* cxt )
{
    /**/
    ID3D11Device* d3dDevide = cxt->d3dDevice;
    ID3D11DeviceContext* d3dCxt = cxt->d3dContext;
    IDXGISwapChain* swapChain = cxt->dxgiSwpChain;
    /* RSのリセット */
    d3dCxt->OMSetDepthStencilState(depthStencilStates[ZS_ReadWrite],0);
    /* AlphaBlendのリセット */
    d3dCxt->OMSetBlendState(blendStateNoBlend,0,0xffffffff);
    /**/
    d3dCxt->ClearDepthStencilView( cxt->depthStencils[0].view, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0 );
    d3dCxt->OMSetRenderTargets(1, &cxt->rendertargets[0].renderTargetView_, cxt->depthStencils[0].view );
    /**/
    for( unsigned int i=0;i<renderCommands.commandIndex_;++i)
    {
        const nlRenderCommand& cmd = renderCommands.renderData[i];
        switch( cmd.type_ )
        {
        case nlRenderCommand::RC_CLEAR_RT:
            {
                const int index = cmd.props[0].valI;
                if( index != -1 )
                {
                    const nlFloat32 r = cmd.props[1].valI/255.0f;
                    const nlFloat32 g = cmd.props[2].valI/255.0f;
                    const nlFloat32 b = cmd.props[3].valI/255.0f;
                    const nlFloat32 a = 1.0f;
                    const nlFloat32 clearColor[4] = {r,g,b,a};
                    d3dCxt->ClearRenderTargetView( cxt->rendertargets[index].renderTargetView_, clearColor );
                }
            }
            break;
        case nlRenderCommand::RC_CLEAR_DS:
            {
                const int index = cmd.props[0].valI;
                d3dCxt->ClearDepthStencilView( cxt->depthStencils[index].view, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0 );
            }
            break;
        case nlRenderCommand::RC_Z_STATE:
            {
                /* Z状態を変える */
                const ZState zType = (ZState)cmd.props[0].valI;
                NL_ASSERT( (zType==ZS_ReadWrite) || (zType==ZS_OnlyRead) || (zType==ZS_NotUse) );
                d3dCxt->OMSetDepthStencilState(depthStencilStates[zType],0);
            }
            break;
        case nlRenderCommand::RC_ALPHA_STATE:
            {
                /* アルファ状態を変える */
                const AlphaState alphaState = (AlphaState)cmd.props[0].valI;
                ID3D11BlendState* blendState = NULL;
                switch( alphaState )
                {
                case AS_Disable: blendState = blendStateNoBlend; break;
                case AS_Add:     blendState = blendStateAddBlend; break;
                case AS_Alpha:   blendState = blendStateAlphaBlend; break;
                default: NL_ASSERT(!"対応していないアルファステート"); break;
                }
                d3dCxt->OMSetBlendState(blendState,0,0xffffffff);
            }
            break;
        case nlRenderCommand::RC_SET_RENDERTARGET:
            {
                int numRt = 3;
                nlArray<int,3>rtNo;
                rtNo[0] = (int)cmd.props[0].valI;
                rtNo[1] = (int)cmd.props[1].valI;
                rtNo[2] = (int)cmd.props[2].valI;
                int dsNo  = (int)cmd.props[3].valI;
                /**/
                NL_ASSERT( (rtNo[0]==-1) || ((0<=rtNo[0]) && (rtNo[0]<=cxt->rendertargets.size()) && (cxt->rendertargets[rtNo[0]].renderTargetView_)) );
                NL_ASSERT( (rtNo[1]==-1) || ((0<=rtNo[1]) && (rtNo[1]<=cxt->rendertargets.size()) && (cxt->rendertargets[rtNo[1]].renderTargetView_)) );
                NL_ASSERT( (rtNo[2]==-1) || ((0<=rtNo[2]) && (rtNo[2]<=cxt->rendertargets.size()) && (cxt->rendertargets[rtNo[2]].renderTargetView_)) );
                NL_ASSERT( ((0<=dsNo) && (dsNo<=cxt->depthStencils.size()) && (cxt->depthStencils[dsNo].view)) );
                /**/
                nlArray<ID3D11RenderTargetView*,3> rtViews;
                if( rtNo[2] != -1 ){ rtViews[2] = cxt->rendertargets[rtNo[2]].renderTargetView_; }else{ numRt = 2; }
                if( rtNo[1] != -1 ){ rtViews[1] = cxt->rendertargets[rtNo[1]].renderTargetView_; }else{ numRt = 1; }
                if( rtNo[0] != -1 ){ rtViews[0] = cxt->rendertargets[rtNo[0]].renderTargetView_; }else{ numRt = 0; }
                
#ifndef INTROMODE
                /* サイズのチェック */
                nlDepthStencil& ds = cxt->depthStencils[dsNo];
                const int w = ds.width;
                const int h = ds.height;
                for( int j=0;j<numRt;++j)
                {
                    const int rtW = cxt->rendertargets[rtNo[j]].width_;
                    const int rtH = cxt->rendertargets[rtNo[j]].height_;
                    if( ( rtW != w ) && ( rtH != h ) )
                    {
                        NL_ERR( ERR_017, j, i, w, h, rtW, rtH );
                    }
                }
#endif
                d3dCxt->OMSetRenderTargets( numRt, &rtViews[0], cxt->depthStencils[dsNo].view );
            }
            break;
        case nlRenderCommand::RC_SET_CB:
            {
                /* 一番最初の要素のみを設定する */
                ShaderType shaderType = (ShaderType)cmd.props[0].valI;
                const int startIndex = cmd.props[1].valI;
                const int numConst = cmd.props[2].valI;
                nlNodeValveArray* c0 = reinterpret_cast<nlNodeValveArray*>( cmd.props[3].valI );
                nlNodeValveArray* c1 = reinterpret_cast<nlNodeValveArray*>( cmd.props[4].valI );
                nlNodeValveArray* c2 = reinterpret_cast<nlNodeValveArray*>( cmd.props[5].valI );
                nlNodeValveArray* c3 = reinterpret_cast<nlNodeValveArray*>( cmd.props[6].valI );
                NL_ASSERT( (0<=numConst) && (numConst<=4) );
                nlMatrix cMat[4];
                if(c0){ cMat[0] = c0->atAsMatrix(0); }
                if(c1){ cMat[1] = c1->atAsMatrix(0); }
                if(c2){ cMat[2] = c2->atAsMatrix(0); }
                if(c3){ cMat[3] = c3->atAsMatrix(0); }
                /**/
                switch( shaderType )
                {
                case ShaderType_VS:
                    NL_ASSERT( startIndex < cbVsMatrix.size() );
                    for( int i=0;i<numConst;++i)
                    {
                        const int targetIndex = startIndex+i;
                        nlCopyToD3DBuffer( cbVsMatrix[targetIndex], (char*)&cMat[i], sizeof(nlMatrix), *cxt );
                    }
                    break;
                case ShaderType_GS:
					NL_ASSERT( startIndex < cbGsMatrix.size() );
                    for( int i=0;i<numConst;++i)
                    {
                        const int targetIndex = startIndex+i;
                        nlCopyToD3DBuffer( cbGsMatrix[targetIndex], (char*)&cMat[i], sizeof(nlMatrix), *cxt );
                    }
                    break;
                case ShaderType_PS:
                    NL_ASSERT( startIndex < cbPsMatrix.size() );
                    for( int i=0;i<numConst;++i)
                    {
                        const int targetIndex = startIndex+i;
                        nlCopyToD3DBuffer( cbPsMatrix[targetIndex], (char*)&cMat[i], sizeof(nlMatrix), *cxt );
                    }
                    break;
                default:
                    NL_ASSERT(!"存在しないシェーダタイプ");
                    break;
                }
            }
            break;
        case nlRenderCommand::RC_SET_TS:
            {
#if 0
                const int texIndex = (int)cmd.props[0].valI;
                const int index = (int)cmd.props[1].valI;
                //NL_ASSERT( index < MAX_PIXEL_SHADER_NUM );
                d3dCxt->PSSetShaderResources(index, 1, &cxt->texs[texIndex].shaderView );
#endif
            }
            break;
        case nlRenderCommand::RC_SET_TS_RT:
            {
                const int rtIndex = (int)cmd.props[0].valI;
                const int index = (int)cmd.props[1].valI;
                NL_ASSERT( (rtIndex == -1) || (0<= rtIndex && rtIndex < MAX_RENDERTARGET_NUM) );
                if( rtIndex != -1 )
                {
                    ID3D11ShaderResourceView* rtSahderView = cxt->rendertargets[rtIndex].renderTargetShaderResource_;
                    d3dCxt->PSSetShaderResources(index, 1, &rtSahderView );
                }
            }
            break;
        case nlRenderCommand::RC_RENDER_SCREEN_POLY2:
            {
                const int psId = cmd.props[0].valI;
                renderScreenPoly(psId,screenVS,cxt);
            }
            break;
        case nlRenderCommand::RC_RENDER_SCREEN_POLY3:
            {
                const int psId = cmd.props[0].valI;
                const int vsId = cmd.props[1].valI;
                if( vsId >= 0 )
                {
                    nlVertexShader& vs = cxt->vss[vsId];
                    renderScreenPoly(psId,vs,cxt);
                }
            }
            break;
        case nlRenderCommand::RC_RENDER_DRAW_GROUP:
            {
                /**/
                const int groupId = cmd.props[0].valI;
                const int vsId = cmd.props[1].valI;
                const int gsId = cmd.props[2].valI;
                const int psId = cmd.props[3].valI;
                const int tpId = cmd.props[4].valI;
                /* シェーダの設定 */
                NL_ASSERT( vsId < MAX_SHADER_NUM );
                NL_ASSERT( gsId < MAX_SHADER_NUM );
                NL_ASSERT( psId < MAX_SHADER_NUM );
                /* VS設定 */
                if( vsId != -1 )
                {
                    nlVertexShader& vs = cxt->vss[vsId];
                    NL_ASSERT( vs.inputLayout_ && vs.shader_ );
                    d3dCxt->IASetInputLayout( vs.inputLayout_ );
                    d3dCxt->VSSetShader( vs.shader_, NULL, 0 );
                }
				else
				{
                    /* 
                      VSにNULL指定は良くない
                      http://maverickproj.web.fc2.com/d3d11_09.html 
                    */
					//d3dCxt->IASetInputLayout( NULL );
                    //d3dCxt->VSSetShader( NULL, NULL, 0 );
				}
                /* GS設定 */
                if( gsId != -1 )
                {
                    nlGeometoryShader& gs = cxt->gss[gsId];
                    //NL_ASSERT( gs.shader ); /* 本来はここにNULLはこないがそのうち対処 */
					if( gs.shader )
					{ d3dCxt->GSSetShader(gs.shader,NULL,0); }
					else
					{ d3dCxt->GSSetShader(NULL,NULL,0); }
                }
                else
                {
                    d3dCxt->GSSetShader(NULL,NULL,0);
                }
                /* PS設定 */
                if( psId != -1 )
                {
                    ID3D11PixelShader* ps = cxt->pss[psId].shader_;
                    NL_ASSERT( ps );
                    d3dCxt->PSSetShader( ps, NULL, 0 );
                }
				else
				{
					d3dCxt->PSSetShader( NULL, NULL, 0 );
				}

                /* トポロジ指定がなければTRIANGLELISTをデフォルトとして設定する */
                D3D11_PRIMITIVE_TOPOLOGY  primitiveTopology;
                if( tpId == 0 ){ primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST; }
                else           { primitiveTopology = (D3D11_PRIMITIVE_TOPOLOGY)tpId; }
                cxt->d3dContext->IASetPrimitiveTopology( primitiveTopology );

                /* オブジェクトの描画 */
                for( int i=0;i<renderCommands.objBlobIndex_;++i)
                {
                    const ObjectBlob& blob = renderCommands.objectBlobs[i];
                    if( blob.groupId == groupId )
                    {
                        const ObjectBlob& blob = renderCommands.objectBlobs[i];
                        const nlNodeValveArray* constVS0 = blob.constVS0;
                        const nlNodeValveArray* constVS1 = blob.constVS1;
                        const nlNodeValveArray* constPS0 = blob.constPS0;
                        const nlNodeValveArray* constPS1 = blob.constPS1;
                        /**/
                        for( int j=0;j<blob.numObject;++j)
                        {
                            nlMatrix constVs0 = constVS0->atAsMatrix(j);
                            nlMatrix constVs1 = constVS1->atAsMatrix(j);
                            nlMatrix constPs0 = constPS0->atAsMatrix(j);
                            nlMatrix constPs1 = constPS1->atAsMatrix(j);
                            renderRenderObject( blob.meshId, constVs0, constVs1, constPs0, constPs1, *cxt );
                        }
                    }
                }
            }
            break;
        case nlRenderCommand::RS_DEBUG_RENDER:
            {
                DebugRenderFunc func = (DebugRenderFunc)cmd.props[0].valI;
                void* userData = (void*)cmd.props[1].valI;
                func(cxt, userData );
            }
            break;
        default:
            break;
        }
    }

    /* 画面更新 */
    cxt->isUseVSync = true;
    d3dCxt->Flush();
    if( cxt->isUseVSync )
    { NL_HR_VALID( swapChain->Present( 1, 0) ); }
    else
    { NL_HR_VALID( swapChain->Present( 0, 0) ); }
}
/**/
static void renderScreenPoly( unsigned int psId, nlVertexShader& vs, nlEngineContext* cxt )
{
    /**/
    ID3D11Device* d3dDevice = cxt->d3dDevice;
    ID3D11DeviceContext* d3dCxt = cxt->d3dContext;
    /* 現在設定されているVS/GSを一時退避 */
    ID3D11VertexShader* origVS;
    ID3D11GeometryShader* origGS;
    d3dCxt->VSGetShader( &origVS, NULL, NULL );
    d3dCxt->GSGetShader( &origGS, NULL, NULL );
    /* psの設定 */
    //NL_ASSERT( psId < MAX_SHADER_NUM );
    if( psId != -1 )
    {
        ID3D11PixelShader* ps = cxt->pss[psId].shader_;
        NL_ASSERT( ps );
        d3dCxt->PSSetShader( ps, NULL, 0 );
    }
    /* 現在設定されているZ設定を一時退避 */
    ID3D11DepthStencilState* origDS;
    d3dCxt->OMGetDepthStencilState( &origDS, NULL );
    /* TODO 現在設定されているアルファ設定を一時退避 */
    /* 現在設定されているTopologyを一時退避 */
    D3D11_PRIMITIVE_TOPOLOGY origTopology;
    d3dCxt->IAGetPrimitiveTopology( &origTopology );
    /* Z設定を変更(読み取りなし) */
    d3dCxt->OMSetDepthStencilState(depthStencilStates[ZS_NotUse], 0);
    /* TODO ポリ描画用のVSに切り替え */
    d3dCxt->VSSetShader( vs.shader_, NULL, 0 );
	/* GSはリセットしておく */
	d3dCxt->GSSetShader(NULL,NULL,0);
    /* トポロジ変更 */
    d3dCxt->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
    /* TODO ポリゴン描画 */
    d3dCxt->Draw( 4, 0 );
    /* TOOD 現在設定されているアルファ設定を元に戻す */
    /* TOOD Z設定を元に戻す */
    d3dCxt->OMSetDepthStencilState( origDS, NULL );NL_SAFE_RELEASE(origDS);
    /* トポロジー設定を元に戻す */
    d3dCxt->IASetPrimitiveTopology( origTopology );
    /* VS/GSを元に戻す */
    d3dCxt->VSSetShader( origVS, NULL, 0 ); NL_SAFE_RELEASE(origVS);
    d3dCxt->GSSetShader( origGS, NULL, 0 ); NL_SAFE_RELEASE(origGS);
}
/**/
static void renderRenderObject( 
                               int meshId,
                               const nlMatrix& constVs0, 
                               const nlMatrix& constVs1, 
                               const nlMatrix& constPs0, 
                               const nlMatrix& constPs1, 
                               nlEngineContext& cxt )
{
    /* TODO 毎回float4x4を受け取らなくても良い。元になるnlVectorを受け取り、emtpryでない場合に限り渡すようにしても良い。 */
    /**/
    ID3D11Device* d3dDevice     = cxt.d3dDevice;
    ID3D11DeviceContext* d3dCxt = cxt.d3dContext;
    const nlMesh& mesh = cxt.models[meshId];
    /* 定数バッファに設定する */
    nlCopyToD3DBuffer( cbVsMatrix[0], (char*)&constVs0, sizeof(constVs0), cxt );
    nlCopyToD3DBuffer( cbVsMatrix[1], (char*)&constVs1, sizeof(constVs1), cxt );
    nlCopyToD3DBuffer( cbPsMatrix[0], (char*)&constPs0, sizeof(constPs0), cxt );
    nlCopyToD3DBuffer( cbPsMatrix[1], (char*)&constPs1, sizeof(constPs1), cxt );
    /**/
    ID3D11Buffer* pBufferTbl[] = { mesh.vretexes_ };
    UINT sizeTbl[] = { sizeof(NLMVertex) };
    UINT OffsetTbl[] = { 0 };
    cxt.d3dContext->IASetVertexBuffers(0, 1, pBufferTbl, sizeTbl, OffsetTbl);
    cxt.d3dContext->IASetIndexBuffer( mesh.indices_, DXGI_FORMAT_R32_UINT, 0);
    cxt.d3dContext->DrawIndexed( mesh.numIndex_, 0, 0);
}

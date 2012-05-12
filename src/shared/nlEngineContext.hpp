#pragma once

#include "nlTypedef.h"
#include "shared/container/nlArray.hpp"
#include "shared/curve/nlCurve.hpp"

/**
* @brief エンジンコンテキスト
*/
struct nlEngineContext
{
    /* D3D関連 */
    ID3D11Device*            d3dDevice;
    ID3D11DeviceContext*     d3dContext;
    IDXGIFactory*            dxgiFactory;
    IDXGISwapChain*          dxgiSwpChain;
    D3D11_VIEWPORT	         viewportDesc;
    D3D11_RASTERIZER_DESC    rasterDesc;
    D3D11_SAMPLER_DESC       samplerDesc;
    /* windowsリソース関連 */
    HWND                     hwnd;
    /* その他リソース */
    nlArray<nlMesh,MAX_MESH_NUM>  models;
    nlCurves paths;
    nlArray<nlVertexShader,MAX_SHADER_NUM> vss;
    nlArray<nlGeometoryShader,MAX_SHADER_NUM> gss;
    nlArray<nlPixelShader,MAX_SHADER_NUM> pss;
    //nlArray<nlTexture,MAX_TEXTURE_NUM> texs;
    nlArray<nlRenderTarget,MAX_RENDERTARGET_NUM> rendertargets;
    nlArray<nlDepthStencil,MAX_RENDERTARGET_NUM> depthStencils;
    ShaderScripts shaderScripts;
    nlSound* sound_;
    /* 全体設定関連 */
    bool isUseVSync;
};

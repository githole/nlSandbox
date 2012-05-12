#pragma once

#include "nlTypedef.h"
#include "shared/container/nlArray.hpp"
#include "shared/curve/nlCurve.hpp"

/**
* @brief �G���W���R���e�L�X�g
*/
struct nlEngineContext
{
    /* D3D�֘A */
    ID3D11Device*            d3dDevice;
    ID3D11DeviceContext*     d3dContext;
    IDXGIFactory*            dxgiFactory;
    IDXGISwapChain*          dxgiSwpChain;
    D3D11_VIEWPORT	         viewportDesc;
    D3D11_RASTERIZER_DESC    rasterDesc;
    D3D11_SAMPLER_DESC       samplerDesc;
    /* windows���\�[�X�֘A */
    HWND                     hwnd;
    /* ���̑����\�[�X */
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
    /* �S�̐ݒ�֘A */
    bool isUseVSync;
};

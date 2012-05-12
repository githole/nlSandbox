#include "main/stdafx.h"
#include "d3d11resource.hpp"
#include "shared/nlMemory.h"
#include "shared/math/nlMath.h"
#include "shared/nlEngineContext.hpp"
#include "shared/nlLog.hpp"

#ifndef INTROMODE
#include <iostream>
#include "tool/nlUtil.hpp"
#include "tool/nlLogComposer.hpp"
#endif

/**
 * @brief
 */
static const D3D10_SHADER_MACRO shaderMacros[] = 
{
    {"f1", "float1"},
    {"f2", "float2"},
    {"f3", "float3"},
    {"f4", "float4"},
    {"f4x4", "float4x4"},
    {"PI", "3.141592653589"},
    /* 短縮化したものは改行を入れることができないため改行を追加する含める */
    {"#include \"common\"", "#include \"common\"\n"},
    {}
};
#ifdef INTROMODE
extern void updateLoading();
#else
void updateLoading()
{ /* 何もしない */ }
#endif
///**
// * @brief 
// */
//class ShaderInclude 
//    : public ID3D10Include 
//{
//public:
//    ShaderInclude(
//        const ShaderScripts* shaderData )
//        :shaderData_( shaderData )
//    {
//        NL_ASSERT(shaderData);
//    }
//    /**/
//    bool findAndLoadShader( const ShaderScriptGroup& shaderGroup, LPCSTR fileName, LPCVOID* data, UINT* dataSize)
//    {
//        int i;
//        for( i=0;i<shaderGroup.numScript;++i)
//        {
//            if( !nlStrcmp( shaderGroup.scripts[i].name, fileName ) )
//            {
//                const nlInt8* script = shaderGroup.scripts[i].script;
//                *data = (LPCVOID*)script;
//                *dataSize = nlStrlen(script);
//                return true;
//            }
//        }
//        return false;
//    }
//    /**/
//    HRESULT __stdcall  Open(
//        D3D10_INCLUDE_TYPE includeType,
//        LPCSTR fileName,
//        LPCVOID parentData,
//        LPCVOID* data,
//        UINT* dataSize )
//    {
//        /**/
//        (void)includeType;
//        (void)parentData;
//        /**/
//        if( findAndLoadShader( shaderData_->commonScripts, fileName, data, dataSize ) )
//        {return	S_OK;}
//        /**/
//        NL_ASSERT( !"存在しないincludeを行った" );
//        return	S_OK;
//    }
//    /**/
//    HRESULT __stdcall Close( LPCVOID ppdata )
//    {
//        /* 常に成功 */
//        return  S_OK;
//    }
//private:
//    const ShaderScripts* shaderData_;
//};
/*----------------------------------------------------------------------------------------------------*/
// Dx11 Functions
/*----------------------------------------------------------------------------------------------------*/
/* Initialize DirectX11 Graphics */
void nlInitD3D11( HWND hWnd, bool isFullScreen, nlEngineContext& cxt )
{
    cxt.hwnd = hWnd;

    IDXGIAdapter* pAdapter		= NULL;
    IDXGIDevice1* pDXGI			= NULL;
    IDXGIFactory* pDXGIFactory	= NULL;
    D3D_FEATURE_LEVEL fl;
    /* FeatureLevelは10.1(SM4.0)で固定する */
    const D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_10_0;
    NL_HR_VALID( D3D11CreateDevice( NULL, D3D_DRIVER_TYPE_HARDWARE,(HMODULE)0,0,&featureLevel,1, D3D11_SDK_VERSION, &cxt.d3dDevice, &fl, &cxt.d3dContext ) );
    NL_HR_ASSSERT( fl == D3D_FEATURE_LEVEL_10_0 );
    /* create DXGI */
    NL_HR_VALID( cxt.d3dDevice->QueryInterface(__uuidof(IDXGIDevice1), (void**)&pDXGI) );
    /* get adapter info */
    NL_HR_VALID( pDXGI->GetAdapter(&pAdapter) );
    NL_HR_ASSSERT( pAdapter != NULL );
    /* get factory */
    NL_HR_VALID( pAdapter->GetParent( __uuidof(IDXGIFactory), (void**)&pDXGIFactory)  );
    NL_HR_ASSSERT( pDXGIFactory != NULL );
    {/* create swap chain */
        DXGI_SWAP_CHAIN_DESC swapChainDesc =
        {
            /* DXGI_MODE_DESC */
            {cxt.rendertargets[0].width_, cxt.rendertargets[0].height_, {60,1}, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED, DXGI_MODE_SCALING_UNSPECIFIED },
            /* DXGI_SAMPLE_DESC */
            {1, 0},
            /* BufferUsage */
            DXGI_USAGE_RENDER_TARGET_OUTPUT,
            /* BufferCount */
            3,
            /* OutputWindow */
            hWnd,
            /* Windowed */
            !isFullScreen,
            /* DXGI_SWAP_EFFECT */
            DXGI_SWAP_EFFECT_DISCARD,
            /* Flags */
            DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH,
        };
        NL_HR_VALID( pDXGIFactory->CreateSwapChain(cxt.d3dDevice, &swapChainDesc, &cxt.dxgiSwpChain) );
    }
    /* Disalbe "Alt+Enter" and so on. must call this after CreateSwapChain() call MakeWindowAssociation(). */
    NL_HR_VALID( pDXGIFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_WINDOW_CHANGES | DXGI_MWA_NO_ALT_ENTER | DXGI_MWA_NO_PRINT_SCREEN ));
    {/* get backbuffer view */
        D3D11_TEXTURE2D_DESC desc;
        cxt.rendertargets[0].renderTargetShaderResource_ = NULL;
        NL_HR_VALID( cxt.dxgiSwpChain->GetBuffer( 0, __uuidof(ID3D11Texture2D), (void**)(&cxt.rendertargets[0].renderTarget_) ) );
        NL_HR_VALID( cxt.d3dDevice->CreateRenderTargetView( cxt.rendertargets[0].renderTarget_, NULL, &cxt.rendertargets[0].renderTargetView_ ) );
        cxt.rendertargets[0].renderTarget_->GetDesc( &desc );
        cxt.rendertargets[0].width_  = desc.Width;
        cxt.rendertargets[0].height_ = desc.Height;
    }
    {/* create and set depthStencil */
        D3D11_TEXTURE2D_DESC depthDesc =
        {
            /*UINT Width;*/
            cxt.rendertargets[0].width_,
            /*UINT Height;*/
            cxt.rendertargets[0].height_,
            /*UINT MipLevels;*/
            1,
            /*UINT ArraySize;*/
            1,
            /*DXGI_FORMAT Format;*/
            DXGI_FORMAT_D24_UNORM_S8_UINT,
            /*DXGI_SAMPLE_DESC SampleDesc;*/
            {1,0},
            /*D3D11_USAGE Usage;*/
            D3D11_USAGE_DEFAULT,
            /*UINT BindFlags;*/
            D3D11_BIND_DEPTH_STENCIL,
            /*UINT CPUAccessFlags;*/
            0,
            /*UINT MiscFlags;*/
            0,
        };
        D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc =
        {
            /* DXGI_FORMAT Format; */
            DXGI_FORMAT_D24_UNORM_S8_UINT,
            /* D3D11_DSV_DIMENSION ViewDimension; */
            D3D11_DSV_DIMENSION_TEXTURE2DMS,
            /* UINT Flags; */
            0,
            /* D3D11_TEX2D_DSV */
            {0}
        };
        cxt.depthStencils[0].width  = depthDesc.Width;
        cxt.depthStencils[0].height = depthDesc.Height;
        NL_HR_VALID( cxt.d3dDevice->CreateTexture2D(&depthDesc, NULL, &cxt.depthStencils[0].tex ) );
        NL_HR_VALID( cxt.d3dDevice->CreateDepthStencilView(cxt.depthStencils[0].tex, &dsvDesc, &cxt.depthStencils[0].view) );
    }
    cxt.d3dContext->OMSetRenderTargets(1, &cxt.rendertargets[0].renderTargetView_, cxt.depthStencils[0].view );

    /* デフォルトの設定 */
    D3D11_VIEWPORT	viewportDesc =
    {
        /* FLOAT TopLeftX; */
        0.0f,
        /* FLOAT TopLeftY; */
        0.0f,
        /* FLOAT Width; */
        (FLOAT)cxt.rendertargets[0].width_,
        /* FLOAT Height; */
        (FLOAT)cxt.rendertargets[0].height_,
        /* FLOAT MinDepth; */
        0.0f,
        /* FLOAT MaxDepth; */
        1.0f,
    };
    cxt.viewportDesc = viewportDesc;
    nlUpdateViewPortSetting(cxt);    
    /**/
    D3D11_RASTERIZER_DESC rasterDesc =
    {
        /* D3D11_FILL_MODE FillMode; */
        D3D11_FILL_SOLID,
        /* D3D11_CULL_MODE CullMode; */
        D3D11_CULL_NONE,
        /* BOOL FrontCounterClockwise; */
        0,
        /* INT DepthBias; */
        0,
        /* FLOAT DepthBiasClamp; */
        0,
        /* FLOAT SlopeScaledDepthBias; */
        0,
        /* BOOL DepthClipEnable; */
        TRUE,
        /* BOOL ScissorEnable; */
        0,
        /* BOOL MultisampleEnable; */
        0,
        /* BOOL AntialiasedLineEnable; */
        0
    };
    cxt.rasterDesc = rasterDesc;
    nlUpdateRasterizeSetting(cxt);
    /**/
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc =
    {
        /* BOOL DepthEnable; */
        TRUE,
        /* D3D11_DEPTH_WRITE_MASK DepthWriteMask; */
        D3D11_DEPTH_WRITE_MASK_ALL,
        /* D3D11_COMPARISON_FUNC DepthFunc; */
        D3D11_COMPARISON_LESS,
        /* BOOL StencilEnable; */
        FALSE,
        /* UINT8 StencilReadMask; */
        0,
        /* UINT8 StencilWriteMask; */
        0,
        /* D3D11_DEPTH_STENCILOP_DESC FrontFace; ダミーデータであることに注意*/
        { D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_NEVER },
        /* D3D11_DEPTH_STENCILOP_DESC BackFace; */
        { D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_NEVER },
    };
    ID3D11DepthStencilState* depthStencilState;
    cxt.d3dDevice->CreateDepthStencilState(&depthStencilDesc, &depthStencilState );
    cxt.d3dContext->OMSetDepthStencilState(depthStencilState, 0);
    depthStencilState->Release();
    /**/
    D3D11_SAMPLER_DESC samplerDesc = 
    {
        /*D3D11_FILTER Filter;*/
        D3D11_FILTER_MIN_MAG_MIP_LINEAR,
        /* D3D11_TEXTURE_ADDRESS_MODE AddressU;*/
        D3D11_TEXTURE_ADDRESS_WRAP,
        /* D3D11_TEXTURE_ADDRESS_MODE AddressV;*/
        D3D11_TEXTURE_ADDRESS_WRAP,
        /* D3D11_TEXTURE_ADDRESS_MODE AddressW;*/
        D3D11_TEXTURE_ADDRESS_WRAP,
        /* FLOAT MipLODBias;*/
        0.0f,
        /* UINT MaxAnisotropy;*/
        1,
        /* D3D11_COMPARISON_FUNC ComparisonFunc;*/
        D3D11_COMPARISON_ALWAYS,
        /* FLOAT BorderColor[ 4 ];*/
        {0.0f,0.0f,0.0f,0.0f},
        /* FLOAT MinLOD;*/
        0,
        /* FLOAT MaxLOD;*/
        D3D11_FLOAT32_MAX,
    };
    cxt.samplerDesc = samplerDesc;
    /**/
    nlUpdateSamplerState(cxt);
}
/* Apply viewport setting */
void nlUpdateViewPortSetting(nlEngineContext& cxt)
{
    cxt.d3dContext->RSSetViewports(1, &cxt.viewportDesc );
}
/* Apply raster setting */
void nlUpdateRasterizeSetting(nlEngineContext& cxt)
{
    ID3D11RasterizerState* rasterizerState;
    cxt.d3dDevice->CreateRasterizerState(&cxt.rasterDesc, &rasterizerState);
    cxt.d3dContext->RSSetState(rasterizerState);
    rasterizerState->Release();
}
/* Apply smaplerstate setting */
void nlUpdateSamplerState(nlEngineContext&cxt)
{
    ID3D11SamplerState* samplerState;
    cxt.d3dDevice->CreateSamplerState(&cxt.samplerDesc, &samplerState);
    cxt.d3dContext->PSSetSamplers(0, 1, &samplerState);
    samplerState->Release();
}
/**/
ID3D11Buffer* nlCreateVertexBuffer(
                                   unsigned int size,
                                   nlEngineContext& cxt)
{
    D3D11_BUFFER_DESC desc =
    {
        /* UINT ByteWidth; */
        size,
        /* D3D11_USAGE Usage; */
        D3D11_USAGE_DYNAMIC,
        /* UINT BindFlags; */
        D3D11_BIND_VERTEX_BUFFER,
        /* UINT CPUAccessFlags; */
        D3D11_CPU_ACCESS_WRITE,
        /* UINT MiscFlags; */
        0,
        /* UINT StructureByteStride; */
        0
    };
    D3D11_SUBRESOURCE_DATA* pSubResource = NULL;
    ID3D11Buffer* pBuffer;
    NL_HR_VALID( cxt.d3dDevice->CreateBuffer(&desc, pSubResource, &pBuffer) );
    return pBuffer;
}
/**/
ID3D11Buffer* nlCreateIndexBuffer(
                                  unsigned int size,
                                  nlEngineContext& cxt )
{
    D3D11_BUFFER_DESC desc =
    {
        /* UINT ByteWidth; */
        size,
        /* D3D11_USAGE Usage; */
        D3D11_USAGE_DYNAMIC,
        /* UINT BindFlags; */
        D3D11_BIND_INDEX_BUFFER,
        /* UINT CPUAccessFlags; */
        D3D11_CPU_ACCESS_WRITE,
        /* UINT MiscFlags; */
        0,
        /* UINT StructureByteStride; */
        0
    };
    D3D11_SUBRESOURCE_DATA* pSubResource = NULL;
    ID3D11Buffer* pBuffer;
    NL_HR_VALID( cxt.d3dDevice->CreateBuffer(&desc, pSubResource, &pBuffer) );
    return pBuffer;
}
/**/
ID3D11Buffer* nlCreateConstBuffer( 
                                  unsigned int size,
                                  nlEngineContext& cxt )
{
    D3D11_BUFFER_DESC desc =
    {
        /* UINT ByteWidth; */
        size,
        /* D3D11_USAGE Usage; */
        D3D11_USAGE_DYNAMIC,
        /* UINT BindFlags; */
        D3D11_BIND_CONSTANT_BUFFER,
        /* UINT CPUAccessFlags; */
        D3D11_CPU_ACCESS_WRITE,
        /* UINT MiscFlags; */
        0,
        /* UINT StructureByteStride; */
        0
    };
    D3D11_SUBRESOURCE_DATA* pSubResource = NULL;
    ID3D11Buffer* pBuffer;
    NL_HR_VALID( cxt.d3dDevice->CreateBuffer(&desc, pSubResource, &pBuffer) );
    return pBuffer;
}
/**/
void nlCreateRenderTarget( 
                           const RenderTargetDescs& descs, 
                           nlEngineContext* cxt )
{
    const unsigned int numRT = descs.numRT;
    /* 0番はBackbuffer用のため1だけずれていることに注意 */
    for( int i=1;i<numRT+1;++i)
    {
        const RenderTargetDesc& desc = descs.rtDescs[i];
        const int scaleShift = desc.scaleByBackbuffer;
        const int w = (cxt->rendertargets[0].width_)>>scaleShift;
        const int h = (cxt->rendertargets[0].height_)>>scaleShift;
        /* 生成 */
        nlRenderTarget& renderTarget = cxt->rendertargets[i];
        /**/
        renderTarget.width_  = w;
        renderTarget.height_ = h;
        /**/
        D3D11_TEXTURE2D_DESC descTarget = 
        {
            w, h, 1, 1, desc.format,
            {1,0},D3D11_USAGE_DEFAULT,
            (D3D11_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE),
            0, 0
        };    
        NL_HR_VALID( cxt->d3dDevice->CreateTexture2D( &descTarget, NULL, &renderTarget.renderTarget_ ) );
        ID3D11Texture2D* rt =renderTarget.renderTarget_;
        NL_HR_VALID( cxt->d3dDevice->CreateRenderTargetView( rt, NULL, &renderTarget.renderTargetView_ ) );
        NL_HR_VALID( cxt->d3dDevice->CreateShaderResourceView( rt, NULL ,&renderTarget.renderTargetShaderResource_ ) );
    }
    /**/
    /* 0番はBackbuffer用のため1だけずれていることに注意 */
    for( int i=1;i<descs.numDS+1;++i)
    {
        /**/
        const int scaleShift = descs.dsDescs[i].scaleByBackbuffer;
        const int w = (cxt->rendertargets[0].width_)>>scaleShift;
        const int h = (cxt->rendertargets[0].height_)>>scaleShift;
        const DXGI_FORMAT format = descs.dsDescs[i].format;
        /**/
        D3D11_TEXTURE2D_DESC depthDesc =
        {
            w, h, 1, 1, format,
            {1,0}, D3D11_USAGE_DEFAULT,
            D3D11_BIND_DEPTH_STENCIL,
            0, 0
        };
        D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc =
        {
            format,
            D3D11_DSV_DIMENSION_TEXTURE2DMS,
            0,{0}
        };
        NL_HR_VALID( cxt->d3dDevice->CreateTexture2D(&depthDesc, NULL, &cxt->depthStencils[i].tex) );
        NL_HR_VALID( cxt->d3dDevice->CreateDepthStencilView(cxt->depthStencils[i].tex, &dsvDesc, &cxt->depthStencils[i].view ) );
        cxt->depthStencils[i].width  = w;
        cxt->depthStencils[i].height = h;
    }
}
/* @NOTE この関数は近い将来撤去されます */
nlVertexShader nlCreateVertexShader(
                                    const nlInt8* script, 
                                    unsigned int scriptSize, 
                                    const nlInt8* funcName,
                                    nlEngineContext& cxt )
{
    struct GOD_VERTEX
    {
        nlVec4    pos_;
        nlVec4    normal_;
        nlVec4    uv_;
        D3DCOLOR  color_;
    };

    ID3DBlob* pBlob = NULL;
    ID3DBlob* pErrorBlob = NULL;
    nlVertexShader vertexShader;
    vertexShader.inputLayout_ = NULL;
    vertexShader.shader_ = NULL;

    
#ifdef INTROMODE
    const DWORD flag = D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_PACK_MATRIX_ROW_MAJOR | D3D10_SHADER_OPTIMIZATION_LEVEL3;
    HRESULT hr = D3D10CompileShader( script, scriptSize, funcName, shaderMacros, NULL, funcName, "vs_4_0", flag, &pBlob, &pErrorBlob );
    if(FAILED( hr ) )
    {
        MessageBoxA(NULL,(nlInt8*)pErrorBlob->GetBufferPointer(),"",MB_OK);
    }
#else 
    const DWORD flag = D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_PACK_MATRIX_ROW_MAJOR;
    HRESULT hr = D3D10CompileShader( script, scriptSize, funcName, shaderMacros, NULL, funcName, "vs_4_0", flag, &pBlob, &pErrorBlob );
#endif

#ifndef INTROMODE
    if( FAILED(hr) )
    {
        std::string error = std::string("[")+std::string(funcName)+std::string("]")+std::string(DXGetErrorDescriptionA(hr));
        if(pErrorBlob)
        {
            error += std::string( (nlInt8*)pErrorBlob->GetBufferPointer() ); 
            error.resize(error.size()-1);/* 改行コードを取り除く */
        }
        NL_ERR( ERR_005, error.c_str() );
        /* ファイルに書き出す */
        QString fileName;
        QTime now = QDateTime::currentDateTime().time();
        fileName.sprintf("err_%s_%d_%d_d.log",funcName,now.hour(),now.minute(),now.second() );
        QString path = sandboxPath(SP_APP)+fileName;
        QFile dataFile(path);
        dataFile.open(QIODevice::WriteOnly|QIODevice::Text);
        dataFile.write( script );
        /**/
        return vertexShader;
    }
#endif
    /* create shader */
    NL_HR_VALID( cxt.d3dDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &vertexShader.shader_ ) );
    /* create inputlayout */
    NL_HR_VALID( cxt.d3dDevice->CreateInputLayout(NLM_INPUT_ELEMENT, _countof(NLM_INPUT_ELEMENT), pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &vertexShader.inputLayout_  ) );
    /**/
    return vertexShader;
}
/**/
nlGeometoryShader nlCreateGeometoryShader( const nlInt8* script, unsigned int scriptSize, const nlInt8* funcName, nlEngineContext& cxt )
{
    ID3DBlob* pBlob = NULL;
    ID3DBlob* pErrorBlob = NULL;
    nlGeometoryShader shader;
    shader.shader = NULL;
    
#ifdef INTROMODE
    const DWORD flag = D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_PACK_MATRIX_ROW_MAJOR | D3D10_SHADER_OPTIMIZATION_LEVEL3;
    HRESULT hr = D3D10CompileShader( script, scriptSize, funcName, shaderMacros, NULL, funcName, "gs_4_0", flag, &pBlob, &pErrorBlob );
#else
    const DWORD flag = D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_PACK_MATRIX_ROW_MAJOR;
    HRESULT hr = D3D10CompileShader( script, scriptSize, funcName, shaderMacros, NULL, funcName, "gs_4_0", flag, &pBlob, &pErrorBlob );
#endif
        
#ifndef INTROMODE
    if( FAILED(hr) )
    {
        std::string error = std::string("[")+std::string(funcName)+std::string("]")+std::string(DXGetErrorDescriptionA(hr));
        error.resize(error.size()-1);/* 改行コードを取り除く */
        if(pErrorBlob)
        {
            error += std::string( (nlInt8*)pErrorBlob->GetBufferPointer() );
            error.resize(error.size()-1);/* 改行コードを取り除く */
        }
        NL_ERR( ERR_026, error.c_str() );
        /* ファイルに書き出す */
        QString fileName;
        QTime now = QDateTime::currentDateTime().time();
        fileName.sprintf("err_%s_%d_%d_d.log",funcName,now.hour(),now.minute(),now.second() );
        QString path = sandboxPath(SP_APP)+fileName;
        QFile dataFile(path);
        dataFile.open(QIODevice::WriteOnly|QIODevice::Text);
        dataFile.write( script );
        /**/
        shader.shader = NULL;
        return shader;
    }
#endif
    NL_HR_VALID( cxt.d3dDevice->CreateGeometryShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &shader.shader ) );

    return shader;
}
/* 全てのシェーダを作成する */
void nlCreateShaders(
                     ShaderType shaderType,
                     int targetIndex,
                     const ShaderScripts& shaderScript, 
                     nlEngineContext& cxt )
{
    /**/
    class Local
    {
    public:
        /**/
        static void createVS( 
            unsigned int target,
            const ShaderScripts& ss, 
            nlEngineContext& cxt )
        {
            const ShaderScript& shaderScript = ss.vsScripts.scripts[target];
            const nlInt8* script   = shaderScript.script;
            const nlInt8* funcName = "main";
            const nlInt8* unitName = shaderScript.name;
            nlVertexShader& vs = cxt.vss[target];
            NL_ASSERT( !vs.shader_ );
            /**/
            ID3DBlob* pBlob = NULL;
            ID3DBlob* pErrorBlob = NULL;
            /* commonと連結 */
            const nlInt8* commonScript = ss.commonScripts.script;
            const nlUint32 scriptLen = nlStrlen( script ) + nlStrlen( commonScript );
            nlInt8* conbinedScript = (nlInt8*)nlMalloc( (scriptLen+1)*sizeof(nlInt8) );
            nlStrcat( conbinedScript, commonScript );
            nlStrcat( conbinedScript, script );

            //printf( conbinedScript );

#ifndef INTROMODE
            const DWORD flag = D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_PACK_MATRIX_ROW_MAJOR;
            HRESULT hr = D3D10CompileShader( conbinedScript, nlStrlen(conbinedScript), unitName, shaderMacros, NULL, funcName, "vs_4_0", flag, &pBlob, &pErrorBlob );
            /* ロードが失敗したらエラーを出力した後に確実にロードが成功するシェーダをロードする */
            if(FAILED( hr ) )
            {
                /**/
                NL_ERR( ERR_005, (nlInt8*)pErrorBlob->GetBufferPointer() );
                /**/
                const nlInt8 script[] =
                {
                    "cbuffer Const0 : register(b0){ f4x4 world; }"
                    "cbuffer Const2 : register(b2){ f4x4 viewProj; }"
                    "float4 main( float4 in_pos:P, float4 in_normal:N, float4 in_col:C ):SV_POSITION"
                    "{ return mul( in_pos, mul(world,viewProj)); }"
                };
                HRESULT hr = D3D10CompileShader( script, sizeof(script)/sizeof(nlInt8), unitName, shaderMacros, NULL, "main", "vs_4_0", flag, &pBlob, &pErrorBlob );
            }
#else
            const DWORD flag = D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_PACK_MATRIX_ROW_MAJOR | D3D10_SHADER_OPTIMIZATION_LEVEL3;
            HRESULT hr = D3DX11CompileFromMemory( conbinedScript, nlStrlen(conbinedScript), funcName, shaderMacros, NULL, funcName, "vs_4_0", flag, 0, NULL, &pBlob, &pErrorBlob, NULL );
            if(FAILED( hr ) )
            {
                MessageBoxA(NULL,(nlInt8*)pErrorBlob->GetBufferPointer(),"",MB_OK);
            }
#endif
            /**/
            nlFree( (void*)conbinedScript );
            /* create shader */
            NL_HR_VALID( cxt.d3dDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &vs.shader_ ) );
            /* create inputlayout */
            NL_HR_VALID( cxt.d3dDevice->CreateInputLayout(NLM_INPUT_ELEMENT, _countof(NLM_INPUT_ELEMENT), pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &vs.inputLayout_  ) );
        }
        /* GSの生成 */
        static void createGS(
            nlUint32 target,
            const ShaderScripts& ss,
            nlEngineContext& cxt )
        {
           /**/
            ID3DBlob* pBlob = NULL;
            ID3DBlob* pErrorBlob = NULL;
            nlGeometoryShader& geometoryShader = cxt.gss[target];
            const nlInt8* script = ss.gsScripts.scripts[target].script;
            const nlInt8* funcName = "main";
            /**/
            NL_ASSERT(!geometoryShader.shader);
            /* commonと連結 */
            const nlInt8* commonScript = ss.commonScripts.script;
            const nlUint32 scriptLen = nlStrlen( script ) + nlStrlen( commonScript );
            nlInt8* conbinedScript = (nlInt8*)nlMalloc( (scriptLen+1)*sizeof(nlInt8) );
            nlStrcat( conbinedScript, commonScript );
            nlStrcat( conbinedScript, script );

#ifndef INTROMODE
            const DWORD flag = D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_PACK_MATRIX_ROW_MAJOR;
			HRESULT hr = D3D10CompileShader(
				conbinedScript, nlStrlen(conbinedScript), 
				funcName, shaderMacros, NULL, 
				funcName, "gs_4_0", flag, &pBlob, &pErrorBlob);
			
            if( SUCCEEDED(hr) )
            {
                if( SUCCEEDED( cxt.d3dDevice->CreateGeometryShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &geometoryShader.shader ) ) )
                { return ;}
                else
                {
                    NL_ASSERT(!"このパスには来ないことになっている");
                    //goto RELOAD;
                    return ;
                }
            }
            else
            {
                /* 失敗した場合はエラーを出力し確実に成功するシェーダをロードする */
                const nlInt8* name = ss.gsScripts.scripts[target].name;
                std::string error = std::string("[")+std::string(name)+std::string("]")+std::string(DXGetErrorDescriptionA(hr));
                error.resize(error.size()-1);/* 改行コードを取り除く */
                if(pErrorBlob)
                {
                    error += std::string( (nlInt8*)pErrorBlob->GetBufferPointer() );
                    error.resize(error.size()-1);/* 改行コードを取り除く */
                }
                NL_ERR( ERR_006, error.c_str());
                /**/
                const nlInt8 script[] = 
                {
                    "struct GSSceneIn\n"
                    "{\n"
                    "   float4 Pos	: POS;\n"
	                "   float3 Norm : NORMAL;\n"
	                "   float2 Tex	: TEXCOORD0;\n"
                    "};\n"
                    "struct PSSceneIn\n"
                    "{\n"
                    "	float4 Pos  : SV_Position;\n"
                    "   float3 Norm : TEXCOORD0;\n"
                    "   float2 Tex  : TEXCOORD1;\n"
                    "};\n"
                    "[maxvertexcount(3)]\n"
                    "void GSScene( triangleadj GSSceneIn input[6], inout TriangleStream<PSSceneIn> OutputStream )\n"
                    "{\n"
                    "   PSSceneIn output = (PSSceneIn)0;\n"
                    "   for( uint i=0; i<6; i+=2 )\n"
                    "   {\n"
                    "       output.Pos = input[i].Pos;\n"
                    "       output.Norm = input[i].Norm;\n"
                    "       output.Tex = input[i].Tex;\n"
                    "       OutputStream.Append( output );\n"
                    "   }\n"
                    "   OutputStream.RestartStrip();\n"
                    "}\n"
                };
                geometoryShader = nlCreateGeometoryShader(script, sizeof(script)/sizeof(nlInt8), "main", cxt );
            }
            return;

#else
            const DWORD flag = D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_PACK_MATRIX_ROW_MAJOR | D3D10_SHADER_OPTIMIZATION_LEVEL3;
            HRESULT hr = D3D10CompileShader( conbinedScript, nlStrlen(conbinedScript), funcName, shaderMacros, NULL, funcName, "gs_4_0", flag, &pBlob, &pErrorBlob );
            if( FAILED(hr) )
            {
                MessageBox( NULL, "failed to load geometory shader", "", MB_OK );
                if(pErrorBlob)
                {
                    MessageBox( NULL, (nlInt8*)pErrorBlob->GetBufferPointer(), "", MB_OK );
                }
                return;
            }
            NL_HR_VALID( cxt.d3dDevice->CreateGeometryShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &geometoryShader.shader ) );
#endif
        }
        /* PSの生成 */
        static void createPS(
            unsigned int target,
            const ShaderScripts& ss,
            nlEngineContext& cxt )
        {
            /**/
            ID3DBlob* pBlob = NULL;
            ID3DBlob* pErrorBlob = NULL;
            nlPixelShader& pixelShader = cxt.pss[target];
            const nlInt8* script = ss.psScripts.scripts[target].script;
            const nlInt8* funcName = "main";

            /**/
            NL_ASSERT(!pixelShader.shader_);

            /* commonと連結 */
            const nlInt8* commonScript = ss.commonScripts.script;
            const unsigned int scriptLen = nlStrlen( script ) + nlStrlen( commonScript );
            nlInt8* conbinedScript = (nlInt8*)nlMalloc( (scriptLen+1)*sizeof(nlInt8) );
            nlStrcat( conbinedScript, commonScript );
            nlStrcat( conbinedScript, script );

#ifndef INTROMODE
            const DWORD flag = D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_PACK_MATRIX_ROW_MAJOR;
            HRESULT hr = D3D10CompileShader( conbinedScript, nlStrlen(conbinedScript), funcName, shaderMacros, NULL, funcName, "ps_4_0", flag, &pBlob, &pErrorBlob );
            if( SUCCEEDED(hr) )
            {
                if( SUCCEEDED( cxt.d3dDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &pixelShader.shader_ ) ) )
                { return ;}
                else
                {
                    NL_ASSERT(!"このパスには来ないことになっている");
                    //goto RELOAD;
                    return ;
                }
            }
            else
            {
                /* 失敗した場合はエラーを出力し確実に成功するシェーダをロードする */
                const nlInt8* name = ss.psScripts.scripts[target].name;
                std::string error = std::string("[")+std::string(name)+std::string("]")+std::string(DXGetErrorDescriptionA(hr));
                error.resize(error.size()-1);/* 改行コードを取り除く */
                if(pErrorBlob)
                { 
                    error += std::string( (nlInt8*)pErrorBlob->GetBufferPointer() );
                    error.resize(error.size()-1);/* 改行コードを取り除く */
                }
                NL_ERR( ERR_006, error.c_str());
                /**/
                const nlInt8 script[] = 
                {
                    "float4 main():SV_Target0"
                    "{return float4(1,0,0,1);}"
                };
                pixelShader = nlCreatePixelShader(script, sizeof(script)/sizeof(nlInt8), "main", cxt );
            }
            return;

#else
            const DWORD flag = D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_PACK_MATRIX_ROW_MAJOR | D3D10_SHADER_OPTIMIZATION_LEVEL3;
            HRESULT hr = D3D10CompileShader( conbinedScript, nlStrlen(conbinedScript), funcName, shaderMacros, NULL, funcName, "ps_4_0", flag, &pBlob, &pErrorBlob );
            if( FAILED(hr) )
            {
                MessageBox( NULL, "failed to load pixel shader", "", MB_OK );
                if(pErrorBlob)
                {
                    MessageBox( NULL, (nlInt8*)pErrorBlob->GetBufferPointer(), "", MB_OK );
                }
                return;
            }
            NL_HR_VALID( cxt.d3dDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &pixelShader.shader_ ) );
#endif
        }
        /* 全VSの生成 */
        static void LoadAllVS( const ShaderScripts& shaderScripts, nlEngineContext& cxt  )
        {
            const ShaderScriptGroup& vsScripts = shaderScripts.vsScripts;
            const unsigned int numScript = vsScripts.numScript;
            for( int i=0;i<numScript;++i)
            {
                createVS( i, shaderScripts, cxt );
                updateLoading();
            }
        }
        /* 全GSの生成 */
        static void LoadAllGS( const ShaderScripts& shaderScripts, nlEngineContext& cxt  )
        {
            const ShaderScriptGroup& gsScripts = shaderScripts.gsScripts;
            const unsigned int numScript = gsScripts.numScript;
            for( int i=0;i<numScript;++i)
            {
                createGS( i, shaderScripts, cxt );
                updateLoading();
            }
        }
        /* 全PSの生成 */
        static void LoadAllPS( const ShaderScripts& shaderScripts, nlEngineContext& cxt  )
        {
            const ShaderScriptGroup& psScripts = shaderScripts.psScripts;
            const unsigned int numScript = psScripts.numScript;
            for( int i=0;i<numScript;++i)
            {
                createPS( i, shaderScripts, cxt );
                updateLoading();
            }
        }
    };
    
    /* 全体をロードする場合 */
    if( shaderType == ShaderType_NONE )
    {
        /* VS全体をロードする */
        Local::LoadAllVS(shaderScript,cxt);
        /* GS全体をロードする */
        Local::LoadAllGS(shaderScript,cxt);
        /* PS全体をロードする */
        Local::LoadAllPS(shaderScript,cxt);
        /* TODO TS全体をロードする */
        /* TODO CS全体をロードする */
    }
    /* 特定の種類のシェーダを読み込む */
    else if( targetIndex == -1 )
    {
        switch( shaderType )
        {
        case ShaderType_VS:
            Local::LoadAllVS(shaderScript,cxt);
            break;
        case ShaderType_GS:
            Local::LoadAllGS(shaderScript,cxt);
            break;
        case ShaderType_PS:
            Local::LoadAllPS(shaderScript,cxt);
            break;
        case ShaderType_TS:
            break;
        case ShaderType_CS:
            break;
        case ShaderType_NONE:
        default:
            NL_ASSERT(!"存在しないシェーダタイプ");
            break;
        }
    }
    /* ピンポイントでロードする */
    else
    {
        switch( shaderType )
        {
        case ShaderType_VS:
            Local::createVS( targetIndex, shaderScript, cxt );
            break;
        case ShaderType_GS:
            Local::createGS( targetIndex, shaderScript, cxt );
            break;
        case ShaderType_PS:
            Local::createPS( targetIndex, shaderScript, cxt );
            break;
        case ShaderType_TS:
            /* TOOD 実装 */
            break;
        case ShaderType_CS:
            /* TOOD 実装 */
            break;
        case ShaderType_NONE:
            /* TOOD 実装 */
        default:
            NL_ASSERT(!"存在しないシェーダタイプ");
            break;
        }
    }
}
/* @NOTE この関数は近い将来撤去されます */
nlPixelShader nlCreatePixelShader( 
                                  const nlInt8* script, 
                                  unsigned int scriptSize, 
                                  const nlInt8* funcName,
                                  nlEngineContext& cxt )
{
    ID3DBlob* pBlob = NULL;
    ID3DBlob* pErrorBlob = NULL;
    nlPixelShader pixelShader;
    pixelShader.shader_ = NULL;
#ifndef INTROMODE
    const DWORD flag = D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_PACK_MATRIX_ROW_MAJOR;
    HRESULT hr = D3D10CompileShader( script, scriptSize, funcName, shaderMacros, NULL, funcName, "ps_4_0", flag, &pBlob, &pErrorBlob );
    if( FAILED(hr) )
    {
        std::string error = std::string("[")+std::string(funcName)+std::string("]")+std::string(DXGetErrorDescriptionA(hr));
        error.resize(error.size()-1);/* 改行コードを取り除く */
        if(pErrorBlob)
        { 
            error += std::string( (nlInt8*)pErrorBlob->GetBufferPointer() );
            error.resize(error.size()-1);/* 改行コードを取り除く */
        }
        NL_ERR( ERR_018, error.c_str() );
        /* ファイルに書き出す */
        QString fileName;
        QTime now = QDateTime::currentDateTime().time();
        fileName.sprintf("err_%s_%d_%d_d.log",funcName,now.hour(),now.minute(),now.second() );
        QString path = sandboxPath(SP_APP)+fileName;
        QFile dataFile(path);
        dataFile.open(QIODevice::WriteOnly|QIODevice::Text);
        dataFile.write( script );
        /**/
        pixelShader.shader_ = NULL;
        return pixelShader;
    }
#else
    const DWORD flag = D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_PACK_MATRIX_ROW_MAJOR|D3D10_SHADER_OPTIMIZATION_LEVEL3;
    HRESULT hr = D3D10CompileShader( script, scriptSize, funcName, shaderMacros, NULL, funcName, "ps_4_0", flag, &pBlob, &pErrorBlob );
    if( FAILED(hr) )
    {
        MessageBox( NULL, (nlInt8*)pErrorBlob->GetBufferPointer(), "", MB_OK );
    }
#endif
    NL_HR_VALID( cxt.d3dDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &pixelShader.shader_ ) );

    return pixelShader;
}
#if 0
/**/
void ntGenerateTextures(const ShaderScripts& shaderScript, 
                        unsigned int index,
                        nlEngineContext* cxt )
{
    /**/
    ID3D11Device* d3dDevice = cxt->d3dDevice;
    ID3D11DeviceContext* d3dContext = cxt->d3dContext;
    /* シェーダスクリプトの作成 */
    const nlInt8* commonScript = shaderScript.commonScripts.script;
    const nlInt8* tsScript = shaderScript.tsScripts.scripts[index].script;
    nlInt8* combinedScript = (nlInt8*)nlMalloc( nlStrlen(commonScript)+nlStrlen(tsScript)+1);
    nlStrcat( combinedScript, commonScript );
    nlStrcat(combinedScript, tsScript );
    /* シェーダの作成 */
    nlPixelShader texShader = nlCreatePixelShader( combinedScript, nlStrlen( combinedScript ), "main", *cxt );
    nlFree( combinedScript );

    /* 書きこむのに必要なシェーダを作成 */
    nlVertexShader screenVS = nlCreateVertexShader( screenVSScript, nlStrlen( screenVSScript), "vs", *cxt );

    /* テクスチャを作成 */
    nlTexture& tex = cxt->texs[index];
    NL_SAFE_RELEASE( tex.texture );
    NL_SAFE_RELEASE( tex.rtView );
    NL_SAFE_RELEASE( tex.shaderView );
    tex.width  = 512; /* HACK サイズは固定 */
    tex.height = 512; /* HACK サイズは固定 */
    int mipLevel = 1;
    int CPUAccessFlags = 0;
    D3D11_TEXTURE2D_DESC descTarget = 
    {
        tex.width, tex.height,
        mipLevel, 1,
        DXGI_FORMAT_B8G8R8A8_UNORM,
        {1,0}, D3D11_USAGE_DEFAULT,
        (D3D11_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE),
        CPUAccessFlags, 0
    };
    d3dDevice->CreateTexture2D( &descTarget, NULL, &tex.texture );
    d3dDevice->CreateRenderTargetView( tex.texture, NULL, &tex.rtView );
    d3dDevice->CreateShaderResourceView( tex.texture, NULL ,&tex.shaderView );

    /* 現在のRasterStateを取得 */
    ID3D11RasterizerState* oldRasterState;
    d3dContext->RSGetState(&oldRasterState);
    D3D11_RASTERIZER_DESC rasterDesc;
    rasterDesc.AntialiasedLineEnable = false;
    rasterDesc.CullMode = D3D11_CULL_NONE;
    rasterDesc.DepthBias = 0;
    rasterDesc.DepthBiasClamp = 0.0f;
    rasterDesc.DepthClipEnable = true;
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.FrontCounterClockwise = false;
    rasterDesc.MultisampleEnable = false;
    rasterDesc.ScissorEnable = false;
    rasterDesc.SlopeScaledDepthBias = 0.0f;
    ID3D11RasterizerState* rasterState;
    d3dDevice->CreateRasterizerState(&rasterDesc, &rasterState);
    d3dContext->RSSetState(rasterState);
    rasterState->Release();

    /* ビューポートの設定 */
    D3D11_VIEWPORT viewport;
    viewport.Width = tex.width;
    viewport.Height = tex.height;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    d3dContext->RSSetViewports(1, &viewport);
    /* シェーダを設定する */
    d3dContext->VSSetShader( screenVS.shader_, NULL, 0);
    d3dContext->PSSetShader( texShader.shader_, NULL, 0);
    /* レンダーターゲットを設定する */
    const float clearColor[4] = {0.0f, 0.0f, 1.0f, 0.0f};
    d3dContext->ClearRenderTargetView( tex.rtView, clearColor );
    d3dContext->OMSetRenderTargets( 1, &tex.rtView, NULL );

    /* 全てのテクスチャを設定する */
    /* TODO 毎回全てのテクスチャを設定せずに、このループで設定したテクスチャを省くようにだけする？*/
    for( int i=0;i<((int)index)-1;++i)
    {
        nlTexture& texSamp = cxt->texs[i];
        d3dContext->PSSetShaderResources(i, 1, &texSamp.shaderView );
    }

    /* 描画する */
    d3dContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
    d3dContext->Draw( 4, 0 );
    /**/
    d3dContext->VSSetShader( NULL, NULL, 0);
    d3dContext->PSSetShader( NULL, NULL, 0);

    /* ビューポート設定を元に戻す */
    /* 現在のビューポート設定を取得しておく */
    D3D11_VIEWPORT oldViewports = {0.0,0.0,cxt->rendertargets[0].width_,cxt->rendertargets[0].height_,0.0f,1.0f};
    d3dContext->RSSetViewports( 1, &oldViewports );

    /* RasterState設定を元に戻す */
    d3dContext->RSSetState(oldRasterState);
    if( oldRasterState ){ oldRasterState->Release(); }

    /* psの解放 */
    NL_SAFE_RELEASE( screenVS.inputLayout_ );
    NL_SAFE_RELEASE( screenVS.shader_ );
    NL_SAFE_RELEASE( texShader.shader_ );
}
#endif
/**/
void ntGenerateTextures(const ShaderScripts& shaderScript, 
                        nlEngineContext* cxt )
{
#if 0
    /* この関数が呼ばれる前にテクスチャは解放されている事が期待される */
    for( int i=0;i<MAX_TEXTURE_NUM;++i){ NL_ASSERT( !cxt->texs[i].texture );}
    const ShaderScriptGroup& tsScripts = shaderScript.tsScripts;
    const ShaderScript& commonScrips = shaderScript.commonScripts;
    /**/
    NL_ASSERT( 0 < tsScripts.numScript );
    /**/
    ID3D11Device* d3dDevice = cxt->d3dDevice;
    ID3D11DeviceContext* d3dContext = cxt->d3dContext;

    tsScripts.numScript;
    tsScripts.scripts[0].script;
    tsScripts.scripts[0].name;

    //const nlInt8* script = tsScripts.scripts;
    nlTexture* texuters = &(cxt->texs[0]);
    /**/
    const unsigned int scriptLen = nlStrlen(script);
    /* 全シェーダを作成 */
    ID3D11PixelShader* texturShaders[MAX_TEXTURE_NUM];
    nlMemset( texturShaders, 0, sizeof(ID3D11PixelShader*)*MAX_TEXTURE_NUM );
    /**/
    const unsigned int numTexture = textureScripts.numTexture;
    /**/
    for( unsigned int i=0;i<numTexture;++i)
    {
        ID3DBlob* pBlob = NULL;
        ID3DBlob* pErrorBlob = NULL;
        const nlInt8* funcName = textureScripts.funcs[i];
        const DWORD flag = D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_PACK_MATRIX_ROW_MAJOR;
        /* ピクセルシェーダの作成 */
        if( FAILED(
            D3D10CompileShader(
            script, scriptLen, funcName, shaderMacros, NULL, 
           "main", "ps_4_0",flag, &pBlob, &pErrorBlob ) ) )
        {
            NL_ERR(ERR_009);
            MessageBoxA( NULL, "load texshader error", "", MB_OK );
            if(pErrorBlob)
            {
                NL_ERR(ERR_003, pErrorBlob->GetBufferPointer() );
                MessageBoxA( NULL, (nlInt8*)pErrorBlob->GetBufferPointer(), "", MB_OK );
            }
            return;
        }
        if( pBlob )
        {
            d3dDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &texturShaders[i] );
        }
        ++i;
    }

    /* 書きこむのに必要なシェーダを作成 */
    ID3D11VertexShader* vsRenderQuad;
    ID3DBlob* pBlob = NULL;
    ID3DBlob* pErrorBlob = NULL;    

    NL_HR_ASSSERT( D3D10CompileShader(script, scriptLen, "vs", shaderMacros, NULL, 
        "main","vs_4_0",(D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_PACK_MATRIX_ROW_MAJOR),
        &pBlob, &pErrorBlob ) ) ;
    d3dDevice->CreateVertexShader( pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &vsRenderQuad );

    /* 全テクスチャを作成 */
    for(unsigned int i=0;i<numTexture;++i)
    {
        /**/
        nlTexture& tex = texuters[i];
        /**/
        tex.width  = 512; /* HACK サイズは適当 */
        tex.height = 512; /* HACK サイズは適当 */
        int mipLevel = 1;
        int CPUAccessFlags = 0;
        D3D11_TEXTURE2D_DESC descTarget = 
        {
            tex.width, tex.height,
            mipLevel, 1,
            DXGI_FORMAT_B8G8R8A8_UNORM,
            {1,0}, D3D11_USAGE_DEFAULT,
            (D3D11_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE),
            CPUAccessFlags, 0
        };
        d3dDevice->CreateTexture2D( &descTarget, NULL, &tex.texture );
        d3dDevice->CreateRenderTargetView( tex.texture, NULL, &tex.rtView );
        d3dDevice->CreateShaderResourceView( tex.texture, NULL ,&tex.shaderView );
    }

    /* 現在のRasterStateを取得 */
    ID3D11RasterizerState* oldRasterState;
    d3dContext->RSGetState(&oldRasterState);
    /**/
    D3D11_RASTERIZER_DESC rasterDesc;
    rasterDesc.AntialiasedLineEnable = false;
    rasterDesc.CullMode = D3D11_CULL_NONE;
    rasterDesc.DepthBias = 0;
    rasterDesc.DepthBiasClamp = 0.0f;
    rasterDesc.DepthClipEnable = true;
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.FrontCounterClockwise = false;
    rasterDesc.MultisampleEnable = false;
    rasterDesc.ScissorEnable = false;
    rasterDesc.SlopeScaledDepthBias = 0.0f;
    ID3D11RasterizerState* rasterState;
    d3dDevice->CreateRasterizerState(&rasterDesc, &rasterState);
    d3dContext->RSSetState(rasterState);
    rasterState->Release();

    /* 全テクスチャを焼き込む */
    for(unsigned int i=0;i<numTexture;++i)
    {
        /**/
        nlTexture& tex = texuters[i];
        /* ビューポートの設定 */
        D3D11_VIEWPORT viewport;
        viewport.Width = tex.width;
        viewport.Height = tex.height;
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;
        viewport.TopLeftX = 0.0f;
        viewport.TopLeftY = 0.0f;
        d3dContext->RSSetViewports(1, &viewport);
        /* シェーダを設定する */
        d3dContext->VSSetShader( vsRenderQuad, NULL, 0);
        d3dContext->PSSetShader( texturShaders[i], NULL, 0);
        /* レンダーターゲットを設定する */
        const float clearColor[4] = {0.0f, 0.0f, 1.0f, 0.0f};
        d3dContext->ClearRenderTargetView( tex.rtView, clearColor );
        d3dContext->OMSetRenderTargets( 1, &tex.rtView, NULL );

        /* 全てのテクスチャを設定する */
        /* TODO 毎回全てのテクスチャを設定せずに、このループで設定したテクスチャを省くようにだけする？*/
        for( unsigned int j=0;j<numTexture;++j)
        {
            if( i != j )
            {
                nlTexture& texSamp = texuters[j];
                d3dContext->PSSetShaderResources(j, 1, &texSamp.shaderView );
            }
        }

        /* 描画する */
        d3dContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
        d3dContext->Draw( 4, 0 );
    }
    /**/
    d3dContext->VSSetShader( NULL, NULL, 0);
    d3dContext->PSSetShader( NULL, NULL, 0);

    /* ビューポート設定を元に戻す */
    /* 現在のビューポート設定を取得しておく */
    D3D11_VIEWPORT oldViewports = {0.0,0.0,cxt->rendertargets[0].width_,cxt->rendertargets[0].height_,0.0f,1.0f};
    d3dContext->RSSetViewports( 1, &oldViewports );

    /* RasterState設定を元に戻す */
    d3dContext->RSSetState(oldRasterState);
    if( oldRasterState ){ oldRasterState->Release(); }

    /* PSを解放 */
    for(unsigned int i=0;i<numTexture;++i)
    {
        if( texturShaders[i] ){ texturShaders[i]->Release(); }
    }
#endif
}
/* @NOTE この関数は近い将来撤去されます */
void nlCopyToD3DBuffer(
                       ID3D11Buffer* constBuffer, 
                       const nlInt8* data, 
                       unsigned int dataSize,
                       nlEngineContext& cxt )
{
    D3D11_MAPPED_SUBRESOURCE mappedResrouce;
    NL_HR_VALID( cxt.d3dContext->Map( constBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResrouce ) );
    nlMemcpy( mappedResrouce.pData, data, dataSize );
    cxt.d3dContext->Unmap( constBuffer, 0 );
}
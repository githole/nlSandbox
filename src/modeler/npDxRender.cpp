#include "main/stdafx.h"
#include "npDxRender.h"
#include "np.h"
#include "nlPoly.h"
#include "log.h"

using namespace std;

/**/
npDxRender::npDxRender( HWND hwnd )
:d3dDevice(NULL),
d3dContext(NULL),
dxgiFactory(NULL),
dxgiSwpChain(NULL),
backBufferView(NULL),
depthStencil(NULL),
futureLevel(D3D_FEATURE_LEVEL_11_0),
lineVertexBuffer(NULL),
lineVS(NULL),
linePS(NULL),
lineIL(NULL),
numLineVertex(0),
meshVB(NULL),
meshIB(NULL),
numMeshIndex(0),
meshVS(NULL),
meshPS(NULL),
meshIL(NULL)
{
    /* TODO 初期化を行う */
    initD3D( hwnd );
    /* 座標軸描画用のデータを作成する */
    constructAxisLine();
    /**/
    constructConstantBuffer();
    /**/
    constructLineShader();
    constructMeshShader();
}
/**/
npDxRender::~npDxRender()
{
}
/**/
void npDxRender::initD3D( HWND hwnd )
{
    /**/
    IDXGIAdapter* pAdapter		= NULL;
    IDXGIDevice1* pDXGI			= NULL;
    IDXGIFactory* pDXGIFactory	= NULL;
    D3D_FEATURE_LEVEL fl;
    /**/ 
    /* create d3d device */
    NL_HR_VALID( D3D11CreateDevice( NULL, D3D_DRIVER_TYPE_HARDWARE,(HMODULE)0,0,NULL,0, D3D11_SDK_VERSION, &d3dDevice, &fl, &d3dContext ) );
    NL_HR_ASSSERT( fl == D3D_FEATURE_LEVEL_11_0 );
    /* create DXGI */
    NL_HR_VALID( d3dDevice->QueryInterface(__uuidof(IDXGIDevice1), (void**)&pDXGI) );
    /* get adapter info */
    NL_HR_VALID( pDXGI->GetAdapter(&pAdapter) );
    NL_HR_ASSSERT( pAdapter != NULL );
    /* get factory */
    NL_HR_VALID( pAdapter->GetParent( __uuidof(IDXGIFactory), (void**)&pDXGIFactory)  );
    NL_HR_ASSSERT( pDXGIFactory != NULL );
    {/* create swap chain */
        DXGI_SWAP_CHAIN_DESC swapChainDesc =
        {
            {BACKBUFFER_WIDTH, BACKBUFFER_HEIGHT, {60,1}, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED, DXGI_MODE_SCALING_UNSPECIFIED },
            {1, 0}, DXGI_USAGE_RENDER_TARGET_OUTPUT,
            3, hwnd, TRUE, DXGI_SWAP_EFFECT_DISCARD,
            DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH,
        };
        NL_HR_VALID( pDXGIFactory->CreateSwapChain(d3dDevice, &swapChainDesc, &dxgiSwpChain) );
    }
    /* Disalbe "Alt+Enter" and so on. must call this after CreateSwapChain() call MakeWindowAssociation(). */
    NL_HR_VALID( pDXGIFactory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_WINDOW_CHANGES | DXGI_MWA_NO_ALT_ENTER | DXGI_MWA_NO_PRINT_SCREEN ));
    {/* get backbuffer view */
        ID3D11Texture2D* pBackBuffer;
        NL_HR_VALID( dxgiSwpChain->GetBuffer( 0, __uuidof(ID3D11Texture2D), (void**)(&pBackBuffer) ) );
        NL_HR_VALID( d3dDevice->CreateRenderTargetView( pBackBuffer, NULL, &backBufferView ) );
        pBackBuffer->Release();

    }
    {/* create and set depthStencil */
        ID3D11Texture2D* pDepthStencil = NULL;
        D3D11_TEXTURE2D_DESC depthDesc =
        {
            BACKBUFFER_WIDTH,
            BACKBUFFER_HEIGHT,
            1, 1, DXGI_FORMAT_D24_UNORM_S8_UINT,
            {1,0}, D3D11_USAGE_DEFAULT,
            D3D11_BIND_DEPTH_STENCIL, 0, 0,
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
        NL_HR_VALID( d3dDevice->CreateTexture2D(&depthDesc, NULL, &pDepthStencil) );
        NL_HR_VALID( d3dDevice->CreateDepthStencilView(pDepthStencil, &dsvDesc, &depthStencil) );
    }
    d3dContext->OMSetRenderTargets(1, &backBufferView.p, depthStencil);

    /* デフォルトの設定 */
    D3D11_VIEWPORT	viewportDesc =
    {
        /* FLOAT TopLeftX; */
        0.0f,
        /* FLOAT TopLeftY; */
        0.0f,
        /* FLOAT Width; */
        (FLOAT)BACKBUFFER_WIDTH,
        /* FLOAT Height; */
        (FLOAT)BACKBUFFER_HEIGHT,
        /* FLOAT MinDepth; */
        0.0f,
        /* FLOAT MaxDepth; */
        1.0f,
    };
    viewportDesc = viewportDesc;
    d3dContext->RSSetViewports(1, &viewportDesc );  
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
    ID3D11RasterizerState* rasterizerState;
    d3dDevice->CreateRasterizerState(&rasterDesc, &rasterizerState);
    d3dContext->RSSetState(rasterizerState);
    rasterizerState->Release();
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
    d3dDevice->CreateDepthStencilState(&depthStencilDesc, &depthStencilState );
    d3dContext->OMSetDepthStencilState(depthStencilState, 0);
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
    samplerDesc = samplerDesc;
}
/**/
void npDxRender::constructAxisLine()
{
    /* 軸描画用用のバッファを作成する */
    vector<LineVertex> lineVertices;
    const float d = 11.0f;
    const float zero[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    const float xDir[4] = {d,    0.0f, 0.0f, 1.0f};
    const float yDir[4] = {0.0f, d,    0.0f, 1.0f};
    const float zDir[4] = {0.0f, 0.0f, d,    1.0f};
    const D3DCOLOR red   = D3DCOLOR_XRGB(0xff,0x00,0x00);
    const D3DCOLOR green = D3DCOLOR_XRGB(0x00,0xff,0x00);
    const D3DCOLOR blue  = D3DCOLOR_XRGB(0x00,0x00,0xff);
    const D3DCOLOR gray  = D3DCOLOR_XRGB(0x80,0x80,0x80);
    /* X軸 */
    lineVertices.push_back( LineVertex(zero, red) );
    lineVertices.push_back( LineVertex(xDir, red) );
    /* Y軸 */
    lineVertices.push_back( LineVertex(zero, green) );
    lineVertices.push_back( LineVertex(yDir, green) );
    /* Z軸 */
    lineVertices.push_back( LineVertex(zero, blue) );
    lineVertices.push_back( LineVertex(zDir, blue) );
    /**/
    const int numLine = 21;
    const float LEN = (numLine-1); 
    /* X軸方向のその他の軸 */
    for( int i=0;i<numLine;++i)
    {
        const float z = (float)(i-numLine/2);
        if( numLine/2 == i  )
        {
            float p0[4] = { -LEN*0.5f, 0.0f, z, 1.0f };
            float p1[4] = { 0.0f, 0.0f, z, 1.0f };
            lineVertices.push_back( LineVertex(p0, gray) );
            lineVertices.push_back( LineVertex(p1, gray) );
        }
        else
        {
            float p0[4] = { -LEN*0.5f, 0.0f, z, 1.0f };
            float p1[4] = {  LEN*0.5f, 0.0f, z, 1.0f };
            lineVertices.push_back( LineVertex(p0, gray) );
            lineVertices.push_back( LineVertex(p1, gray) );
        }
    }
    /* z軸に沿ったもの */
    for( int i=0;i<numLine;++i)
    {
        const float x = (float)(i-numLine/2);
        if( numLine/2 == i  )
        {
            float p0[4] = { x, 0.0f, -LEN*0.5f, 1.0f };
            float p1[4] = { x, 0.0f, 0.0f,      1.0f };
            lineVertices.push_back( LineVertex(p0, gray) );
            lineVertices.push_back( LineVertex(p1, gray) );
        }
        else
        {
            float p0[4] = { x, 0.0f, -LEN*0.5f, 1.0f };
            float p1[4] = { x, 0.0f, LEN*0.5f,   1.0f };
            lineVertices.push_back( LineVertex(p0, gray) );
            lineVertices.push_back( LineVertex(p1, gray) );
        }
    }


    /* 頂点バッファに変換する */
    D3D11_BUFFER_DESC desc =
    {
        sizeof(LineVertex)*lineVertices.size(), 
        D3D11_USAGE_DYNAMIC,D3D11_BIND_VERTEX_BUFFER,
        D3D11_CPU_ACCESS_WRITE, 0, 0
    };
    D3D11_SUBRESOURCE_DATA* pSubResource = NULL;
    NL_HR_VALID( d3dDevice->CreateBuffer(&desc, pSubResource, &lineVertexBuffer) );
    D3D11_MAPPED_SUBRESOURCE mappedResrouce;
    NL_HR_VALID( d3dContext->Map( lineVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResrouce ) );
    memcpy( mappedResrouce.pData, &lineVertices[0], lineVertices.size()*sizeof(LineVertex) );
    d3dContext->Unmap( lineVertexBuffer, 0 );
    numLineVertex = lineVertices.size();
}
/**/
void npDxRender::constructConstantBuffer()
{
    D3D11_BUFFER_DESC desc =
    {
        sizeof(D3DXMATRIX),
        D3D11_USAGE_DYNAMIC,
        D3D11_BIND_CONSTANT_BUFFER,
        D3D11_CPU_ACCESS_WRITE,
        0, 0
    };
    D3D11_SUBRESOURCE_DATA* pSubResource = NULL;
    NL_HR_VALID( d3dDevice->CreateBuffer(&desc, pSubResource, &constantBufferViewMatrix) );
}
/**/
void npDxRender::constructLineShader()
{
    /**/
    const char shaderAxis[] =
    {
        "cbuffer PerFrameData:register(b0)\n"
        "{\n"
        "column_major float4x4 viewProj;\n"
        "};\n"
        "struct VO\n"
        "{\n"
        "float4 pos:SV_POSITION;\n"
        "float4 col:COLOR0;\n"
        "};\n"
        "VO vs(\n"
        "         float4 in_pos :P,\n"
        "         float4 color  :C )\n"
        "{\n"
        "    VO ret;\n"
        "    ret.pos = mul( viewProj, in_pos );\n"
        "    ret.col = color;\n"
        "    return ret;\n"
        "}\n"
        "float4 ps( VO vsVal ):SV_Target\n"
        "{\n"
        "    return vsVal.col;\n"
        "}\n"
    };
    /* 頂点シェーダの作成 */
    ID3DBlob* pBlob = NULL;
    ID3DBlob* pErrorBlob = NULL;
    if(FAILED( 
        D3DX11CompileFromMemory( 
        shaderAxis, sizeof(shaderAxis)/sizeof(char), 
        "vs", NULL, NULL, "vs", "vs_5_0", 
        (D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR),
        0, NULL, &pBlob, &pErrorBlob, NULL ) ) )
    { MessageBoxA(NULL,(char*)pErrorBlob->GetBufferPointer(),"",MB_OK); }
    NL_HR_VALID( d3dDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &lineVS ) );
    /* 頂点レイアウトの作成 */
    static D3D11_INPUT_ELEMENT_DESC inputElement[] = 
    {
        { "P", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "C", 0, DXGI_FORMAT_R8G8B8A8_UNORM,     0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    /* create inputlayout */
    /* TODO ここのレイアウト作成を他に出せないかを確認してみる */
    NL_HR_VALID( d3dDevice->CreateInputLayout(inputElement, _countof(inputElement), pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &lineIL ) );

    /* ピクセルシェーダの作成 */
    if( FAILED(
        D3DX11CompileFromMemory( 
        shaderAxis, sizeof(shaderAxis)/sizeof(char), 
        "ps", NULL, NULL, "ps", 
        "ps_5_0", 0, 0, NULL, &pBlob, &pErrorBlob, NULL ) ) )
    {
        string error = std::string("[]");
        error.resize(error.size()-1);/* 改行コードを取り除く */
        if(pErrorBlob)
        { 
            error += std::string( (char*)pErrorBlob->GetBufferPointer() );
            error.resize(error.size()-1);/* 改行コードを取り除く */
        }
        MessageBoxA( NULL, error.c_str(), "error", MB_OK  );
    }
    NL_HR_VALID( d3dDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &linePS ) );
}
/**/
void npDxRender::constructMeshShader()
{
    const char shader[] =
    {
        "cbuffer PerFrameData:register(b0)\n"
        "{\n"
        "   column_major float4x4 viewProj;\n"
        "};\n"
        "float4 lightDir = float4(1,1,1,1);\n"
        "struct VO\n"
        "{\n"
        "   float4 pos:SV_POSITION;\n"
        "   float3 normal:TEXCOORD1;"
        "   float2 uv:TEXCOORD0;"
        "   float4 col:TEXCOORD2;"
        "};\n"
        "VO vs(\n"
        "  float4 in_pos:P,\n"
        "  float4 in_normal:N,\n"
        "  float4 in_uv:U )\n"
        "{\n"
        "    VO ret;\n"
        "    ret.pos = mul( viewProj, in_pos );\n"
        "    ret.uv  = in_uv;\n"
        "    ret.normal  = in_normal;\n"
        "    //ret.col = dot( in_normal, lightDir)*100;\n"
        "    ret.col = in_normal;\n"
        "    return ret;\n"
        "}\n"
        "float4 ps( VO input ):SV_Target0\n"
        "{\n"
        "    return input.col;\n"
        "}\n"
    };
    /* TODO VertexShader作成*/
    /* 頂点シェーダの作成 */
    ID3DBlob* pBlob = NULL;
    ID3DBlob* pErrorBlob = NULL;
    if(FAILED( 
        D3DX11CompileFromMemory( 
        shader, sizeof(shader)/sizeof(char), 
        "vs", NULL, NULL, "vs", "vs_5_0", 
        (D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR),
        0, NULL, &pBlob, &pErrorBlob, NULL ) ) )
    { MessageBoxA(NULL,(char*)pErrorBlob->GetBufferPointer(),"",MB_OK); }
    NL_HR_VALID( d3dDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &meshVS ) );
    /* 頂点レイアウトの作成 */
    static D3D11_INPUT_ELEMENT_DESC inputElement[] = 
    {
        { "P", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "N", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "U", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    /* create inputlayout */
    /* TODO ここのレイアウト作成を他に出せないかを確認してみる */
    NL_HR_VALID( d3dDevice->CreateInputLayout(inputElement, _countof(inputElement), pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &meshIL ) );

    /* ピクセルシェーダの作成 */
    if( FAILED(
        D3DX11CompileFromMemory( 
        shader, sizeof(shader)/sizeof(char), 
        "ps", NULL, NULL, "ps", 
        "ps_5_0", 0, 0, NULL, &pBlob, &pErrorBlob, NULL ) ) )
    {
        string error = std::string("[]");
        error.resize(error.size()-1);/* 改行コードを取り除く */
        if(pErrorBlob)
        { 
            error += std::string( (char*)pErrorBlob->GetBufferPointer() );
            error.resize(error.size()-1);/* 改行コードを取り除く */
        }
        MessageBoxA( NULL, error.c_str(), "error", MB_OK  );
    }
    NL_HR_VALID( d3dDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &meshPS ) );
}
/**/
void npDxRender::render( float eyePos[3] )
{
    /* ViewProjection行列の作成 */
    D3DXMATRIX matView, matProj, matViewPorj;
    D3DXVECTOR3 zero(0.0f,0.0f,0.0f);
    D3DXVECTOR3 up(0.0f,1.0f,0.0f);
    const float fovy = 3.1415926535f*0.5f;
    const float aspect = (float)BACKBUFFER_WIDTH/BACKBUFFER_HEIGHT;
    const float zn = 0.1f;
    const float zf = 1000.0f;
    D3DXMatrixLookAtRH( &matView, (D3DXVECTOR3*)eyePos, (D3DXVECTOR3*)&zero, (D3DXVECTOR3*)&up );
    D3DXMatrixPerspectiveFovRH( &matProj, fovy, aspect, zn, zf );
    D3DXMatrixMultiply( &matViewPorj, &matView, &matProj );

    /* ViewProjection行列の設定 */
    D3D11_MAPPED_SUBRESOURCE mappedResrouce;
    NL_HR_VALID( d3dContext->Map( constantBufferViewMatrix, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResrouce ) );
    memcpy( mappedResrouce.pData, (char*)&matViewPorj,sizeof(matViewPorj) );
    d3dContext->Unmap( constantBufferViewMatrix, 0 );
    d3dContext->VSSetConstantBuffers( 0, 1, &constantBufferViewMatrix.p );
    d3dContext->PSSetConstantBuffers( 0, 1, &constantBufferViewMatrix.p );

    /* バックバッファをクリアする */
    float clearColor[4] = { (float)0x1E/(float)0xff, (float)0x41/(float)0xff, (float)0x5A/(float)0xff, 0.0f };
    d3dContext->ClearRenderTargetView( backBufferView, clearColor );
    d3dContext->ClearDepthStencilView( depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0 );

    /* 軸を描画する */
    d3dContext->IASetInputLayout( lineIL );
    d3dContext->VSSetShader( lineVS, NULL, 0);
    d3dContext->PSSetShader( linePS, NULL, 0);
    ID3D11Buffer* pBufferTbl[] = { lineVertexBuffer };
    UINT SizeTbl[] = { sizeof(LineVertex) };
    UINT OffsetTbl[] = { 0 };
    d3dContext->IASetVertexBuffers(0, 1, pBufferTbl, SizeTbl, OffsetTbl);
    d3dContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_LINELIST );
    d3dContext->Draw( numLineVertex, 0 );

    /* mesh を描画する */
    d3dContext->IASetInputLayout( meshIL );
    d3dContext->VSSetShader( meshVS, NULL, 0);
    d3dContext->PSSetShader( meshPS, NULL, 0);
    ID3D11Buffer* meshBufferTbl[] = { meshVB };
    UINT meshSizeTbl[] = { sizeof(MeshVertex) };
    UINT meshOffsetTbl[] = { 0 };
    d3dContext->IASetVertexBuffers(0, 1, meshBufferTbl, meshSizeTbl, meshOffsetTbl);
    d3dContext->IASetIndexBuffer( meshIB, DXGI_FORMAT_R32_UINT, 0);
    d3dContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
    d3dContext->DrawIndexed( numMeshIndex, 0, 0);

    /**/
    d3dContext->Flush();
    NL_HR_VALID( dxgiSwpChain->Present( 1, 0) );
}
/**/
void npDxRender::setNewMesh( npTriangleMesh* newMesh )
{
    /* 頂点数/面数が0の場合は何もしない */
    if( newMesh->numVert == 0 || newMesh->numFace == 0 )
    {
        return;
    }
    /* TODO D3D用のメッシュに構築する */
    newMesh->numVert;
    newMesh->numFace;
    newMesh->indexes;

    log("start construct mesh");

    /* create vertex buffer */
    D3D11_BUFFER_DESC descVB =
    {
        newMesh->numVert*sizeof(MeshVertex),
        D3D11_USAGE_DYNAMIC,
        D3D11_BIND_VERTEX_BUFFER,
        D3D11_CPU_ACCESS_WRITE,
        0, 0
    };
    D3D11_SUBRESOURCE_DATA* subResourceVB = NULL;
    ID3D11Buffer* tmpMeshVB = NULL;
    NL_HR_VALID( d3dDevice->CreateBuffer( &descVB, subResourceVB, &tmpMeshVB ) );
    meshVB.Attach(tmpMeshVB);

    /* create index buffer */
    D3D11_BUFFER_DESC descIB =
    {
        sizeof(unsigned int)*newMesh->numFace*3,
        D3D11_USAGE_DYNAMIC,
        D3D11_BIND_INDEX_BUFFER,
        D3D11_CPU_ACCESS_WRITE,
        0, 0
    };
    D3D11_SUBRESOURCE_DATA* pSubResourceIB = NULL;
    ID3D11Buffer* tmpMeshIB = NULL;
    NL_HR_VALID( d3dDevice->CreateBuffer(&descIB, pSubResourceIB, &tmpMeshIB ) );
    meshIB.Attach(tmpMeshIB);

    /* copy vertex buffer */
    D3D11_MAPPED_SUBRESOURCE mappedVB;
    NL_HR_VALID( d3dContext->Map( meshVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedVB ) );
    MeshVertex* dstVertex = (MeshVertex*)mappedVB.pData;
    for( unsigned int vi=0;vi<newMesh->numVert;++vi)
    {
        npTriangleMesh::npVertex& srcVertex = newMesh->vertex[vi];
        dstVertex->pos[0] = srcVertex.position[0];
        dstVertex->pos[1] = srcVertex.position[1];
        dstVertex->pos[2] = srcVertex.position[2];
        dstVertex->pos[3] = 1.0f;
        /**/
        dstVertex->normal[0] = srcVertex.normal[0];
        dstVertex->normal[1] = srcVertex.normal[1];
        dstVertex->normal[2] = srcVertex.normal[2];
        dstVertex->normal[3] = 1.0f;
        /**/
        ++dstVertex;
    }
    d3dContext->Unmap( meshVB, 0);

    /* copy index */
    D3D11_MAPPED_SUBRESOURCE mappedIB;
    NL_HR_VALID( d3dContext->Map( meshIB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedIB ) );
    unsigned int* dstIndex = (unsigned int*)mappedIB.pData;
    for( unsigned int ii=0;ii<newMesh->numFace;++ii)
    {
        dstIndex[ii*3+0] = newMesh->indexes[ii][0];
        dstIndex[ii*3+1] = newMesh->indexes[ii][1];
        dstIndex[ii*3+2] = newMesh->indexes[ii][2];
    }
    d3dContext->Unmap( meshIB, 0);
    numMeshIndex = newMesh->numFace * 3;

    /**/
    QString finishMes;
    finishMes.sprintf( "vert:[%d] face:[%d]", newMesh->numVert, newMesh->numFace );
    log(finishMes);
    log("finish construct mesh");
}

#ifndef _NPDXRENDER_H_
#define _NPDXRENDER_H_

#include "main/stdafx.h"

class npDxRender
{
public:
    npDxRender( HWND hwnd );
    ~npDxRender();
    void render( float eyePos[3] );
    void setNewMesh( struct npTriangleMesh* newMesh );
private:
    void initD3D( HWND hwnd );
    void constructAxisLine();
    void constructConstantBuffer();
    void constructLineShader();
    void constructMeshShader();
private:
    /**/
    CComPtr<ID3D11Device>        d3dDevice;
    CComPtr<ID3D11DeviceContext> d3dContext;
    CComPtr<IDXGIFactory>        dxgiFactory;
    CComPtr<IDXGISwapChain>      dxgiSwpChain;
    CComPtr<ID3D11RenderTargetView>  backBufferView;
    CComPtr<ID3D11DepthStencilView>  depthStencil;
    D3D_FEATURE_LEVEL        futureLevel;   
    /* ê¸ï`âÊ */
    struct LineVertex
    {
        float pos[4];
        D3DCOLOR color;
        /**/
        LineVertex(
            const float aPos[4],
            D3DCOLOR aColor )
        {
            for( int i=0;i<4;++i)
            { pos[i] = aPos[i]; }
            color = aColor;
        }
    };
    CComPtr<ID3D11Buffer> lineVertexBuffer;
    CComPtr<ID3D11VertexShader> lineVS;
    CComPtr<ID3D11PixelShader>  linePS;
    CComPtr<ID3D11InputLayout>  lineIL;
    unsigned int numLineVertex;
    /* ÉÇÉfÉãï`âÊ */
    struct MeshVertex
    {
        float pos[4];
        float normal[4];
        float uv[4];
    };
    CComPtr<ID3D11Buffer> meshVB;
    CComPtr<ID3D11Buffer> meshIB;
    unsigned int numMeshIndex;
    CComPtr<ID3D11VertexShader> meshVS;
    CComPtr<ID3D11PixelShader>  meshPS;
    CComPtr<ID3D11InputLayout>  meshIL;
    /**/
    CComPtr<ID3D11Buffer> constantBufferViewMatrix;
};

#endif

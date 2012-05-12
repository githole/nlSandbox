#include "main/stdafx.h"
#include "widget_preview.h"
#include "nlSandbox.h"
#include "tool/nlCam.h"
#include "shared/render/nlRender.h"
#include "shared/container/nlArray.hpp"
#include "shared/sound/nlSound.h"

using namespace std;

/**/
widget_preview::widget_preview( QWidget *parent )
    :QWidget( parent ),
    isEnableDebugCam_(true),
	isRenderDebugLine_(true),
    cxt_(NULL),
    isInitialized_(false)
{
	/* このフラグさえつければチラツキはなくなる. */
	setAttribute(Qt::WA_PaintOnScreen, true );
	/* ネット上で散見されていたこれらのフラグは要る？ */
	//setAutoFillBackground(false);
	//setAttribute(Qt::WA_MSWindowsUseDirect3D, true); 
	//setAttribute(Qt::WA_NoSystemBackground, true );
	//setAttribute(Qt::WA_OpaquePaintEvent, false );
    setFocusPolicy( Qt::StrongFocus );
}
/**/
void widget_preview::initialize( nlEngineContext* cxt )
{
    cxt_ = cxt;
    constructRenderAxisResource(cxt);
    isInitialized_ = true;
}
/**/
void widget_preview::constructRenderAxisResource( nlEngineContext* cxt )
{
    /**/
    ID3D11Device* d3dDevice = cxt->d3dDevice;
    ID3D11DeviceContext* d3dContext = cxt->d3dContext;

    /* TODO 軸描画に必要なリソースをロードする */
    /* TODO VSをロード */
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
        "vs", NULL, NULL, "vs", "vs_4_0", 
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
        "ps_4_0", 0, 0, NULL, &pBlob, &pErrorBlob, NULL ) ) )
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
    NL_HR_VALID( d3dDevice->CreateBuffer(&desc, NULL, &lineVertexBuffer) );
    D3D11_MAPPED_SUBRESOURCE mappedResrouce;
    NL_HR_VALID( d3dContext->Map( lineVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResrouce ) );
    memcpy( mappedResrouce.pData, &lineVertices[0], lineVertices.size()*sizeof(LineVertex) );
    d3dContext->Unmap( lineVertexBuffer, 0 );
    numLineVertex = lineVertices.size();

    /* ConstantBufferの作成 */
     D3D11_BUFFER_DESC constantBufferDesc =
    {
        sizeof(D3DXMATRIX),
        D3D11_USAGE_DYNAMIC,
        D3D11_BIND_CONSTANT_BUFFER,
        D3D11_CPU_ACCESS_WRITE,
        0, 0
    };
    NL_HR_VALID( d3dDevice->CreateBuffer(&constantBufferDesc, NULL, &constantBufferViewMatrix) );
}
/**/
widget_preview::~widget_preview()
{
}
/**/
void widget_preview::mousePressEvent(QMouseEvent* e )
{
    Qt::MouseButtons btn = e->button();
    switch( e->button() )
    {
    case Qt::RightButton: 
        camera_.onRButtonClick( e->x(), e->y() );
        RenderingForce();
        break;
    case Qt::MidButton:   
        camera_.onMButtonClick( e->x(), e->y() ); 
        RenderingForce();
        break;
	case Qt::LeftButton:
		/* タイムラインのスライダーを持つ */
		prevPressPointX_ = e->x();
		//prevPressPoint_ = e->position();
		break;
    }
    
}
/**/
void widget_preview::mouseMoveEvent( QMouseEvent* e )
{
    switch( e->buttons() )
    {
    case Qt::RightButton: 
        camera_.onRButtonDrag( e->x(), e->y() ); 
        RenderingForce();
        break;
    case Qt::MidButton: 
        camera_.onMButtonDrag( e->x(), e->y() ); 
        RenderingForce();
        break;
	case Qt::LeftButton:
		{
			int dif = (e->x()-prevPressPointX_)*16;
			prevPressPointX_ = e->x();
			ui.dockTimeline->movePosition(dif);
		}
		break;
    }
}
/**/
void widget_preview::mouseReleaseEvent(QMouseEvent* e)
{

}
/**/
void widget_preview::wheelEvent (QWheelEvent* e)
{
    camera_.onMouseWheel( e->delta() );
    RenderingForce();
}
/**/
void widget_preview::keyPressEvent(QKeyEvent* e )
{
    /* F2で視点位置をリセットする */
    if( e->key() == Qt::Key_F2 )
    {
        camera_.defautlView();
        RenderingForce();
    }
    /* F3で視点設定を切り替える */
    if( e->key() == Qt::Key_F3 )
    {
        isEnableDebugCam_ = !isEnableDebugCam_;
        RenderingForce();
    }
    /* F4で軸の表示を切り替える　*/
    if( e->key() == Qt::Key_F4 )
    {
        isRenderDebugLine_ = !isRenderDebugLine_;
        RenderingForce();
    }
    /* スペースキーでタイムラインの再生を切り替えられる */
	if( e->key() == Qt::Key_Space )
	{ui.dockTimeline->togglePlay();}
    /* 矢印キーで3秒前後する */
    if( (e->key() == Qt::Key_Right ) )
    {ui.dockTimeline->movePosition(3000);}
    if( (e->key() == Qt::Key_Left ) )
    {ui.dockTimeline->movePosition(-3000);}
}
/**/
void widget_preview::updateFPS()
{
    if( !isInitialized_ )
    {
        return;
    }
    /**/
	const unsigned int frameInterval_   = 30;
	static unsigned int countStartTime_ = 0;
	static unsigned int frameCount_     = 0;
	if( !(frameCount_ % frameInterval_) )
	{
		/* FPSを計算する */
		frameCount_ = frameInterval_;
		unsigned long elapse = timeGetTime() - countStartTime_;
		countStartTime_ = timeGetTime();
		float fps = frameInterval_*1000/(float)elapse ;
		/* 表示する */
		QString fpsStr;
		fpsStr.sprintf("FPS:%2.2f",fps);
		ui.labelFps->setText(fpsStr);
	}
	--frameCount_;
}
/**/
void widget_preview::Rendering()
{
    if( !isInitialized_ )
    { return;}
    /* 
    前回のレンダリングから15ms経過していない場合はキャンセルする。
    これはレンダリングがVSync同期の60FPSで動作することを仮定している。
    (16.66から少し余裕を持たせて15msにしている)
    */
    static unsigned long prevRender = 0;
    const unsigned long now = timeGetTime();
    if( now - prevRender < 15 )
    { return;  }
    prevRender = now;
    /**/
    RenderingForce();
}
/**/
void widget_preview::RenderingForce()
{
    /**/
    const int nowInMs = ui.sliderTimeline->value();
    /**/
	updateFPS();
	/**/
    camera_.update( g_ctx );
    /* クリアしておく */
    SceneArray& scenes = nlScene_getSceneData();
    ProcessBuffer& processBuffer = nlScene_getProcessBuffer();
    for( int i=0;i<processBuffer.size();++i)
    {
        processBuffer[i].isProcessed_ = false;
    }
    /* デバッグ用のカメラなら、SNT_DEBUG_CAMERAの出力を事前に決めてしまう */    
	if( isEnableDebugCam() )
    {
        SceneArray& scenes = nlScene_getSceneData();
        const int sceneIndex = nlScene_sceneIndex(nowInMs);
        nlScene& scene = scenes[sceneIndex];
        ProcessBuffer& processBuffer = nlScene_getProcessBuffer();
        for( int j=0;j<scene.sceneMaps_.size();++j)
        {
            if( scene.sceneMaps_[j].type_ == SNT_DEBUG_CAMERA )
            {
                nlCamDesc cam = ui.previewWidget->getCameDesc();
                nlMatrix view;
                D3DXMatrixLookAtRH( &view, (D3DXVECTOR3*)&cam.pos_, (D3DXVECTOR3*)&cam.lookat_, (D3DXVECTOR3*)&cam.updir_ );
                processBuffer[j].output_.resizeAsMatrix(1);
                processBuffer[j].output_.writeAsMatrix( 0, view );
                processBuffer[j].isProcessed_ = true;
            }
            if( scene.sceneMaps_[j].type_ == SNT_DEBUG_CAMERA_POS )
            {
                nlCamDesc cam = ui.previewWidget->getCameDesc();
                nlVec3& p3 = cam.pos_;
                nlVec4  p4 = {p3.x_,p3.y_,p3.z_,0.0f};
                nlMatrix view;
                processBuffer[j].output_.resizeAsVec4(1);
                processBuffer[j].output_.writeAsVec4( 0, p4 );
                processBuffer[j].isProcessed_ = true;
            }
        }
    }
    /**/
    nlRenderCommands commands;

    nlScene_Update( nowInMs, &commands, g_ctx );
	/* カメラコードだけデバッグ版を利用する場合はそうする */    
	if( isEnableDebugCam() )
	{
        /* 軸描画を追加 */
        commands.renderDebug( launchRenderAxis, this );
	}
    /* 描画 */
    nlRender( commands, &g_ctx );
}
/**/
void widget_preview::launchRenderAxis( nlEngineContext* cxt, void* userData )
{
    widget_preview* preview = (widget_preview*)userData;
    preview->renderAxis(cxt );
}
/**/
void widget_preview::renderAxis( nlEngineContext* cxt )
{
    /**/
    ID3D11Device* d3dDevice     = cxt->d3dDevice;
    ID3D11DeviceContext* d3dContext = cxt->d3dContext;
    /**/
    nlCamDesc cam = ui.previewWidget->getCameDesc();
    nlMatrix proj, view, viewProj;
    D3DXMatrixPerspectiveFovRH( &proj, cam.fovy_, cam.aspect_, cam.zn_, cam.zf_ );
    D3DXMatrixLookAtRH( &view, (D3DXVECTOR3*)&cam.pos_, (D3DXVECTOR3*)&cam.lookat_, (D3DXVECTOR3*)&cam.updir_ );
    D3DXMatrixMultiply( &viewProj, &view, &proj );
    /* 変更する設定をセーブする */
    ID3D11InputLayout* savedIL = NULL;
    d3dContext->IAGetInputLayout( &savedIL );
    D3D11_PRIMITIVE_TOPOLOGY topology;
    d3dContext->IAGetPrimitiveTopology( &topology );
    ID3D11Buffer* cb0Prev;
    d3dContext->VSGetConstantBuffers(0, 1, &cb0Prev);
    /* ViewProjection行列の設定 */
    D3D11_MAPPED_SUBRESOURCE mappedResrouce;
    NL_HR_VALID( d3dContext->Map( constantBufferViewMatrix, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResrouce ) );
    memcpy( mappedResrouce.pData, (char*)&viewProj,sizeof(viewProj) );
    d3dContext->Unmap( constantBufferViewMatrix, 0 );
    d3dContext->VSSetConstantBuffers( 0, 1, &constantBufferViewMatrix.p );
    /* 軸を描画する */   
    d3dContext->IASetInputLayout( lineIL );
    d3dContext->VSSetShader( lineVS, NULL, 0);
    d3dContext->PSSetShader( linePS, NULL, 0);
    d3dContext->GSSetShader( NULL, NULL, 0);
    ID3D11Buffer* pBufferTbl[] = { lineVertexBuffer };
    UINT SizeTbl[] = { sizeof(LineVertex) };
    UINT OffsetTbl[] = { 0 };
    d3dContext->IASetVertexBuffers(0, 1, pBufferTbl, SizeTbl, OffsetTbl);
    d3dContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_LINELIST );
    d3dContext->Draw( numLineVertex, 0 );
    /* 全ての設定を復元する */
    if( savedIL ) 
    {
        d3dContext->IASetInputLayout( savedIL );
        savedIL->Release();
    }
    if( cb0Prev )
    {
        d3dContext->VSSetConstantBuffers(0, 1, &cb0Prev);
        cb0Prev->Release();
    }
    d3dContext->IASetPrimitiveTopology( topology );
}

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
	/* ���̃t���O��������΃`���c�L�͂Ȃ��Ȃ�. */
	setAttribute(Qt::WA_PaintOnScreen, true );
	/* �l�b�g��ŎU������Ă��������̃t���O�͗v��H */
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

    /* TODO ���`��ɕK�v�ȃ��\�[�X�����[�h���� */
    /* TODO VS�����[�h */
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
    /* ���_�V�F�[�_�̍쐬 */
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
    /* ���_���C�A�E�g�̍쐬 */
    static D3D11_INPUT_ELEMENT_DESC inputElement[] = 
    {
        { "P", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "C", 0, DXGI_FORMAT_R8G8B8A8_UNORM,     0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    /* create inputlayout */
    /* TODO �����̃��C�A�E�g�쐬�𑼂ɏo���Ȃ������m�F���Ă݂� */
    NL_HR_VALID( d3dDevice->CreateInputLayout(inputElement, _countof(inputElement), pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &lineIL ) );

    /* �s�N�Z���V�F�[�_�̍쐬 */
    if( FAILED(
        D3DX11CompileFromMemory( 
        shaderAxis, sizeof(shaderAxis)/sizeof(char), 
        "ps", NULL, NULL, "ps", 
        "ps_4_0", 0, 0, NULL, &pBlob, &pErrorBlob, NULL ) ) )
    {
        string error = std::string("[]");
        error.resize(error.size()-1);/* ���s�R�[�h����菜�� */
        if(pErrorBlob)
        { 
            error += std::string( (char*)pErrorBlob->GetBufferPointer() );
            error.resize(error.size()-1);/* ���s�R�[�h����菜�� */
        }
        MessageBoxA( NULL, error.c_str(), "error", MB_OK  );
    }
    NL_HR_VALID( d3dDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &linePS ) );

    /* ���`��p�p�̃o�b�t�@���쐬���� */
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
    /* X�� */
    lineVertices.push_back( LineVertex(zero, red) );
    lineVertices.push_back( LineVertex(xDir, red) );
    /* Y�� */
    lineVertices.push_back( LineVertex(zero, green) );
    lineVertices.push_back( LineVertex(yDir, green) );
    /* Z�� */
    lineVertices.push_back( LineVertex(zero, blue) );
    lineVertices.push_back( LineVertex(zDir, blue) );
    /**/
    const int numLine = 21;
    const float LEN = (numLine-1); 
    /* X�������̂��̑��̎� */
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
    /* z���ɉ��������� */
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
    /* ���_�o�b�t�@�ɕϊ����� */
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

    /* ConstantBuffer�̍쐬 */
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
		/* �^�C�����C���̃X���C�_�[������ */
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
    /* F2�Ŏ��_�ʒu�����Z�b�g���� */
    if( e->key() == Qt::Key_F2 )
    {
        camera_.defautlView();
        RenderingForce();
    }
    /* F3�Ŏ��_�ݒ��؂�ւ��� */
    if( e->key() == Qt::Key_F3 )
    {
        isEnableDebugCam_ = !isEnableDebugCam_;
        RenderingForce();
    }
    /* F4�Ŏ��̕\����؂�ւ���@*/
    if( e->key() == Qt::Key_F4 )
    {
        isRenderDebugLine_ = !isRenderDebugLine_;
        RenderingForce();
    }
    /* �X�y�[�X�L�[�Ń^�C�����C���̍Đ���؂�ւ����� */
	if( e->key() == Qt::Key_Space )
	{ui.dockTimeline->togglePlay();}
    /* ���L�[��3�b�O�シ�� */
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
		/* FPS���v�Z���� */
		frameCount_ = frameInterval_;
		unsigned long elapse = timeGetTime() - countStartTime_;
		countStartTime_ = timeGetTime();
		float fps = frameInterval_*1000/(float)elapse ;
		/* �\������ */
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
    �O��̃����_�����O����15ms�o�߂��Ă��Ȃ��ꍇ�̓L�����Z������B
    ����̓����_�����O��VSync������60FPS�œ��삷�邱�Ƃ����肵�Ă���B
    (16.66���班���]�T����������15ms�ɂ��Ă���)
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
    /* �N���A���Ă��� */
    SceneArray& scenes = nlScene_getSceneData();
    ProcessBuffer& processBuffer = nlScene_getProcessBuffer();
    for( int i=0;i<processBuffer.size();++i)
    {
        processBuffer[i].isProcessed_ = false;
    }
    /* �f�o�b�O�p�̃J�����Ȃ�ASNT_DEBUG_CAMERA�̏o�͂����O�Ɍ��߂Ă��܂� */    
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
	/* �J�����R�[�h�����f�o�b�O�ł𗘗p����ꍇ�͂������� */    
	if( isEnableDebugCam() )
	{
        /* ���`���ǉ� */
        commands.renderDebug( launchRenderAxis, this );
	}
    /* �`�� */
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
    /* �ύX����ݒ���Z�[�u���� */
    ID3D11InputLayout* savedIL = NULL;
    d3dContext->IAGetInputLayout( &savedIL );
    D3D11_PRIMITIVE_TOPOLOGY topology;
    d3dContext->IAGetPrimitiveTopology( &topology );
    ID3D11Buffer* cb0Prev;
    d3dContext->VSGetConstantBuffers(0, 1, &cb0Prev);
    /* ViewProjection�s��̐ݒ� */
    D3D11_MAPPED_SUBRESOURCE mappedResrouce;
    NL_HR_VALID( d3dContext->Map( constantBufferViewMatrix, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResrouce ) );
    memcpy( mappedResrouce.pData, (char*)&viewProj,sizeof(viewProj) );
    d3dContext->Unmap( constantBufferViewMatrix, 0 );
    d3dContext->VSSetConstantBuffers( 0, 1, &constantBufferViewMatrix.p );
    /* ����`�悷�� */   
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
    /* �S�Ă̐ݒ�𕜌����� */
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

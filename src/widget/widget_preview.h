#pragma once

#include "main/stdafx.h"
#include "tool/nlCam.h"
#include "shared/math/nlMath.h"


class widget_preview 
    : public QWidget
{
    Q_OBJECT

public:
    widget_preview(QWidget *parent = 0);
    void initialize( struct nlEngineContext* cxt );
private:
    void widget_preview::constructRenderAxisResource( nlEngineContext* cxt );
public:
    ~widget_preview();

	void setRenderDebugLine( bool isRenderDebugLine )
	{ isRenderDebugLine_ = isRenderDebugLine; }
	void updateFPS();

public slots:
	void Rendering();
    void RenderingForce();

private slots:
    QPaintEngine* paintEngine() const
    { return 0;  }
    void paintEvent(QPaintEvent *p)
    { Rendering();}
    void repaintCanvas()
    { Rendering(); }
    void render ( QPaintDevice * target, const QPoint & targetOffset, const QRegion & sourceRegion, RenderFlags renderFlags  )
    { Rendering(); }
    void render ( QPainter * painter, const QPoint & targetOffset, const QRegion & sourceRegion, RenderFlags renderFlags )
    { Rendering(); }
    void repaint ( int x, int y, int w, int h )
    { Rendering(); }
    void repaint ( const QRect & rect )
    { Rendering(); }
    void repaint ( const QRegion & rgn )
    { Rendering(); }

    /**/
    void mousePressEvent(QMouseEvent* e);
    void mouseMoveEvent(QMouseEvent* e);
	void mouseReleaseEvent(QMouseEvent* e);
    void wheelEvent (QWheelEvent* e);
    void keyPressEvent(QKeyEvent* e );

    /**/
public:
    bool isEnableDebugCam()const
    {
        return isEnableDebugCam_;
    }
    nlCamDesc getCameDesc()const
    {
        return camera_.camDesc();
    }
private:
    static void launchRenderAxis( nlEngineContext* cxt, void* userData );
    void renderAxis( nlEngineContext* cxt );

private:
    bool isEnableDebugCam_;
	bool isRenderDebugLine_;
    nlCamFreedom camera_;
    struct nlEngineContext* cxt_;
    bool isInitialized_;

    /* ê¸ï`âÊóp */
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
    CComPtr<ID3D11Buffer> constantBufferViewMatrix;
	int prevPressPointX_;
};

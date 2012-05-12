#pragma once
#include "tool/nlCurveSerializer.hpp"

namespace Ui
{
    class nlSandboxClass;
};

/**
 * @brief 
 */
class widget_curve 
    : public QWidget
{
	Q_OBJECT
public:
	widget_curve( QWidget* parent );
	~widget_curve();
    void initialize(class Ui::nlSandboxClass* ui, struct nlEngineContext* cxt );

protected:
    void paintEvent(QPaintEvent* iPaintEvent);
	void mousePressEvent(QMouseEvent* e);
    void mouseMoveEvent(QMouseEvent* e);
	void mouseReleaseEvent(QMouseEvent* e);
	void mouseDoubleClickEvent(QMouseEvent* e);
    void keyPressEvent(QKeyEvent* e);

public slots:
	void onChangeCurrentCurve( int index );
    void onChangeCurveMode( int idx );
	void onResetCurve();
	void onClickLiner( bool isCheak );
	void onClickCatmul( bool isCheak );
signals:
    void onRepaintMainPreview();
private:
	enum grabePointXYZ_t
	{
		gpxyz_None,
		gpxyz_X,
		gpxyz_Y,
		gpxyz_Z,
	};
	void hitTestPathPoint( int x, int y, int* pathIdx, int* xyzIdx, grabePointXYZ_t grabXYZ = gpxyz_None )const;
	void movePoint_(QMouseEvent* e, bool isGrid, bool isOnlyUpdown );
public:
	void wheelEvent(QWheelEvent* e);
private:
	void renderDragpointInfo_( QPainter& painter );
	void renderBackground_( QPainter& painter );
	void renderPathPoint_( QPainter& painter );
	void renderCurve_( QPainter& painter );
	nlCurve* getCurrentPath_();
	const nlCurve* getCurrentPath_()const;
	void    getCurvePonit_( nlCurvePoint** prev, nlCurvePoint** cur, nlCurvePoint** next );
private:
	class View
	{
	public:
		View( int x, int y, float scale )
			:x_(x), y_(y), scale_(scale)
		{}
		int   getLeft()const;
		int   getTop()const;
		int   getRight()const;
		int   getBottom()const;
		float scale()const;
		void localToScreenPos_(int* ox, int* oy, float ix, float iy)const;
		void screenToLocalPos_(float* ox, float* oy, int ix, int iy)const;
		void onMousePress( int mx, int my);
		void onMouseMove( int mx, int my);
		void onMouseRelease( int mx, int my);
		void onMouseWheel( int mouseDelta );
		void reset();
	private:
		int   x_;
		int   y_;
		float scale_;
		int dragStartX_;
		int dragStartY_;
		int viewXBeforeDragX_;
		int viewXBeforeDragY_;
	}view_;
	int dragStartX_;
	int dragStartY_;
	/**/
	int curPathIdx_;
    int currentXYZIdx_;
	int draggedPointIdx_;
	int pointFrameBeforeDrag_;
	int pointVBeforeDrag_;
};
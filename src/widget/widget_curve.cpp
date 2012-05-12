#include "main/stdafx.h"
#include "widget_curve.h"
#include "nlsandbox.h"
#include "shared/curve/nlCurve.hpp"
#include "tool/nlUtil.hpp"
#include "tool/nlId.hpp"

using namespace std;
using namespace boost;

/**/
const int g_pathPointRadius = 15;
const float Y_SCALE = 30.0;

widget_curve::widget_curve( QWidget* parent )   
	:QWidget( parent ),
	curPathIdx_(0),
	draggedPointIdx_( -1 ),
	view_(0,-parent->height()/2, 0.2f )
{
    /**/
    setFocusPolicy( Qt::StrongFocus );
}
/**/
widget_curve::~widget_curve()
{
}
/**/
void widget_curve::initialize(Ui::nlSandboxClass* ui, nlEngineContext* cxt )
{
    /**/
    const QString groupName = "CURVE_NAME";
    clearId(groupName);
    /* 全てのカーブをロードする */
    QDir root(sandboxPath( SP_PATH ));
    BOOST_FOREACH(const QFileInfo& info , root.entryInfoList() )
    {
        if( info.isFile() && info.suffix() == "txt" )
        {
            /**/
            const QByteArray baseName = info.baseName().toLocal8Bit();
            const QByteArray absPath = info.absoluteFilePath().toLocal8Bit();
            setId( groupName, baseName );
            boost::optional<int> id = getId( "CURVE_NAME", baseName );
            /* 各当するパスをロードする */
            deserializeCurve( absPath.data(), cxt->paths[*id], nlCST_KDF );
            /* 項目名を登録 */
            ui->pathList->addItem( baseName );
        }
    }
}
/**/
void widget_curve::paintEvent(QPaintEvent* pe )
{
	QPainter painter(this);
	QPen newPen;
	newPen.setStyle( Qt::DotLine );
	painter.setPen( newPen );
	painter.setRenderHint(QPainter::Antialiasing);

	renderDragpointInfo_( painter );
	renderBackground_( painter );
	renderPathPoint_( painter );
	renderCurve_( painter );

	painter.end();
}
/**/
void widget_curve::renderDragpointInfo_( QPainter& painter )
{
	/* ドラッグ中のポイントの情報を表示 */
	QString infoStr;
	nlCurvePoint* pathPoint;
	getCurvePonit_(NULL, &pathPoint, NULL);
	if( pathPoint )
	{
		/* 座標 */
        const nlArray<float,3>& xyz = pathPoint->xyz_; 
		infoStr.sprintf("no.%d (%d,%2.1f,%2.1f,%2.1f)", draggedPointIdx_, pathPoint->time_,xyz[0], xyz[1], xyz[2] );
		painter.drawText( 100, 100, infoStr );
	}
}

/**/
nlCurve* widget_curve::getCurrentPath_()
{
	return &g_ctx.paths[curPathIdx_];
}
/**/
const nlCurve* widget_curve::getCurrentPath_()const
{
	return &g_ctx.paths[curPathIdx_];
}
/**/
void widget_curve::getCurvePonit_( 
								nlCurvePoint** prev, 
								nlCurvePoint** cur, 
								nlCurvePoint** next )
{
	if( prev ){ *prev = NULL; }
	if( cur  ){ *cur  = NULL; }
	if( next ){ *next = NULL; }

	nlCurve* curPath = getCurrentPath_();
	if( !curPath )
	{ return; }

	if( draggedPointIdx_ == -1 )
	{ return ; }

	int pathNum = curPath->points_.size();

#define GET_PATH( targetIdx, ret )\
	if( pathNum <= targetIdx )\
	{ return; }\
	if( ret && (0<=targetIdx) && ( targetIdx < curPath->points_.size()) )\
	{ *ret = &curPath->points_[targetIdx]; }

	GET_PATH( (draggedPointIdx_ - 1), prev );
	GET_PATH( (draggedPointIdx_    ), cur  );
	GET_PATH( (draggedPointIdx_ + 1), next );
}
/**/
void widget_curve::renderBackground_( QPainter& painter )
{
	/* 背景の升目を描画 */
	/* 背景の横線 */
	const int lineInter = 200;
	const int ixStart = view_.getLeft()/lineInter;
	const int ixEnd   = (width()/view_.scale() + view_.getLeft() )/lineInter + 1;
	for( int ix=ixStart;ix<ixEnd;++ix )
	{
		int dx = (ix * lineInter -view_.getLeft())*view_.scale();
		QPoint p1 = QPoint(dx, 0);
		QPoint p2 = QPoint(dx, height());
		painter.drawLine( p1, p2 );

		QString posXStr;
		posXStr.sprintf( "%d", ix*lineInter );
		painter.drawText( p2, posXStr );
	}
	const int iyStart = view_.getTop()/lineInter;
	const int iyEnd   = (height()/view_.scale()+view_.getTop())/lineInter + 1;
	for( int iy=iyStart;iy<iyEnd;++iy )
	{
		int dy = (iy*lineInter-view_.getTop())*view_.scale();
		QPoint p1 = QPoint(0, dy);
		QPoint p2 = QPoint(width(), dy);
		painter.drawLine( p1, p2 );

		int yPos = -(iy*lineInter);
		if( !(yPos % 100) )
		{
			QString posYStr;
			posYStr.sprintf( "%d", yPos/100 );
			painter.drawText( p1, posYStr );
		}
	}
	/* 中心線の描画 */
	{
		const int dx = -view_.getLeft()*view_.scale();
		const int dy = -view_.getTop()*view_.scale();
		const int penWidth = 3;
		painter.setPen( QPen(Qt::red,penWidth, Qt::DotLine ) );
		painter.drawLine( QPoint( 0,dy), QPoint( width(),dy) );
		painter.setPen( QPen(Qt::blue,penWidth, Qt::DotLine ) );
		painter.drawLine( QPoint( dx,0), QPoint( dx,height()) );
		
	}
}
/**/
void widget_curve::renderPathPoint_( QPainter& painter )
{
	/* 単純にそれぞれのパスポイントを描画する */
	nlCurve* path = getCurrentPath_();
	painter.setPen( QPen(Qt::red,g_pathPointRadius, Qt::SolidLine, Qt::RoundCap ) );
	if( path )
	{
		nlCurvePoints& points = path->points_;
        const int size = points.size();
        for(int i=0;i<size;++i)
        {
            nlCurvePoint& point = points[i];
			int frame, xyz;
            for( int xyzIdx=0;xyzIdx<3;++xyzIdx)
            {
                view_.localToScreenPos_( &frame, &xyz, point.time_, point.xyz_[xyzIdx] );
                painter.drawPoint( frame, xyz );
            }
		}
	}
}
/**/
void widget_curve::renderCurve_( QPainter& painter )
{
    const int penWidth = 2;
    QPen pens[]=
    {
        QPen(Qt::red,penWidth, Qt::DotLine ),    /* x */
        QPen(Qt::green,penWidth, Qt::DotLine ),  /* y */
        QPen(Qt::blue,penWidth, Qt::DotLine ),   /* z */
    };
    for( int xyzIdx=0;xyzIdx<3;++xyzIdx)
    {
        painter.setPen( pens[xyzIdx] );
        for( int wx=0;wx<width();++wx)
        {
            float lx,ly;
            nlVec3 xyz;
            view_.screenToLocalPos_( &lx, &ly, wx, 0);
            nlCurve* path = getCurrentPath_();
            xyz = nlGetCurveValue( *path, lx );
            int dumx, py;
            view_.localToScreenPos_( &dumx, &py, 0, xyz.e_[xyzIdx]);
            painter.drawPoint( wx, py );
        }
    }
}
/**/
void widget_curve::mousePressEvent(QMouseEvent* e )
{
	if( Qt::LeftButton == e->buttons() )
	{
		grabePointXYZ_t grabPoint = gpxyz_None;
		if(GetAsyncKeyState ('Z'))       { grabPoint = gpxyz_X; }
		else if(GetAsyncKeyState ('X'))  { grabPoint = gpxyz_Y; }
		else if(GetAsyncKeyState ('C'))  { grabPoint = gpxyz_Z; }
		/**/
		nlCurve* path = getCurrentPath_();
        hitTestPathPoint( e->x(), e->y(), &draggedPointIdx_, &currentXYZIdx_, grabPoint );
		nlCurvePoint* point;
		getCurvePonit_( NULL, &point, NULL );
		if( point )
		{
			pointFrameBeforeDrag_ = point->time_;
			pointVBeforeDrag_ = point->xyz_[currentXYZIdx_];
			dragStartX_ = e->x();
			dragStartY_ = e->y();
		}
        /* 点が無ければ視点移動 */
        else
        {
            view_.onMousePress( e->x(), e->y() );
        }
	}
	/* 再描画 */
	repaint();
}
/**/
void widget_curve::hitTestPathPoint( int x, int y, int* pathIdx, int* axyzIdx, grabePointXYZ_t grabXYZ )const
{
    *pathIdx = -1;
    *axyzIdx = -1;

	const nlCurve* path = getCurrentPath_();
	if( !path )
	{ 
        *pathIdx = -1;
        *axyzIdx  = -1;
        return ; 
    }

	int idx = 0;
	const nlCurvePoints& points = path->points_;
	for( idx=0;idx<points.size();++idx)
	{
		const nlCurvePoint& point = points[idx];
		int px, py;
        for( int xyzIdx=0;xyzIdx<3;++xyzIdx)
        {
            view_.localToScreenPos_( &px, &py, point.time_, point.xyz_[xyzIdx]);
            int dx = px - x;
            int dy = py - y;
            int lenSq = dx*dx+dy*dy;
            const int radius = g_pathPointRadius;
            if( lenSq < radius*radius )
            {
				/* グラブ設定も見てヒット判定 */
				if( (grabXYZ == gpxyz_None) ||
					(grabXYZ == gpxyz_X && xyzIdx == 0 ) ||
					(grabXYZ == gpxyz_Y && xyzIdx == 1 ) ||
					(grabXYZ == gpxyz_Z && xyzIdx == 2 ) )
				{
					*pathIdx = idx;
					*axyzIdx  = xyzIdx;
				}
            }
        }
	}
	return;
}
/**/
void widget_curve::mouseMoveEvent(QMouseEvent* e )
{
	nlCurvePoint* point;
	getCurvePonit_( NULL, &point, NULL );

    if( Qt::LeftButton == e->buttons() )
    {
        /* 点を移動させている場合 */
        if( point && Qt::LeftButton == e->buttons() )
        {
			const bool isGrid       = ( GetKeyState( VK_SHIFT ) < 0 );
			const bool isOnLyUpDown = ( GetKeyState( VK_CONTROL ) < 0 );
            movePoint_( e, isGrid, isOnLyUpDown );
            /* previewも本体も再描画してもらう */
            emit( onRepaintMainPreview() );
        }
        /* 視点移動している場合 */
        else
        {
            view_.onMouseMove( e->x(), e->y() );
        }
    }
	/* 再描画 */
	repaint();
}
/**/
void widget_curve::mouseReleaseEvent(QMouseEvent* e)
{
	nlCurvePoint* point;
	getCurvePonit_( NULL, &point, NULL );
    if( Qt::LeftButton == e->button() )
    {
        /* 点を移動させていた場合 */
        if( point  )
        {
			const bool isGrid = ( GetKeyState( VK_SHIFT ) < 0 );
			const bool isOnlyUpDown = ( GetKeyState( VK_CONTROL ) < 0 );
            movePoint_(e, isGrid,isOnlyUpDown);
            draggedPointIdx_ = -1;
        }
        /* 視点を移動させていた場合 */
        else
        {
            view_.onMouseRelease( e->x(), e->y() );
        }
    }
	/* 再描画 */
	repaint();
}
/**/
void widget_curve::mouseDoubleClickEvent(QMouseEvent* e)
{
	nlCurve* path = getCurrentPath_();
	if( !path )
	{ return ; }
	nlCurvePoints& points = path->points_;

	float lx, ly;
	view_.screenToLocalPos_( &lx, &ly, e->x(), e->y());
	/* ポイントがある場合は、 それを削除する */
    int pathPointIdx;
    int xyzIdx;
	hitTestPathPoint( e->x(), e->y(), &pathPointIdx, &xyzIdx );
	if( pathPointIdx != -1 )
	{
		if( pathPointIdx < points.size() )
		{
            points.erase( pathPointIdx );
			return ;
		}
	}
	/* ない場合は、そこに作る */
	else
	{
        for(int i=points.size()-1;i>=0;--i)
		{
			if( points[i].time_ < lx )
			{
                const nlCurvePoint& pre = points[i];
                const nlCurvePoint& cur = points[i+1];
                /* 前後の点の補間位置に挿入 */
                const float d1 = (int)pre.time_ - lx;
                const float d2 = lx - (int)cur.time_;
                const float factor = d1/(d1+d2);
                const float x = (cur.xyz_[0]*factor + pre.xyz_[0]*(1.0f-factor));
                const float y = (cur.xyz_[1]*factor + pre.xyz_[1]*(1.0f-factor));
                const float z = (cur.xyz_[2]*factor + pre.xyz_[2]*(1.0f-factor));
				nlCurvePoint newPoint;
                newPoint.time_ = lx;
                newPoint.xyz_[0] = x;
                newPoint.xyz_[1] = y;
                newPoint.xyz_[2] = z;
				points.insert( i, newPoint );
				return ;
			}
		}
	}
}
/**/
void widget_curve::keyPressEvent(QKeyEvent* e)
{
    /* 保存 */
    if( (e->key() == Qt::Key_S)  && (e->modifiers() & Qt::ControlModifier ) )
    {
        if( MessageBoxA( NULL, "現在のカーブを保存しますか？", "確認", MB_YESNO ) == IDYES )
        {
            const optional<QString> tag = getTag( "CURVE_NAME",curPathIdx_ );
            if( tag )
            {
                nlCurve* path = getCurrentPath_();
                QString fullPath = sandboxPath( SP_PATH ) + *tag + ".txt";
                QByteArray fullPathBA = fullPath.toLocal8Bit();
                serializeCurve( fullPathBA.data(), *path, nlCST_KDF );
            }
        }
	}

	/* スペースキーでタイムラインの再生を切り替えられる */
	if( e->key() == Qt::Key_Space )
	{
        bool isLoopScene = (Qt::ShiftModifier&e->modifiers());
        ui.dockTimeline->togglePlay( isLoopScene );
    }
	/* 矢印キーで3秒前後する */
	else if( (e->key() == Qt::Key_Right ) )
	{ui.dockTimeline->movePosition(3000);}
	else if( (e->key() == Qt::Key_Left ) )
    {ui.dockTimeline->movePosition(-3000);}
}
/**/
void widget_curve::onChangeCurrentCurve( int index )
{
	/**/
	curPathIdx_ = index;
	/* そのパスの設定をUIに反映する */
	nlCurve* path = getCurrentPath_();
    /* カーブタイプの表示更新 */
    switch( path->type_ )
    {
    case nlCurveType_Line:
		ui.radioCurveLiner->setChecked( true );
		ui.radioCurveCatmul->setChecked( false );
        break;
    case nlCurveType_CatmulRom:
        ui.radioCurveLiner->setChecked( false );
		ui.radioCurveCatmul->setChecked( true );
        break;
    }
	/* 再描画 */
	repaint();
}
/**/
void widget_curve::onResetCurve()
{
	/* パスの全ての点を0に整える*/
	nlCurve* curPath = getCurrentPath_();
	if( curPath )
	{
		const int numPoints = curPath->points_.size();
		for( int i=0;i<numPoints;++i)
		{
			curPath->points_[i].xyz_[0] = 0.0f;
			curPath->points_[i].xyz_[1] = 0.0f;
			curPath->points_[i].xyz_[2] = 0.0f;
		}
	}
	/* 視点位置を中心にする。 */
	view_.reset();
	/* 再描画 */
	repaint();
}
/**/
void widget_curve::onClickLiner( bool isCheak )
{
	nlCurve* path = getCurrentPath_();
	path->type_ = isCheak ? nlCurveType_Line : path->type_;
}
/**/
void widget_curve::onClickCatmul( bool isCheak )
{
	nlCurve* path = getCurrentPath_();
	path->type_ = isCheak ? nlCurveType_CatmulRom : path->type_;
}
/**/
void widget_curve::onChangeCurveMode( int idx )
{
    /* 0か1しかない */
    NL_ASSERT( idx <= 1 );

    /* パスのタイプを変化させる */
    nlCurve* path = getCurrentPath_();
    if( idx == 0 )
    {
        path->type_ = nlCurveType_Line;
    }
    else if( idx == 1 )
    {
        path->type_ = nlCurveType_CatmulRom;
    }

    /* 再描画命令を発生させる */
    repaint();
}

/**/
void widget_curve::movePoint_(QMouseEvent* e, bool isGrid, bool isOnlyUpdown )
{
	/* 左右の点を越えないようにする */
	nlCurvePoint* prevPoint;
	nlCurvePoint* curPoint;
	nlCurvePoint* nextPoint;
	getCurvePonit_( &prevPoint, &curPoint, &nextPoint );

	float leftRange  = 0.0f;;
	float rightRange = FLT_MAX;
	if( prevPoint )
	{ leftRange = prevPoint->time_; }

	if( nextPoint )
	{ rightRange = nextPoint->time_; }

	/* 最左端である場合は、x:0から動かない */
	if( !prevPoint )
	{ rightRange = leftRange = 0; }

	if( curPoint )
	{
		/* 上下制限がされていなければ、フレームも移動する */
		if( !isOnlyUpdown )
		{
			curPoint->time_ = pointFrameBeforeDrag_ + ((e->x() - dragStartX_)/view_.scale() );
			curPoint->time_ = max( min(curPoint->time_, rightRange ), leftRange );
		}
		curPoint->xyz_[currentXYZIdx_] = pointVBeforeDrag_ - ((e->y() - dragStartY_)/(view_.scale()*Y_SCALE) );

		/* 10でグリッド */
		if( isGrid )
		{
			curPoint->time_  = curPoint->time_/10*10;
			curPoint->xyz_[currentXYZIdx_] = (int)(curPoint->xyz_[currentXYZIdx_])/10*10;
		}
	}
}

/**/
void widget_curve::wheelEvent(QWheelEvent* e)
{
	view_.onMouseWheel( e->delta() );
	repaint();
}

/**/
int widget_curve::View::getLeft()const
{
	return x_;
}
/**/
int   widget_curve::View::getTop()const
{
	return y_;
}
/**/
int   widget_curve::View::getRight()const
{
	return 0;
}
/**/
int   widget_curve::View::getBottom()const
{
	return 0;
}
/**/
float widget_curve::View::scale()const
{
    return scale_;
}
/**/
void widget_curve::View::localToScreenPos_( int* ox, int* oy, float ix, float iy)const
{
    iy = -iy;

	*ox = ((float)(ix -x_))*scale();
	*oy = ((float)((iy*Y_SCALE) -y_))*scale();
}
/**/
void widget_curve::View::screenToLocalPos_( float* ox, float* oy, int ix, int iy)const
{
	*ox = (ix)  /scale() + x_;
	*oy = ((iy) /scale() + y_)/Y_SCALE;

    *oy = -*oy;
}
/**/
void widget_curve::View::onMousePress( int mx, int my)
{
	dragStartX_ = mx;
	dragStartY_ = my;
	viewXBeforeDragX_ = getLeft();
	viewXBeforeDragY_ = getTop();
}
/**/
void widget_curve::View::onMouseMove( int mx, int my)
{
	x_ = viewXBeforeDragX_ - ((mx - dragStartX_)/scale());
	y_ = (viewXBeforeDragY_ - ((my - dragStartY_)/scale()));
}
/**/
void widget_curve::View::onMouseRelease( int mx, int my)
{
}
/**/
void widget_curve::View::onMouseWheel( int mouseDelta )
{
	scale_ += mouseDelta/(120.0f*10.0f);
	scale_ = max(min(scale_,10.0f),0.2f);
}
/**/
void widget_curve::View::reset()
{
	//scale_ = 
	x_ = 0;
	y_ = 0;
}
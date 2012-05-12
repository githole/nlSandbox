#include "main/stdafx.h"
#include "NodeEditor.h"

using namespace std;

/**/
nodeEditor_ValveOut::nodeEditor_ValveOut(
    NodeEditor* graphWidget,
    nodeEditor_Node* srcNode,
    nlEdgeType edgeType )
    :graph_(graphWidget),
    boundingRect_(-EDGE_CIRCLE_SIZE/2,-EDGE_CIRCLE_SIZE/2,EDGE_CIRCLE_SIZE,EDGE_CIRCLE_SIZE),
    node_( srcNode ),
    targetValveIn_( NULL ),
    nowConnect_(false),
    edgeType_(edgeType)
{
    setFlag(ItemIsMovable);
    setFlag(ItemSendsGeometryChanges);
    setCacheMode(DeviceCoordinateCache);
    setZValue(0);
    /*  */
    updatePos();
    /* ドロップシャドウをつけるとなぜかドラッグができなくなるのでつけない */
    //setGraphicsEffect( createSceneDropShadowEffect() );
}
/* バウンディング矩形。描画のカリングに利用されている */
QRectF nodeEditor_ValveOut::boundingRect() const
{
    QPointF valvePos;
    QRectF result = boundingRectAndValvePos( &valvePos );
    return result;
}
/**/
QRectF nodeEditor_ValveOut::boundingRectAndValvePos( QPointF* valvePos ) const
{
    const QPointF positionInParentCoordinates = pos();
    const QPointF& p1 = node_->valveOutPosInSceneCord() - positionInParentCoordinates;
    const QPointF  p2(0.0f,0.0f);
    QRectF rectLine( min( p1.x(), p2.x() ), min( p1.y(), p2.y() ), abs( p1.x() - p2.x() ), abs( p1.y() - p2.y() ) );
    /* でるところの丸も含める。 */
    QRectF valveStartCircle( p1.x()-EDGE_CIRCLE_SIZE/2, p1.y()-EDGE_CIRCLE_SIZE/2, EDGE_CIRCLE_SIZE, EDGE_CIRCLE_SIZE );
    /* 合算 */
    QRectF resultRect = boundingRect_ | rectLine | valveStartCircle;  

    *valvePos = p1;

    return resultRect;
}
/* 形。マウスドラッグの判定に利用されている */
QPainterPath nodeEditor_ValveOut::shape() const
{
    QPainterPath path;
    path.addRect( boundingRect_ );
    return path;
}
/**/
void nodeEditor_ValveOut::paint(
                                QPainter* painter, 
                                const QStyleOptionGraphicsItem *option, 
                                QWidget* )
{
    /**/
    const float lod = option->levelOfDetailFromTransform(painter->worldTransform());
    const QColor valveColor = getValveColor(edgeType_);
    /**/   
    if( lod > 0.35f )
    {
        /* ドラッグ対象/ドラッグ元を描画 */
        QPainterPath path;
        path.setFillRule( Qt::WindingFill );
        path.addEllipse( boundingRect_ );
        QRectF srcNodeRect = boundingRect_;
        srcNodeRect.translate( (node_->valveOutPosInSceneCord( )-pos()) );
        path.addEllipse( srcNodeRect );
        /**/
        painter->fillPath( path, QBrush( valveColor ) );
    }

    /* 線の部分を描画。近かったらICE風の曲線にする */
    if( lod > 0.5f )
    {
        QPainterPath path;
        const QPointF start  = node_->valveOutPosInSceneCord()-pos();
        const QPointF end    = QPointF(0.0f,0.0f);
        const QPointF startC = QPointF( (start.x()+end.x())*0.5f, start.y() );
        const QPointF endC   = QPointF( (start.x()+end.x())*0.5f, end.y() );
        path.moveTo(start);
        path.cubicTo( startC, endC, end );
        painter->setPen( QPen(valveColor, 1.5f ) );
        painter->drawPath( path );
    }
    else
    {
        QLineF line( node_->valveOutPosInSceneCord()-pos(), QPointF(0.0f,0.0f) );
        painter->setPen( QPen( valveColor, EDGE_LINE_WIDTH, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin) );
        painter->drawLine( line );
    }
}
/**/
void nodeEditor_ValveOut::updatePos()
{
    /* 連結状態であれば、連結先の位置に固定する */
    if( targetValveIn_ )
    {
        setPos( targetValveIn_->pos() );
    }
    /* 非連結状態であれば、親の定位置に追従する */
    else
    {
        setPos( node_->valveOutPosInSceneCord( ) );
    }
}
/**/
QVariant nodeEditor_ValveOut::itemChange(GraphicsItemChange change, const QVariant &value)
{
    return QGraphicsItem::itemChange(change, value);
}
/**/
void nodeEditor_ValveOut::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    /* 既に接続されている場合はこのタイミングで外す */
    if( targetValveIn_ )
    {
        const unsigned int valveNo = targetValveIn_->valveNo();
        targetValveIn_->node()->onDisconnect( valveNo );
        targetValveIn_->setValveOut( NULL );
        targetValveIn_ = NULL;
        nowConnect_ = true;
    }
    /**/
    update();
    QGraphicsItem::mousePressEvent(event);
}
/**/
void nodeEditor_ValveOut::mouseReleaseEvent(QGraphicsSceneMouseEvent* e )
{
    /* 近くに連結可能なバルブがあった場合連結する */
    QPointF pos = e->scenePos();
    nodeEditor_ValveIn* valveIn = graph_->findValveIn( e->scenePos(), node_ );
    const bool isConnectable = 
        (valveIn != NULL) && 
        !(valveIn->valveOut()) && 
        valveIn->edgeType() == edgeType_;

    /* 接続する場合 */
    if( isConnectable )
    {
        targetValveIn_ = valveIn;
        targetValveIn_->setValveOut( this );
        updatePos();
        nowConnect_ = true;
        /**/
        const unsigned int nodeId = node_->nodeId();
        const unsigned int valveNo = targetValveIn_->valveNo();
        targetValveIn_->node()->onConnect( nodeId, valveNo );
    }
    /* なければ親の位置に戻す。同時に連結も外す */
    else
    {
        if( nowConnect_ )
        {
            nowConnect_ = false;
        }
        setPos( node_->valveOutPosInSceneCord( ) );
    }
    /**/
    update();
    QGraphicsItem::mouseReleaseEvent(e);    
}
#include "main/stdafx.h"
#include "NodeEditor.h"

#pragma warning(disable:4100) /* 参照されない引数 */
#pragma warning(disable:4244) /* 数値の変換 */

/**/
nodeEditor_ValveIn::nodeEditor_ValveIn(
                                       NodeEditor* graphWidget, 
                                       nodeEditor_Node* parentNode,
                                       int valveNo,
                                       nlEdgeType edgeType )
                                       :graph(graphWidget),
                                       boundingRect_(-EDGE_CIRCLE_SIZE/2,-EDGE_CIRCLE_SIZE/2,EDGE_CIRCLE_SIZE,EDGE_CIRCLE_SIZE),
                                       valveNo_( valveNo ),
                                       node_( parentNode ),
                                       valveOut_( NULL ),
                                       edgeType_(edgeType)
{
    //setFlag(ItemIsMovable);
    setFlag(ItemSendsGeometryChanges);
    setCacheMode(DeviceCoordinateCache);
    setZValue(0);
    /**/
    updatePos();
}
/* バウンディング矩形。描画のカリングに利用されている */
QRectF nodeEditor_ValveIn::boundingRect() const
{
    return boundingRect_;
}
/* 形。マウスドラッグの判定に利用されている */
QPainterPath nodeEditor_ValveIn::shape() const
{
    QPainterPath path;
    path.addRect( boundingRect_ );
    return path;
}
/**/
void nodeEditor_ValveIn::paint(
                               QPainter *painter, 
                               const QStyleOptionGraphicsItem* option, 
                               QWidget *widget)
{
    const float lod = option->levelOfDetailFromTransform(painter->worldTransform());
    const QColor valveColor = getValveColor(edgeType_);    
    /**/
    if( ( lod > 0.35f ) )
    {
        QPainterPath path;
        QRectF srcNodeRect = boundingRect_;
        srcNodeRect.translate( (node_->valveInPosInSceneCord( valveNo_ )-pos()) );
        path.addEllipse( srcNodeRect );
        painter->fillPath( path, QBrush( valveColor ) );
    }
}
/**/
void nodeEditor_ValveIn::updatePos()
{
    /* 常に所定の位置 */
    setPos( node_->valveInPosInSceneCord( valveNo_ ) );
    /* 連結されていたらそれも更新する */
    if( valveOut_ )
    {
        valveOut_->updatePos();
    }
}

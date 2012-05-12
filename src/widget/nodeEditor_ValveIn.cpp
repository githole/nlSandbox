#include "main/stdafx.h"
#include "NodeEditor.h"

#pragma warning(disable:4100) /* �Q�Ƃ���Ȃ����� */
#pragma warning(disable:4244) /* ���l�̕ϊ� */

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
/* �o�E���f�B���O��`�B�`��̃J�����O�ɗ��p����Ă��� */
QRectF nodeEditor_ValveIn::boundingRect() const
{
    return boundingRect_;
}
/* �`�B�}�E�X�h���b�O�̔���ɗ��p����Ă��� */
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
    /* ��ɏ���̈ʒu */
    setPos( node_->valveInPosInSceneCord( valveNo_ ) );
    /* �A������Ă����炻����X�V���� */
    if( valveOut_ )
    {
        valveOut_->updatePos();
    }
}

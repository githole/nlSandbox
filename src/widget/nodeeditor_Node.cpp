#include "main/stdafx.h"
#include "NodeEditor.h"
#include "tool/nlUtil.hpp"

using namespace std;

const float nodeWidth  = 75.0f;
const float nodeHeight = 100.0f;

/**/
nodeEditor_Node::nodeEditor_Node(
                                 NodeEditor* graph,
                                 QGraphicsScene* scene,
                                 const NodeFeature& nodeFuture,
                                 unsigned int sceneIndex,
                                 unsigned int sceneMapIdx )
    :graph_(graph),
    boundingRect_(-nodeWidth/2,-nodeHeight/2,nodeWidth,nodeHeight),
    valveOuts_( NULL ),
    nodeNameText_( nodeFuture.nodeName_.c_str() ),
    nodeFeature_( nodeFuture ),
    sceneIndex_(sceneIndex),
    sceneMapIdx_(sceneMapIdx)
{
    setFlag(ItemIsMovable);
    setFlag(ItemSendsGeometryChanges);
    setCacheMode(DeviceCoordinateCache);
    setZValue(-1);
    /**/
	/* �m�[�h�̃T�C�Y����̓m�[�h���ɂ���ĕω������� */
	const int numValveOut = nodeFuture.inputValveProps_.size();
	const int brHeight = numValveOut==0?35:numValveOut*15+20;
	boundingRect_ = QRectF(-nodeWidth/2,-nodeHeight/2,nodeWidth, brHeight );
    roundRect_           = roundRectPath( boundingRect_, 10.0f );
    /* �o�̓o���u�̍쐬 */
    nodeEditor_ValveOut* outValve = new nodeEditor_ValveOut(graph, this, nodeFuture.outEdgeType_ );
    scene->addItem( outValve );
    valveOuts_.push_back( outValve );
    /* ���̓o���u�̍쐬 */
    for( int valveNo=0;valveNo<numValveOut;++valveNo )
    {
        nodeEditor_ValveIn* valveIn = new nodeEditor_ValveIn(graph, this, valveNo, nodeFuture.inputValveProps_[valveNo].edgeType_ );
        scene->addItem( valveIn );
        valveInList_.push_back( valveIn );
    }
    /**/
    for( int i=0;i<numValveOut;++i)
    {
        inputValveTexts_[i] = QStaticText( nodeFuture.inputValveProps_[i].propName_.c_str() );
    }

    /* TODO ��œW�J���� */
    setId(sceneMapIdx);

    /**/
    if( configFileValue("config","usedrop").toBool() )
    {
        setGraphicsEffect( new QGraphicsDropShadowEffect() );
    }
}
/**/
nodeEditor_Node::~nodeEditor_Node()
{
    /**
       ����͂��Ȃ��Ă����Bdelete nodeEditor_Node;�̓A�v���R�[�h����Ă΂ꂸ�A
       scene()->removeItem( node );���Ăяo���������炱�̃f�X�g���N�^���Ăяo����邽��
     */
    //graphScene->removeItem( this );
}
/**/
void nodeEditor_Node::finalize()
{
    /* �O��̘A���֘A�����Z�b�g���� */
    std::vector<nodeEditor_ValveOut*> voList = valveOutList();
    for( int i=0;i<voList.size();++i)
    {
        nodeEditor_ValveIn* valveIn = voList[i]->valveIn();
        if( valveIn )
        {
            valveIn->setValveOut( NULL ); 
        }
    }
    for( int i=0;i<valveInList_.size();++i)
    {
        nodeEditor_ValveOut* valveOut = valveInList_[i]->valveOut();
        if( valveOut )
        {
            valveOut->setValveIn( NULL );
            valveOut->updatePos();
        }
    }

    /* ValveInOut���폜���Ă��� */
    QGraphicsScene* graphScene = graph_->scene();
    for( int i=0;i<valveOuts_.size();++i)
    {
        graphScene->removeItem( valveOuts_[i] );
    }
    valveOuts_.clear();
    /* ValveIn���폜���� */
    for( int i=0;i<valveInList_.size();++i)
    {
        graphScene->removeItem( valveInList_[i] );
    }
    valveInList_.clear();

    /* �V�[���}�b�v�̊e���ӏ������������� */
    SceneArray& scenes = nlScene_getSceneData();
    nlScene& scene = scenes[sceneIndex_];
    SceneMap& sceneMap = scene.sceneMaps_;
    nlSceneNode& sceneNode = sceneMap[sceneMapIdx_];
    sceneNode.type_ = SNT_INVALID;
    for( int i=0;i<SCENENODE_INPUT_NUM;++i){ sceneNode.prevNode_[i] = -1; }
    for( int i=0;i<SCENENODE_PROP_NUM;++i){ sceneNode.prop_[i] = 0.0; }
    sceneNode.numValve_ = 0;
    sceneNode.outEdgeType = NL_ET_None;
    /* �폜�����V�[���m�[�h���Q�Ƃ��Ă���m�[�h���C�� */
    for( int i=0;i<scene.numSceneMap_;++i)
    {
        for( int j=0;j<sceneMap[i].prevNode_.size();++j)
        {
            if( sceneMap[i].prevNode_[j] == sceneMapIdx_ )
            {
                sceneMap[i].prevNode_[j] = 0;
            }
        }
    }
    /* �ĕ`�� */
    graph_->redraw2();
}
/* �o�E���f�B���O��`�B�`��̃J�����O�ɗ��p����Ă��� */
QRectF nodeEditor_Node::boundingRect() const
{
    /* �h���b�v�V���h�E���l������ */
    QRectF pathRect = roundRect_.boundingRect();
    /* �o�̓o���u�̕� */
    for( int i=0;i<valveOuts_.size();++i )
    {
        nodeEditor_ValveOut* valveOut = valveOuts_[i];
        QPointF pointf;
        QRectF valveOutRect = valveOut->boundingRectAndValvePos( &pointf );
        valveOutRect.translate( -pointf.x(), -pointf.y() );
        valveOutRect.translate( valveOutPosInLocalCord() );
        pathRect |= valveOutRect;
    }
    return pathRect;
}
/* �`�B�}�E�X�h���b�O�̔���ɗ��p����Ă��� */
QPainterPath nodeEditor_Node::shape() const
{
    return roundRect_;
}
/**/
void nodeEditor_Node::paint(
                 QPainter *painter, 
                 const QStyleOptionGraphicsItem* option, 
                 QWidget* widget )
{
    /* ��͂̕`�� */
    const  bool isDraged = (option->state & QStyle::State_Sunken);

    const float lod = option->levelOfDetailFromTransform(painter->worldTransform());

    /* LOD�ɂ���āA�`��̊ȗ������s�� */
	QColor nodeColor( Qt::red );
	switch( nodeFuture().nodeType_ )
	{
	case NodeType_Unknown:    nodeColor = Qt::red;break;
	case NodeType_Math:       nodeColor = Qt::darkRed; break;
    case NodeType_Matrix:     nodeColor = Qt::darkRed; break;
	case NodeType_Array:      nodeColor = Qt::darkGreen;break;
	case NodeType_Render:     nodeColor = Qt::darkGray;break;
	case NodeType_Util:       nodeColor = Qt::darkCyan; break;
	case NodeType_Other:      nodeColor = Qt::red; break;
	case NodeType_Deprecated: nodeColor = Qt::red; break;
	default:                  nodeColor = Qt::red; break;
	}
    
    QBrush bokanBrush( nodeColor );
    if( lod > 0.7f )
    { painter->fillPath(roundRect_, bokanBrush ); }
    else
    { painter->fillRect( boundingRect_, bokanBrush ); }

    /* �m�[�h����`�� */
    if( lod > 0.70f )
    {
        painter->setPen(QPen(Qt::black));
        painter->drawStaticText( QPointF(-nodeNameText_.size().width()/2,-nodeHeight/2), nodeNameText_ );
        painter->drawStaticText( QPointF(-nodeNameText_.size().width()/2,-nodeHeight/2), idText_ );
    }
    /* �e�o���u����`�� */
    if( lod > 0.71f )
    {
        painter->setPen(QPen(Qt::black));
        for( int i=0; i<SCENENODE_INPUT_NUM; ++i )
        {
            QStaticText& text = inputValveTexts_[i];
            QPointF valvePos = valveInPosInLocalCord( i ) + QPointF( 5.0f, -7.0f );
            painter->drawStaticText( valvePos, text );
        }
    }
}
/**/
QPointF nodeEditor_Node::valveOutPosInSceneCord() const
{
    QPointF resultPos = valveOutPosInLocalCord() + pos();
    return resultPos;
}
/**/
QPointF nodeEditor_Node::valveOutPosInLocalCord() const
{
    /* HACK �l�͓K�� */
    return QPointF( nodeWidth/2, -30 );
}


/**/
void nodeEditor_Node::onConnect( unsigned int prevNodeIndex, unsigned int valveInIndex )
{
    /**/
    addNewValveOut();
    /* TODO �����̃m�[�h��Ԃ�ύX���� */
    printf("onConnect():prev:%d valvein:%d\n",prevNodeIndex,valveInIndex);
    /* TODO �V�[���}�b�v�̊e���ӏ������������� */
    SceneArray& scenes = nlScene_getSceneData();
    nlSceneNode& sceneNode = scenes[sceneIndex_].sceneMaps_[sceneMapIdx_];
    sceneNode.prevNode_[valveInIndex] = prevNodeIndex;
    /* �ĕ`�� */
    graph_->redraw2();
}
/**/
void nodeEditor_Node::addNewValveOut()
{
    nodeEditor_ValveOut* outValve = new nodeEditor_ValveOut(graph_, this, nodeFeature_.outEdgeType_ );
    scene()->addItem( outValve );
    valveOuts_.push_back( outValve );
}
/**/
void nodeEditor_Node::onDisconnect( unsigned int valveInIndex )
{
    /* TODO �e������valveOuts_��delete���� */
    printf("onDisconnect():valvein:%d\n",valveInIndex);
    /* TODO �V�[���}�b�v�̊e���ӏ������������� */
    SceneArray& scenes = nlScene_getSceneData();
    nlSceneNode& sceneNode = scenes[sceneIndex_].sceneMaps_[sceneMapIdx_];
    sceneNode.prevNode_[valveInIndex] = -1;
    /* �ĕ`�� */
    graph_->redraw2();
}
/**/
QPointF nodeEditor_Node::valveInPosInSceneCord( int valveNo ) const
{
    QPointF resultPos = valveInPosInLocalCord( valveNo ) + pos();
    return resultPos;
}
/**/
QPointF nodeEditor_Node::valveInPosInLocalCord( int valveNo ) const
{
    /* HACK �l�͓K�� */
    return QPointF( -nodeWidth/2, -30 + valveNo*15 );
}
/**/
QPainterPath nodeEditor_Node::roundRectPath(const QRectF &rect, float radius )
 {
     qreal x1, y1, x2, y2;
     rect.getCoords(&x1, &y1, &x2, &y2);
     const float diam = 2.0f*radius;
     const float r  = radius;

     QPainterPath path;
     path.moveTo(x1, y1 + r );
     path.arcTo( QRect( x1, y1, r, r ), 180.0f, -90.0f);//
     path.lineTo( x2-r, y1 );
     path.arcTo( QRect( x2-r, y1, r, r ), 90.0f, -90.0f);
     path.lineTo( x2, y2-r );
     path.arcTo( QRect( x2-r, y2-r, r, r ), 00.0f, -90.0f);//
     path.lineTo( x1+r, y2 );
     path.arcTo( QRect( x1, y2-r, r, r ), -90.0f, -90.0f);
     path.lineTo( x1, y1+r );
     return path;
 }
/**/
QVariant nodeEditor_Node::itemChange(GraphicsItemChange change, const QVariant &value)
{
    switch (change) 
    {
     case ItemPositionHasChanged:
         {
             for( int i=0;i<valveInList_.size();++i)
             {
                 valveInList_[i]->updatePos();
             }
             for( int i=0;i<valveOuts_.size();++i)
             {
                 nodeEditor_ValveOut* valveOut = valveOuts_[i];
                 valveOut->updatePos();
             }
         }
         graph_->itemMoved();
         break;
     default:
         break;
    };
    return QGraphicsItem::itemChange(change, value);
}
/**/
void nodeEditor_Node::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    update();
    QGraphicsItem::mousePressEvent(event);
}
/**/
void nodeEditor_Node::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    update();
    QGraphicsItem::mouseReleaseEvent(event);
}
#pragma once

#include "main/stdafx.h"

#include "shared/scene/nlScene.hpp"

class NodeEditor;
class nodeEditor_Node;
class nodeEditor_ValveIn;
class nodeEditor_ValveOut;

#pragma warning(disable:4800) /* boolへの強制変換 */
#pragma warning(disable:4245) /* signed/unsigned */
#pragma warning(disable:4018) /* signed/unsigned比較 */

const float dropShadowLen = 2.0f; 
const int EDGE_CIRCLE_SIZE = 8;
const int EDGE_LINE_WIDTH  = 2;


enum ValvePropType_t
{
    VPT_FLOAT,
    VPT_INT,
    VPT_STRING_TO_ID,
};
/**
 * @brief 
 */
enum NodeType
{
	/**/
	NodeType_Unknown = 0, 
	/* 算術系 */
	NodeType_Math,
    /* 算術系 */
	NodeType_Matrix,
	/* 配列操作系 */
	NodeType_Array, 
	/* 描画系 */
	NodeType_Render,
	/* 便利系 */
	NodeType_Util,
	/**/
	NodeType_Other,
	/**/
	NodeType_Deprecated,
	/**/
	NodeType_Max,
};

/* ノード設定 */
struct NodeFeature
{
    /**/
    std::string   nodeName_;
    std::string   description_;
	NodeType nodeType_;
    nlSceneNode_t nodeId_;
    /* 出力口の型 */
    nlEdgeType outEdgeType_;

    NodeFeature()
        :nodeName_("not set"),
		nodeType_(NodeType_Unknown),
        nodeId_(SNT_INVALID),
        outEdgeType_(NL_ET_Vec4)
    {}

    /* バルブ */
    struct ValveProp
    {
        /**/
        std::string propName_;
        nlEdgeType edgeType_;
        /**/
        ValveProp()
            :propName_("unknown"),
            edgeType_(NL_ET_Vec4)
        {}
        ValveProp(
            std::string propName )
            :propName_( propName ),
            edgeType_(NL_ET_Vec4)
        {}
        ValveProp(
            std::string propName,
            nlEdgeType edgeType )
            :propName_( propName ),
            edgeType_(edgeType)
        {}
    };
    std::vector<ValveProp> inputValveProps_;

    /* プロパティ */
    struct PropProp
    {
        std::string propName;
        /* 文字列からIDを指定するようなタイプのものか？ */
        ValvePropType_t type_;

        PropProp()
            :propName( "" ),
            type_( VPT_FLOAT )
        {}
        PropProp(
            std::string propName,
            ValvePropType_t type )
            :propName( propName ),
            type_( type )
        {}
    };
    std::vector<PropProp> props_;
};
/* NodeEditorへの設定値 */
struct NodeeditorboardDesc
{
    std::vector<NodeFeature> nodeFeatures_;
};

/* 外部から設定するノードごとに持つ値 */
struct NodeDataProp
{
    /* SCENENODE_PROP_NUMに準拠させる */
    float prop_[SCENENODE_PROP_NUM];
    unsigned int numValve_;
    bool isUse_;
    std::string comment_;
    std::string propStr_[SCENENODE_PROP_NUM];

    NodeDataProp()
        :numValve_(1),
        isUse_( true )
    {
		for(int i=0;i<(sizeof(prop_)/sizeof(*prop_)); ++i )
		{
			prop_[i]    = 0.0f;
			propStr_[i] = "";
		}
	}
};


/* 出力点 */
class nodeEditor_ValveOut
    :public QGraphicsItem
{
public:
    nodeEditor_ValveOut(
        NodeEditor *graphWidget,
        nodeEditor_Node* node,
        nlEdgeType edgeType );

    enum { Type = UserType + 1 };
    int type() const { return Type; }

    QRectF boundingRect() const;
    QRectF boundingRectAndValvePos( QPointF* valvePos ) const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void updatePos();

    bool isConnected()
    { return targetValveIn_; }

    void setValveIn( nodeEditor_ValveIn* valveIn )
    { targetValveIn_ = valveIn; }

    nodeEditor_Node* node()
    { return node_; }

    nodeEditor_ValveIn* valveIn()
    { return targetValveIn_; }

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
    QPointF newPos;
    NodeEditor* graph_;
    QRectF boundingRect_;
    nodeEditor_Node* node_;
    nodeEditor_ValveIn* targetValveIn_;
    nlEdgeType edgeType_;
    bool nowConnect_;
};

static QColor getValveColor( nlEdgeType edgeType )
{
    switch( edgeType )
    {
    case NL_ET_Matrix: return Qt::darkRed;
    case NL_ET_Vec4:   return Qt::darkYellow;
    case NL_ET_Render: return Qt::darkGray;
    case NL_ET_None:   return Qt::transparent;
    default: NL_ASSERT(!"");    return Qt::red;
    }
}

/* 入力点 */
class nodeEditor_ValveIn 
    :public QGraphicsItem
{
public:
    nodeEditor_ValveIn(
        NodeEditor* graphWidget, 
        nodeEditor_Node* parent,
        int valveNo,
        nlEdgeType edgeType );

    enum { Type = UserType + 2 };
    int type() const { return Type; }

    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void updatePos();

    nodeEditor_Node* node() const
    { return node_; }

    void setValveOut( nodeEditor_ValveOut* valveOut )
    { valveOut_ = valveOut; }

    nodeEditor_ValveOut* valveOut() const
    { return valveOut_; }

    nlEdgeType edgeType()const
    { return edgeType_; }

    unsigned int valveNo() const
    { return valveNo_; }

private:
    QPointF newPos;
    NodeEditor* graph;
    QRectF boundingRect_;
    const int valveNo_;
    nodeEditor_Node* node_;
    nodeEditor_ValveOut* valveOut_;
    nlEdgeType edgeType_;
};

/* ノード */
class nodeEditor_Node 
    :public QGraphicsItem
{
public:
    nodeEditor_Node(
        NodeEditor* graphWidget, 
        QGraphicsScene* scene,
        const NodeFeature& nodeFuture,
        unsigned int sceneIndex,
        unsigned int sceneMapIdx );

    ~nodeEditor_Node();

    void finalize();

    /**/
    void setProp( const NodeDataProp& newProp )
    {
        nodeProp_ = newProp;
    }
public:
    enum { Type = UserType + 3 };
    int type() const { return Type; }

    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    QPointF valveInPosInSceneCord( int valveNo ) const;
    QPointF valveInPosInLocalCord( int valveNo ) const;
    QPointF valveOutPosInSceneCord() const;
    QPointF valveOutPosInLocalCord() const;
    const NodeFeature& nodeFuture() const
    { return nodeFeature_; }
    NodeDataProp& nodeProp()
    { return nodeProp_; }
    const NodeDataProp& nodeProp() const
    { return nodeProp_; }

    nodeEditor_ValveOut* valveOut(
        unsigned int idx ) const
    { return valveOuts_[idx]; }

    std::vector<nodeEditor_ValveOut*>& valveOutList()
    { return valveOuts_; }

    std::vector<nodeEditor_ValveIn*>&  valveInList()
    { return valveInList_; }
    /*  */
    void removeValves();
    /* このノードのID(シーンマップ内でのindex)を返す */
    unsigned int nodeId() const
    { return sceneMapIdx_; }
    /*  */
    void onConnect( unsigned int prevNodeIndex, unsigned int valveInIndex );
    void addNewValveOut();
    void onDisconnect( unsigned int valveInIndex );
    /**/
    void setId( unsigned int id )
    {
        QString text = nodeFeature_.nodeName_.c_str();
        QString idStr;
        idStr.sprintf("[%d]",id);
        text += idStr;
        nodeNameText_ = QStaticText(text);
    }
protected:
    QPainterPath roundRectPath(const QRectF &rect, float radius );
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
    /* エディタ関係 */
    QPointF newPos;
    NodeEditor* graph_;
    QRectF boundingRect_;
    QPainterPath roundRect_;
    std::vector<nodeEditor_ValveOut*> valveOuts_; /* TODO smartptr化する */
    std::vector<nodeEditor_ValveIn*>  valveInList_;
    QStaticText nodeNameText_;
    QStaticText inputValveTexts_[SCENENODE_INPUT_NUM];
    QStaticText idText_;
    /* ノードそのもの関連 */
    NodeFeature  nodeFeature_;
    NodeDataProp nodeProp_;
    /**/
    const unsigned int sceneIndex_;
    const unsigned int sceneMapIdx_;
};

/* ノード情報をエクスポート/インポートする時に利用する情報 */
struct NodeExportInfo
{
    std::string nodeName_;     /* mul,addなどの種類を表す名前 */
    QPointF position_;         /* ノードの表示位置 */
    unsigned int prevNode_[SCENENODE_INPUT_NUM]; /* 前のノードインデックスを表す。-1は存在しない場合 */

    NodeDataProp prop_;
    NodeFeature future_; 

    NodeExportInfo()
    {
        for( int i=0;i<SCENENODE_INPUT_NUM;++i)
        {
            prevNode_[i] = 0xffffffff;
        }
    }
};
typedef std::vector<NodeExportInfo> NodeExportInfos;
/**/
struct ExportSceneInfo
{
    int totalTimeInMs;
    NodeExportInfos nodeInfo;
};
/* ノードの盤面 */
class NodeEditor 
    :public QGraphicsView
{
    Q_OBJECT
public:
    NodeEditor(QWidget* parent);
    void loadAlloScene();
    NodeFeature nodeNameToFuture( QString nodeName );
    void updateId( ExportSceneInfo& sceneInfo );
    void initialize();
    void finalize();
    ~NodeEditor();
    void itemMoved();

    nodeEditor_ValveIn* findValveIn( 
        QPointF pos,
        const nodeEditor_Node* excludoNode );

    /* ノードのアドレスからそのノードのインデックスを返す。この関数は非常に重い。 */
    unsigned int getNodeIndex( const nodeEditor_Node* node ) const;
    /* エクスポート/インポート */
    ExportSceneInfo exportNodeInfoFromUI();
    bool findFuture( 
        const NodeExportInfo&nodeInfo, 
        const std::vector<NodeFeature>& nodeFutures, 
        NodeFeature& retFuture );
    void importNodeInfo( const ExportSceneInfo& exportScene, unsigned int sceneIdx );
    void updateIds( const ExportSceneInfo& exportScene );
    /* ソートされていることを保証する。 */
    ExportSceneInfo exportNodeInfoFromMemory();
    /* シーンリストを取得する */
    std::vector<ExportSceneInfo> sceneInfoList()const;

    /**/
    void redraw2()
    { emit(redraw()); }

protected:
    /**
     * @brief 
     */
    virtual NodeeditorboardDesc nodeTypeList()const = 0;
    /**
     * @brief 
     */
    virtual void onAddNode() = 0;
    /**
     * @brief 
     */
    virtual void onConnectNode() = 0;
    /**
     * @brief 
     */
    virtual void onDisconnectNode() = 0;
    /**
     * @brief 
     */
    virtual void onRemoveNode() = 0;
    /**
     * @brief 
     */
    virtual void onForcusNode() = 0;
    /**
     * @brief 
     */
    virtual void onDisforcusNode() = 0;
    /**
     * @brief 
     */
    virtual void onClick( boost::optional<int> clickedNode ) = 0;
    /**
     * @brief 
     */
    virtual void onDoubleClick( boost::optional<int> clickedNode ) = 0;
    

public slots:
    /* ノードを追加する */
    void rightClickMenuTriggerd(int menuIdx, QPointF scenePos );
    /**/
    void deleteNodeAt( QPointF scenePos );
    void onSceneEditValveNumChange(int);
    void onSceneEditProp0Change(double); /* 各プロパティが変化したときに呼ばれる */
    void onSceneEditProp1Change(double);
    void onSceneEditProp2Change(double);
    void onSceneEditProp3Change(double);
    void onSceneEditProp4Change(double);
	void onSceneEditProp5Change(double);
    /**/
    void saveCurrentScene();
private:
    void onSceneEditPropChange( double value, unsigned int index );
public slots:
    void onProp0StringChanged(QString newText);
    void onProp1StringChanged(QString newText);
    void onProp2StringChanged(QString newText);
    void onProp3StringChanged(QString newText);
    void onProp4StringChanged(QString newText);
private:
    void onPropStringChanged( QString newTextQStr, unsigned int index, QDoubleSpinBox* target );
public slots:
	void onChangeSceneTimeInMs(int timeInMs );
    void onSceneEditIsUseThisNodeChange( int isChk );
    void onSceneEditCommentChanged();
    void onChangeCurrentScene( int sceneIndex );
    /**/
    void writeSceneToMemory();
signals:
    void redraw();
signals:
    void chagenSceneEditValuveNum(int);

    void chagenProp0Label(QString);
    void chagenProp1Label(QString);
    void chagenProp2Label(QString);
    void chagenProp3Label(QString);
    void chagenProp4Label(QString);

    void changeIsThisNodeEnable( bool isChk );

protected:
    void keyPressEvent(QKeyEvent *event);
    void updateNodePropDisplay_( QGraphicsItem* item );
    void mousePressEvent( QMouseEvent* e );
    void mouseMoveEvent( QMouseEvent* e );
    void mouseReleaseEvent ( QMouseEvent* e );
    void wheelEvent(QWheelEvent *event);
    void mouseDoubleClickEvent ( QMouseEvent* e );
    void drawBackground(QPainter *painter, const QRectF &rect);

private:
    NodeeditorboardDesc desc_;
    nodeEditor_Node* currentChoiseNode_;
    QString currentSceneName_;
    /* 現在コピーしているノードの情報 */
    struct NodeCopyData
    {
        NodeFeature future_;
        NodeDataProp prop_;
        QPointF relPosition_;
        QPointF position_;
        /**/
        NodeCopyData(
            const NodeFeature& future,
            const NodeDataProp& prop,
            const QPointF& position )
            :future_(future),
            prop_(prop),
            position_(position)
        {
        }
    };
    //typedef std::pair<NodeFeature,NodeDataProp> NodeCopyData;
    typedef std::vector<NodeCopyData> NodeCopydatas;
    NodeCopydatas nodeCopydatas;

    /* シーン名のリスト */
    std::vector<QFileInfo> sceneFiles;
    /**/
    int currentSceneIndex_;
    /**/
    typedef std::map<QString,ExportSceneInfo> LoadedSceneInfo;
    LoadedSceneInfo loadedSceneInfo_;
    QPoint selectStart_;
};

/**/
class RightClickAction
    :public QAction
{
    Q_OBJECT
public:
    RightClickAction( 
        const char* tag, 
        QObject* parent, 
        int idx,
		NodeType nodeType )
        :idx_(idx),
        QAction( tag, parent ),
		nodeType_(nodeType)
    {}
    void setScenePos(
        QPointF scenePos )
    {
        scenePos_ = scenePos;
    }
	NodeType nodeType()const
	{
		return nodeType_;
	}
public slots:
    void origTriggered()
    {
        emit(clickTrigger(idx_, scenePos_ ));
    }
signals:
    void clickTrigger(int index, QPointF scenePos );
private:
    /* 生成メニューの番号。*/
    int idx_;
    QPointF scenePos_;
	NodeType nodeType_;
};
/* 右クリックの削除アクション */
class RightClickDeleteAction
    :public QAction
{
    Q_OBJECT
public:
    RightClickDeleteAction( 
        const char* tag, 
        QObject* parent )
        :QAction( tag, parent )
    {}
    void setScenePos(
        QPointF scenePos )
    {
        scenePos_ = scenePos;
    }
    public slots:
        void origTriggered()
        {
            emit(clickTrigger( scenePos_ ));
        }
signals:
        void clickTrigger( QPointF scenePos );
private:
    QPointF scenePos_;
};
/**/
class NodeEditor_scene
    :public QGraphicsScene
{
public:
    NodeEditor_scene( 
        QObject* parent,
        std::vector<RightClickAction*> actions,
        RightClickDeleteAction* delAction )
        :QGraphicsScene( parent ),
        contextMenuActions_( actions ),
        delAction_( delAction )
    {}
    /**/
    void contextMenuEvent(
        QGraphicsSceneContextMenuEvent* e );
private:
    std::vector<RightClickAction*> contextMenuActions_;
    RightClickDeleteAction* delAction_;
};



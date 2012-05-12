#include "main/stdafx.h"
#include "NodeEditor.h"
#include "nlsandbox.h"
#include "shared/nlLog.hpp"
#include "tool/nlId.hpp"
#include "tool/nlUtil.hpp"
#include "tool/nlLogComposer.hpp"


using namespace std;
using namespace boost;

const float sceneSize = 10000;
static void sortNodes( NodeExportInfos& nodes );

/**/
template<class Archive> 
inline void serialize( 
                      Archive & ar,
                      ExportSceneInfo& value,  
                      const unsigned int ver )
{
    if( ver == 0 )
    {
        ar & boost::serialization::make_nvp("SceneFrame", value.totalTimeInMs);
        ar & boost::serialization::make_nvp("NodeInfo", value.nodeInfo);
    }
    else if( ver == 1 )
    {
        ar & boost::serialization::make_nvp("SceneTotalTime", value.totalTimeInMs);
        ar & boost::serialization::make_nvp("NodeInfo", value.nodeInfo);
    }
}
BOOST_CLASS_VERSION(ExportSceneInfo, 1);

template<class Archive> 
inline void serialize( 
                      Archive & ar,
                      NodeDataProp& value,  
                      const unsigned int ver )
{
	if( ver == 0 )
    {
        ar & boost::serialization::make_nvp("Prop0", value.prop_[0]);
        ar & boost::serialization::make_nvp("Prop1", value.prop_[1]);
        ar & boost::serialization::make_nvp("Prop2", value.prop_[2]);
        ar & boost::serialization::make_nvp("Prop3", value.prop_[3]);
        ar & boost::serialization::make_nvp("Prop4", value.prop_[4]);
        ar & boost::serialization::make_nvp("Prop5", value.prop_[5]);
        ar & boost::serialization::make_nvp("NumValve", value.numValve_);
        ar & boost::serialization::make_nvp("PropStr0", value.propStr_[0] );
        ar & boost::serialization::make_nvp("PropStr1", value.propStr_[1] );
        ar & boost::serialization::make_nvp("PropStr2", value.propStr_[2] );
        ar & boost::serialization::make_nvp("PropStr3", value.propStr_[3] );
        ar & boost::serialization::make_nvp("PropStr4", value.propStr_[4] );
        ar & boost::serialization::make_nvp("PropStr5", value.propStr_[5] );
        ar & boost::serialization::make_nvp("IsUse", value.isUse_ );
        ar & boost::serialization::make_nvp("Comment", value.comment_ );
    }
}
BOOST_CLASS_VERSION(NodeDataProp, 0);

template<class Archive> 
inline void serialize( 
                      Archive & ar,
                      QPointF& value,  
                      const unsigned int ver )
{
    if( ver == 0 )
    {
        ar & boost::serialization::make_nvp("x", value.rx() );
        ar & boost::serialization::make_nvp("y", value.ry() );
    }
}
BOOST_CLASS_VERSION(QPointF, 0);

template<class Archive> 
inline void serialize( 
                      Archive & ar,
                      NodeExportInfo& value,  
                      const unsigned int ver )
{
    if( ver == 0 )
    {
        ar & boost::serialization::make_nvp("NodeName", value.nodeName_ );
        ar & boost::serialization::make_nvp("Position", value.position_ );
        ar & boost::serialization::make_nvp("PrevNode0", value.prevNode_[0] );
        ar & boost::serialization::make_nvp("PrevNode1", value.prevNode_[1] );
        ar & boost::serialization::make_nvp("PrevNode2", value.prevNode_[2] );
        ar & boost::serialization::make_nvp("PrevNode3", value.prevNode_[3] );
        ar & boost::serialization::make_nvp("PrevNode4", value.prevNode_[4] );
        ar & boost::serialization::make_nvp("PrevNode5", value.prevNode_[5] );
        ar & boost::serialization::make_nvp("PrevNode6", value.prevNode_[6] );
        ar & boost::serialization::make_nvp("PrevNode7", value.prevNode_[7] );
        ar & boost::serialization::make_nvp("Prop", value.prop_ );
    }
}
BOOST_CLASS_VERSION(NodeExportInfo, 0);

/**/
NodeEditor::NodeEditor(
    QWidget* parent )
    :QGraphicsView( parent ),
    currentChoiseNode_( NULL ),
    currentSceneIndex_(0)
{
    /* シーンファイルのリストを作成する */
    QDir root(sandboxPath( SP_SCENE ));
    BOOST_FOREACH(const QFileInfo& info , root.entryInfoList() )
    {
        if( info.isFile() && info.suffix() == "xml" )
        {
            sceneFiles.push_back(info);
        }
    }
}
/**/
void NodeEditor::loadAlloScene()
{
    QDir root(sandboxPath( SP_SCENE ));
    /* 全てのシーンファイルをロードする */
    BOOST_FOREACH(const QFileInfo& info , root.entryInfoList() )
    {
        if( !info.isFile() || (info.suffix() != "xml") )
        { continue; }
        /**/
        ExportSceneInfo exportInfo = {};
        exportInfo.totalTimeInMs = MIN_SCENE_TIME; 
        /**/
        QString fileName = info.absoluteFilePath();
        QByteArray fileNameBA = fileName.toLocal8Bit();
        /**/
        ifstream file( fileNameBA.data() );
        try
        {
            /* ノードデータとシーン時間をロード */
            boost::archive::xml_iarchive iarchive(file);
            iarchive >> boost::serialization::make_nvp("Root", exportInfo);
        }
        catch( boost::archive::archive_exception& expception )
        { /* 単にファイルがない場合なので、無視 */ }
        /**/
        NL_LOG( INF_005, exportInfo.nodeInfo.size() );
        /* ソート */
        sortNodes(exportInfo.nodeInfo);
        /**/
        updateId(exportInfo);
        /**/
        loadedSceneInfo_[info.baseName()] = exportInfo;
    }
    /* コンボボックスに登録 */
    /* 全てのシーンファイルをロードする */
    BOOST_FOREACH(const QFileInfo& info , root.entryInfoList() )
    {
        if( !info.isFile() || (info.suffix() != "xml") )
        { continue; }
        ui.comboBox_scene->addItem(info.baseName());
    }
}
/**/
NodeFeature NodeEditor::nodeNameToFuture( QString nodeName )
{
    const vector<NodeFeature>& nodeFutures = desc_.nodeFeatures_;
    for( int i=0;i<nodeFutures.size();++i)
    {
        QByteArray tmp = nodeName.toLocal8Bit();
        if( !strcmp( nodeFutures[i].nodeName_.c_str(), tmp.data() ) )
        {
            return nodeFutures[i];
        }
    }
    NodeFeature empty;
    return empty;
}
/**/
void NodeEditor::updateId( ExportSceneInfo& sceneInfo )
{
    /* そのシーンが持つノードでプロパティがstring->IDのものがあった場合、そのstringの正しいIDに変換する */
    NodeExportInfos& infos = sceneInfo.nodeInfo;
    BOOST_FOREACH( NodeExportInfo& info, infos)
    {
        info.future_ = nodeNameToFuture( info.nodeName_.c_str() );
        const vector<NodeFeature::PropProp>& props = info.future_.props_;
        NodeDataProp& prop = info.prop_;
        NodeFeature& future = info.future_;
        for( int i=0;i<props.size(); ++i )
        {
            if( props[i].type_ == VPT_STRING_TO_ID )
            {
                string propGroup = future.props_[i].propName;
                string propStr = prop.propStr_[i];
                optional<int> newId = getId( propGroup.c_str(), propStr.c_str() );
                if( newId && newId !=  (int)prop.prop_[i] )
                {
                    NL_LOG( WAR_000,
                        props[i].propName.c_str(),
                        (int)prop.prop_[i],
                        *newId);
                    prop.prop_[i] = *newId;
                }
            }
        }
    }   
}
/**/
void NodeEditor::initialize()
{
    const NodeeditorboardDesc desc2 = nodeTypeList();
    /* NodeeditorboardDescのチェック */
    for( unsigned int i=0;i<desc2.nodeFeatures_.size();++i)
    {
        const NodeFeature& nodeFuture = desc2.nodeFeatures_[i];
        NL_ASSERT( nodeFuture.inputValveProps_.size() <= SCENENODE_INPUT_NUM );
        NL_ASSERT( nodeFuture.props_.size() <= SCENENODE_PROP_NUM );
    }
	/**/
    desc_ = desc2;
	/**/
	class Sort2
	{
	public:
		bool operator()( const NodeFeature& lhs, const NodeFeature& rhs )const
		{ 
			return lhs.nodeName_ < rhs.nodeName_; 
		}
	};
	std::sort( desc_.nodeFeatures_.begin(), desc_.nodeFeatures_.end(), Sort2() );
    /**/
    loadAlloScene();
    /* 右クリックメニューを作成する */
    const vector<NodeFeature>& nfs = desc_.nodeFeatures_;
    std::vector<RightClickAction*> contextMenuActions;
    for( int i=0;i<nfs.size();++i )
    {
        RightClickAction* action = new RightClickAction( nfs[i].nodeName_.c_str(), this, i, nfs[i].nodeType_ );
        connect(action, SIGNAL(triggered()), action, SLOT(origTriggered()));
        connect(action, SIGNAL(clickTrigger(int,QPointF)), this, SLOT(rightClickMenuTriggerd(int,QPointF)));
        contextMenuActions.push_back( action );
    }
    RightClickDeleteAction* delAction = new RightClickDeleteAction( "delete", this );
    connect(delAction, SIGNAL(triggered()), delAction, SLOT(origTriggered()));
    connect(delAction, SIGNAL(clickTrigger(QPointF)), this, SLOT(deleteNodeAt(QPointF)));
    /**/
    NodeEditor_scene* thisScene = new NodeEditor_scene(this, contextMenuActions, delAction );
    thisScene->setItemIndexMethod(QGraphicsScene::NoIndex);
    thisScene->setSceneRect(-sceneSize/2, -sceneSize/2, sceneSize, sceneSize);
    setScene(thisScene);
    setCacheMode(CacheBackground);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);

    /* コネクト */
    {
        connect( ui.sceneEditValveNum, SIGNAL( valueChanged(int) ), this, SLOT(onSceneEditValveNumChange(int)) );
        connect( ui.sceneEditProp0Float, SIGNAL( valueChanged(double) ), this, SLOT(onSceneEditProp0Change(double)) );
        connect( ui.sceneEditProp1Float, SIGNAL( valueChanged(double) ), this, SLOT(onSceneEditProp1Change(double)) );
        connect( ui.sceneEditProp2Float, SIGNAL( valueChanged(double) ), this, SLOT(onSceneEditProp2Change(double)) );
        connect( ui.sceneEditProp3Float, SIGNAL( valueChanged(double) ), this, SLOT(onSceneEditProp3Change(double)) );
        connect( ui.sceneEditProp4Float, SIGNAL( valueChanged(double) ), this, SLOT(onSceneEditProp4Change(double)) );
        /**/
        connect( ui.sceneEditProp0Combo, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(onProp0StringChanged(QString)) );
        connect( ui.sceneEditProp1Combo, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(onProp1StringChanged(QString)) );
        connect( ui.sceneEditProp2Combo, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(onProp2StringChanged(QString)) );
        connect( ui.sceneEditProp3Combo, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(onProp3StringChanged(QString)) );
        connect( ui.sceneEditProp4Combo, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(onProp4StringChanged(QString)) );
        /**/
		connect( ui.sceneTotalTime, SIGNAL(valueChanged (int)), this, SLOT(onChangeSceneTimeInMs(int)) );
        connect( ui.chkStopThisNode, SIGNAL(stateChanged(int)), this, SLOT(onSceneEditIsUseThisNodeChange(int)) );
        connect( ui.textComment, SIGNAL(textChanged()), this, SLOT(onSceneEditCommentChanged()) );
        connect( this, SIGNAL( chagenSceneEditValuveNum(int) ), ui.sceneEditValveNum, SLOT(setValue(int)) );
        connect( this, SIGNAL( chagenProp0Label(QString) ), ui.sceneEditPropLabel0, SLOT(setText(QString)) );
        connect( this, SIGNAL( chagenProp1Label(QString) ), ui.sceneEditPropLabel1, SLOT(setText(QString)) );
        connect( this, SIGNAL( chagenProp2Label(QString) ), ui.sceneEditPropLabel2, SLOT(setText(QString)) );
        connect( this, SIGNAL( chagenProp3Label(QString) ), ui.sceneEditPropLabel3, SLOT(setText(QString)) );
        connect( this, SIGNAL( chagenProp4Label(QString) ), ui.sceneEditPropLabel4, SLOT(setText(QString)) );
        /* 再描画を行う */
        connect( this,SIGNAL(redraw()),ui.previewWidget,SLOT(Rendering()));
    }
}
/**/
void NodeEditor::rightClickMenuTriggerd( int menuIdx, QPointF scenePos )
{
    /* 各当するシーンノードを算出する */
    SceneArray& sceneArray = nlScene_getSceneData();
    /* 最初にSNT_INVALIDになったものを割り当てる */
    SceneMap& sceneMap = sceneArray[currentSceneIndex_].sceneMaps_;
    int sceneMapIdx = 0;
    for(sceneMapIdx=0;sceneMapIdx<sceneMap.size();++sceneMapIdx)
    {
        if( sceneMap[sceneMapIdx].type_ == SNT_INVALID )
        {break;}
    }
    /**/
    const vector<NodeFeature>& nodeFutures = desc_.nodeFeatures_;
    NL_ASSERT( (0<=menuIdx) && (menuIdx<nodeFutures.size()) );
    nodeEditor_Node* newNode = new nodeEditor_Node(this,scene(), nodeFutures[menuIdx], currentSceneIndex_, sceneMapIdx );
    newNode->setPos( scenePos );
    scene()->addItem( newNode );
    /* nodeEditor_Nodeと同じTypeを書き込む */
    SceneArray& scenes = nlScene_getSceneData();
    nlSceneNode& sceneNode = scenes[currentSceneIndex_].sceneMaps_[sceneMapIdx];
    sceneNode.type_ = nodeFutures[menuIdx].nodeId_;
    for( int i=0;i<SCENENODE_INPUT_NUM;++i){ sceneNode.prevNode_[i] = 0; }
    for( int i=0;i<SCENENODE_PROP_NUM;++i){  sceneNode.prop_[i] = 0.0f; }
    sceneNode.numValve_ = 1;
    sceneNode.outEdgeType = nodeFutures[menuIdx].outEdgeType_;
    ++scenes[currentSceneIndex_].numSceneMap_;
	/* 再描画 */
    emit(redraw());
}
/**/
void NodeEditor::deleteNodeAt( QPointF scenePosF )
{
    /* 指定箇所のノードを削除する */
    nodeEditor_Node* node = dynamic_cast<nodeEditor_Node*>( scene()->itemAt( scenePosF ) );
    NL_ASSERT( node );
    /* removeItemを呼んだ直後には削除されないので、明示的に終了処理 */
    node->finalize();
    scene()->removeItem( node );
    /* 削除時の処理 */
    onRemoveNode();
	/* 再描画 */
    emit(redraw());
}
/**/
void NodeEditor::finalize()
{
#if 0 /* オートセーブ封印 */
    /* 現在の値は取得しておく */
    loadedSceneInfo_[currentSceneName_] = exportNodeInfoFromUI();
    /* 全シーンを保存 */
    LoadedSceneInfo::iterator ite = loadedSceneInfo_.begin();
    while( ite != loadedSceneInfo_.end() )
    {
        /**/
        QString filePath = sandboxPath(SP_SCENE)+ite->first+".xml";
        QByteArray fileNameBA = filePath.toLocal8Bit();
        std::ofstream file( fileNameBA.data() );
        boost::archive::xml_oarchive oarchive(file);
        oarchive << boost::serialization::make_nvp("Root", ite->second);
        /**/
        ++ite;
    }
#endif
}
/**/
NodeEditor::~NodeEditor()
{
}
/**/
void NodeEditor::itemMoved()
{
}
/**/
nodeEditor_ValveIn* NodeEditor::findValveIn( 
    QPointF scenePos, 
    const nodeEditor_Node* excludoNode )
{
    /* TODO これをQtの探索関数を利用するようにする */
    /* 指定した箇所のノードのバルブインを探索する */
    nodeEditor_Node* node = NULL;
    QGraphicsScene* scene = this->scene();
    QList<QGraphicsItem*> items = scene->items();
    QList<QGraphicsItem*>::iterator ite = items.begin();
    while( ite != items.end() )
    {
        nodeEditor_ValveIn* valveIn = dynamic_cast<nodeEditor_ValveIn*>( *ite );
        if( valveIn )
        {
            nodeEditor_Node* node = valveIn->node();
            bool isSameParent = ( node == excludoNode );
            if( !isSameParent )
            {
                const QPointF dir = valveIn->scenePos() - scenePos;
                const float x = dir.x();
                const float y = dir.y();
                const float len = sqrtf( x*x+y*y );
                if( len < EDGE_CIRCLE_SIZE )
                {
                    return valveIn; 
                }
            }
        }
        ++ite;
    }    
    return NULL;
}
/**/
unsigned int NodeEditor::getNodeIndex( 
    const nodeEditor_Node* targetNode ) const
{
    /* 全てのノードを回収する */
    NodeEditor_scene* thisScene = static_cast<NodeEditor_scene*>( scene() );
    QList<QGraphicsItem*> grapItems = thisScene->items();
    QList<QGraphicsItem*>::iterator ite = grapItems.begin();
    unsigned int nodeCnt = 0;
    while( ite != grapItems.end() )
    {
        QGraphicsItem* graphItem = *ite;
        nodeEditor_Node* node = dynamic_cast<nodeEditor_Node*>(graphItem);
        if( node )
        {
            if( node == targetNode )
            {
                return nodeCnt;
            }
            ++nodeCnt;
        }
        ++ite;
    }
    return -1;
}
/**/
ExportSceneInfo NodeEditor::exportNodeInfoFromUI()
{
    /* 全てのノードを回収する */
    NodeEditor_scene* thisScene = static_cast<NodeEditor_scene*>( scene() );
    std::vector<nodeEditor_Node*> allNodes;
    QList<QGraphicsItem*> grapItems = thisScene->items();
    QList<QGraphicsItem*>::iterator ite = grapItems.begin();
    while( ite != grapItems.end() )
    {
        QGraphicsItem* graphItem = *ite;
        nodeEditor_Node* node = dynamic_cast<nodeEditor_Node*>(graphItem);
        if( node )
        { 
            allNodes.push_back( node );
        }
        ++ite;
    }

    /* ノードアドレスからノードのインデックスを直接出せるようにする */
    boost::unordered_map<nodeEditor_Node*,unsigned int> nodeIndexes;
    for(int i=0;i<allNodes.size();++i)
    {
        nodeIndexes[ allNodes[i] ] = i;
    }

    /* ノード情報を保存 */
    ExportSceneInfo exportSceneInfo;
    NodeExportInfos& exportInfos = exportSceneInfo.nodeInfo;
    for( int i=0;i<allNodes.size();++i )
    {
        NodeExportInfo nodeInfo;
        nodeInfo.position_ = allNodes[i]->pos();
        nodeInfo.nodeName_ = allNodes[i]->nodeFuture().nodeName_;
        nodeInfo.prop_     = allNodes[i]->nodeProp();
        nodeInfo.future_   = allNodes[i]->nodeFuture();

        /* ノードの繋がり情報を取得する */
        std::vector<nodeEditor_ValveIn*> valveIns = allNodes[i]->valveInList();
        for( int j=0;j<valveIns.size();++j)
        {
            nodeEditor_ValveIn* valveIn = valveIns[j];
            if( valveIn )
            {
                nodeEditor_ValveOut* valveOut = valveIn->valveOut();
                if( valveOut )
                {
                    NL_ASSERT( valveOut->node() );
                    nodeEditor_Node* node = valveOut->node();
                    unsigned int idx = nodeIndexes.at(node);
                    nodeInfo.prevNode_[j] = idx;
                }
            }
        }
        exportInfos.push_back( nodeInfo );
    }
    /* シーン時間を取得 */
    int sceneTotalTime = ui.sceneTotalTime->value();
    exportSceneInfo.totalTimeInMs = sceneTotalTime;
    /**/
    return exportSceneInfo;
}
/**/
bool NodeEditor::findFuture( const NodeExportInfo&nodeInfo, const vector<NodeFeature>& nodeFutures, NodeFeature& retFuture )
{
    for( int i=0;i<nodeFutures.size();++i)
    {
        const NodeFeature& future = nodeFutures[i];
        if( future.nodeName_ == nodeInfo.nodeName_ )
        {
            retFuture = future;
            return true;
        }
    }   
    NL_ERR( ERR_014, nodeInfo.nodeName_.c_str() );
    return false;
}
/**/
void NodeEditor::importNodeInfo( 
    const ExportSceneInfo& exportScene,
    unsigned int sceneIdx )
{
    /* TODO 全てのノードをクリアする */
    /* 各ノードを復元していく */
    const NodeExportInfos& nodeInfos = exportScene.nodeInfo;
    vector<nodeEditor_Node*> newNodes;
    for( int i=0;i<nodeInfos.size();++i)
    {
        const NodeExportInfo& nodeInfo = exportScene.nodeInfo[i];
        const vector<NodeFeature>& nodeFutures = desc_.nodeFeatures_;
        nodeEditor_Node* newNode = NULL;
        NodeFeature future;
        if( findFuture( nodeInfo, nodeFutures, future) )
        {
            newNode = new nodeEditor_Node(this,scene(), future, sceneIdx, i );
            NodeDataProp& prop = newNode->nodeProp();
            prop = nodeInfo.prop_;
        }
        else
        {
            NodeFeature nullFuture;
            nullFuture.nodeName_ = "Unkown";
            nullFuture.description_ = "";
            nullFuture.nodeType_ = NodeType_Deprecated;
            nullFuture.nodeId_ = SNT_INVALID;
            nullFuture.outEdgeType_ = NL_ET_None;
            newNode = new nodeEditor_Node(this,scene(), nullFuture, sceneIdx, i  );
            NodeDataProp& prop = newNode->nodeProp();
            prop = nodeInfo.prop_;
        }
        newNode->setPos( nodeInfo.position_ );
        scene()->addItem( newNode );
        newNodes.push_back( newNode );
    }

    /* 連結を復元する */
    for( int i=0;i<nodeInfos.size();++i)
    {
        const NodeExportInfo& nodeInfo = nodeInfos[i];
        std::vector<nodeEditor_ValveIn*>& viList = newNodes[i]->valveInList();

        for( int j=0;j<viList.size();++j)
        {
            unsigned int prevNodeIdx = nodeInfo.prevNode_[j];
            if( prevNodeIdx == 0xffffffff )
            {
                continue;
            }

            nodeEditor_Node* prevNode = newNodes[prevNodeIdx];

            /* 接続元ノードの利用されていない最初のvalveOutをこのノードのvalveInに接続する */
            vector<nodeEditor_ValveOut*>& prevVoList = prevNode->valveOutList();
START_CONNECT:
            int k=0;
            for( k=0;k<prevVoList.size()-1;++k)
            {
                nodeEditor_ValveOut* prevVo = prevVoList[k];
                if( !prevVo->isConnected() )
                {
                    viList[j]->setValveOut( prevVo );
                    prevVo->setValveIn( viList[j] );
                    break;
                }
            }
            /* 合致するのがなかったら新しいValveOutを追加する。常に余計に一つないといけない事に注意 */
            if( k == prevVoList.size()-1 )
            {
                prevNode->addNewValveOut();
                goto START_CONNECT;
            }
        }
    }
    /**/
    for( int i=0;i<newNodes.size();++i)
    {
        nodeEditor_Node* newNode = newNodes[i];
        std::vector<nodeEditor_ValveIn*> viList = newNode->valveInList();
        for( int j=0;j<viList.size();++j)
        {
            viList[j]->updatePos();
        }
        std::vector<nodeEditor_ValveOut*> voList = newNode->valveOutList();
        for( int j=0;j<voList.size();++j)
        {
            voList[j]->updatePos();
        }
    }
    ///* IDを設定する */
    //for( int i=0;i<newNodes.size();++i)
    //{
    //    nodeEditor_Node* newNode = newNodes[i];
    //    newNode->setId(i);
    //}
    /**/
    SceneArray& sceneArray = nlScene_getSceneData();
    sceneArray[currentSceneIndex_].sceneTimeInMs = MX( exportScene.totalTimeInMs, MIN_SCENE_TIME );
}
/**/
void NodeEditor::updateIds( 
    const ExportSceneInfo& exportScene )
{
    /* TODO 全てのノードをクリアする */
    int itemId = 0;
    QList<QGraphicsItem *> items = scene()->items();
    foreach( QGraphicsItem* item, items )
    {
        nodeEditor_Node* node = dynamic_cast<nodeEditor_Node*>(item);
        if( node )
        {
            node->setId(itemId);
            ++itemId;
        }
    }
}
/**/
ExportSceneInfo NodeEditor::exportNodeInfoFromMemory()
{
#if 0 /* ソートされているかのチェック  */
    const ExportSceneInfo& sceneInfo = loadedSceneInfo_[currentSceneName_];
    for( unsigned int nodeIndex=0;nodeIndex<sceneInfo.nodeInfo.size();++nodeIndex )
    {
        const NodeExportInfo& nodeInfo = sceneInfo.nodeInfo[nodeIndex];
        BOOST_FOREACH( unsigned int prevIdx, nodeInfo.prevNode_ )
        {
            if( (prevIdx != -1) && (prevIdx > nodeIndex) )
            {
                NL_LOG(ERR_025);
            }
        }
    }
#endif
    /**/
    return loadedSceneInfo_[currentSceneName_];
}
/**/
vector<ExportSceneInfo> NodeEditor::sceneInfoList()const
{
    vector<ExportSceneInfo> ret;
    LoadedSceneInfo::const_iterator ite = loadedSceneInfo_.begin();
    while( ite != loadedSceneInfo_.end() )
    {
        ret.push_back( ite->second );
        ++ite;
    }
    return ret;
}
/**/
void NodeEditor::onSceneEditValveNumChange(int newValveNum )
{
    if( currentChoiseNode_ )
    {
        /**/
        currentChoiseNode_->nodeProp().numValve_ = newValveNum; 
        /* 同様のデータをシーンマップにも書き込む */
        SceneArray& scenes = nlScene_getSceneData();
        nlSceneNode& sceneNode = scenes[currentSceneIndex_].sceneMaps_[currentChoiseNode_->nodeId()];
        sceneNode.numValve_ = newValveNum;
        /* 再描画 */
        emit(redraw());
    }
}
/**/
void NodeEditor::onSceneEditProp0Change( double value ){onSceneEditPropChange(value,0);}
void NodeEditor::onSceneEditProp1Change( double value ){onSceneEditPropChange(value,1);}
void NodeEditor::onSceneEditProp2Change( double value ){onSceneEditPropChange(value,2);}
void NodeEditor::onSceneEditProp3Change( double value ){onSceneEditPropChange(value,3);}
void NodeEditor::onSceneEditProp4Change( double value ){onSceneEditPropChange(value,4);}
void NodeEditor::onSceneEditProp5Change( double value ){onSceneEditPropChange(value,5);}
void NodeEditor::onSceneEditPropChange( double value, unsigned int index )
{
    if( currentChoiseNode_ )
    { 
        /**/
        currentChoiseNode_->nodeProp().prop_[index] = value; 
        /* シーンマップにも同様のデータを書き込む */
        SceneArray& scenes = nlScene_getSceneData();
        nlSceneNode& sceneNode = scenes[currentSceneIndex_].sceneMaps_[currentChoiseNode_->nodeId()];
        sceneNode.prop_[index] = value;
        /* 再描画 */
        emit(redraw());
    }
}
/**/
void NodeEditor::saveCurrentScene()
{
    /**/
    if( MessageBoxA( NULL, "現在のシーンを保存しますか？", "確認", MB_YESNO ) != IDYES )
    { return ;}
    /**/
    ExportSceneInfo nowSceneInfo = exportNodeInfoFromUI();
    QString filePath = sandboxPath(SP_SCENE)+currentSceneName_+".xml";
    QByteArray fileNameBA = filePath.toLocal8Bit();
    std::ofstream file( fileNameBA.data() );
    boost::archive::xml_oarchive oarchive(file);
    oarchive << boost::serialization::make_nvp("Root", nowSceneInfo );
}
/**/
void NodeEditor::onProp0StringChanged(QString newTextQStr){onPropStringChanged(newTextQStr,0,ui.sceneEditProp0Float);}
void NodeEditor::onProp1StringChanged(QString newTextQStr){onPropStringChanged(newTextQStr,1,ui.sceneEditProp1Float);}
void NodeEditor::onProp2StringChanged(QString newTextQStr){onPropStringChanged(newTextQStr,2,ui.sceneEditProp2Float);}
void NodeEditor::onProp3StringChanged(QString newTextQStr){onPropStringChanged(newTextQStr,3,ui.sceneEditProp3Float);}
void NodeEditor::onProp4StringChanged(QString newTextQStr){onPropStringChanged(newTextQStr,4,ui.sceneEditProp4Float);}
/**/
void NodeEditor::onPropStringChanged( QString newTextQStr, unsigned int index, QDoubleSpinBox* target )
{
    if( !currentChoiseNode_ )
    { return; }

    /*if( newTextQStr = "" )
    { return; }*/

    /* VPT_STRING_TO_ID出ない場合は終了 */
    const NodeFeature& feature = currentChoiseNode_->nodeFuture();
    if( (index<feature.props_.size()) && (feature.props_[index].type_ != VPT_STRING_TO_ID) )
    { return; }

    /**/
    SceneArray& scenes = nlScene_getSceneData();
    nlSceneNode& sceneNode = scenes[currentSceneIndex_].sceneMaps_[currentChoiseNode_->nodeId()];

    /**/
    optional<int> id;
    QByteArray nweTextBA = newTextQStr.toUtf8();
    const char* newText = nweTextBA.data();
    const NodeFeature& future = currentChoiseNode_->nodeFuture();
    if( index<future.props_.size() )
    {
        string groupName = future.props_[index].propName;
        id = getId( groupName.c_str(), newText );
    }

    /* 有効なタグの場合 */
    if( id )
    { 
        target->setValue((double)*id);
        sceneNode.prop_[index] = *id;
    }
    /* 有効なタグでない場合 */
    else
    {
        target->setValue((double)0); 
        sceneNode.prop_[index] = 0;
    }

    /* 現在値の保存 */
    nodeEditor_Node* node = currentChoiseNode_;
    if( node )
    {
        NodeDataProp& nodeProp  = node->nodeProp();
        nodeProp.propStr_[index] = newText;
    }
}
/**/
void NodeEditor::onChangeSceneTimeInMs( int timeInMs )
{
    SceneArray& sceneArray = nlScene_getSceneData();
	unsigned int& sceneTimeInMs = sceneArray[currentSceneIndex_].sceneTimeInMs;
	sceneTimeInMs = timeInMs;
}
/**/
void NodeEditor::onSceneEditIsUseThisNodeChange( int state )
{
    /* 現在のノードの利用、不利用フラグを設定する */
    nodeEditor_Node* node = currentChoiseNode_;
    if( node )
    {
        NodeDataProp& nodeProp  = node->nodeProp();
        switch( state )
        {
        case Qt::Unchecked:
            nodeProp.isUse_ = false;
            break;
        case Qt::Checked:
            nodeProp.isUse_ = true;
            break;
        case Qt::PartiallyChecked:
            /* 特に何も */
            break;
        default:
            break;
        }
    }
    /* 再描画 */
    emit(redraw());
}
/**/
void NodeEditor::onSceneEditCommentChanged()
{
    /* 現在のテキストを取得する */
    QString text = ui.textComment->toPlainText();
	if( text == "" )
	{ return; }
    /* それをテキストとして設定する */
    nodeEditor_Node* node = currentChoiseNode_;
    if( node )
    {
        QByteArray baText = text.toUtf8();
        NodeDataProp& nodeProp  = node->nodeProp();
        nodeProp.comment_ = baText.data();
    }
}
/**/
void NodeEditor::onChangeCurrentScene( int newSceneIndex )
{
    /**/
    if( ( newSceneIndex < 0 ) || ( sceneFiles.size() <= newSceneIndex ) )
    {
        return;
    }
     /* 前のシーンを保存する */
    if( currentSceneName_ != "" )
    {
        writeSceneToMemory();
    }
    currentSceneIndex_ = newSceneIndex;
    /* 新しいシーンを再構築する */
	const QString mediaDir = sandboxPath(SP_MEDIA);
    QString sceneName = sceneFiles[newSceneIndex].baseName();
    /* 現在の値を保存しておく */
    if( currentSceneName_ != "" )
    {
        ExportSceneInfo tmp = exportNodeInfoFromUI();
        loadedSceneInfo_[currentSceneName_] = tmp;
    }
    /* シーン全体の破棄 */
    scene()->clear();
    /* ロード情報から復元する */
    ExportSceneInfo exportInfo = {};
    NL_ASSERT( loadedSceneInfo_.find(sceneName) != loadedSceneInfo_.end() );
    importNodeInfo( loadedSceneInfo_[sceneName], currentSceneIndex_ );
    /* ウィジェットのタイトルを変更する */
    ui.dockSceneEdit->setWindowTitle( QString("SceneEdit [")+QString(sceneName)+QString("]") );
    /* シーン設定に関するGUIを更新 */
    SceneArray& sceneArray = nlScene_getSceneData();
    const int sceneTimeInMs = sceneArray[currentSceneIndex_].sceneTimeInMs;
    ui.sceneTotalTime->setValue( sceneTimeInMs );

#if 1
	/* そのシーンが持つノードでプロパティがstring->IDのものがあった場合、そのstringの正しいIDに変換する */
	QList<QGraphicsItem*> items = scene()->items();
	BOOST_FOREACH( QGraphicsItem* item, items )
	{
		nodeEditor_Node* node = dynamic_cast<nodeEditor_Node*>(item);
		if( !node )
		{ continue; }
		/**/
		const vector<NodeFeature::PropProp>& props = node->nodeFuture().props_;
		NodeDataProp& nodeProp = node->nodeProp();
        const NodeFeature& nodeFuture = node->nodeFuture();
		for( int i=0;i<props.size(); ++i )
		{
			if( props[i].type_ == VPT_STRING_TO_ID )
			{
				string propStr = nodeProp.propStr_[i];
                string propGroup = nodeFuture.props_[i].propName;
                optional<int>newId = getId( propGroup.c_str(), propStr.c_str() );
				if( newId && newId !=  (int)nodeProp.prop_[i] )
				{
                    NL_LOG( WAR_000,
						props[i].propName.c_str(),
						(int)nodeProp.prop_[i],
						*newId);
					nodeProp.prop_[i] = *newId;
				}
			}
		}
	}
#endif
    /**/
    currentSceneName_ = sceneName;
	/* 再描画 */
    emit(redraw());
}
/**/
void NodeEditor::writeSceneToMemory()
{
    /**/
    const QString fileName = currentSceneName_;
    /* シーンエディタUIのノード情報を取得し、これを保存する */
    loadedSceneInfo_[fileName] = exportNodeInfoFromUI();
}
/* ノードのソート */
int getDepth( const NodeExportInfos& nodes, int index, vector<int>& depth  )
{
    /* 存在しないノードなら0 */
    if( index == -1 )
    { return 0; }

    /* 既に計算されていたらそれを利用 */
    if( depth[index] != -1 )
    { return depth[index]; }

    /* 葉っぱなら深さ0 */
    bool isLeaf = true;
    for( int j=0;j<SCENENODE_INPUT_NUM;j++)
    {
        if( nodes[index].prevNode_[j] != -1 )
        { isLeaf = false; }
    }
    if( isLeaf )
    {
        depth[index] = 0;
        return 0;
    }
    /* 葉っぱでないなら、子供の深さを判定する */
    else
    {
        int maxDepth = -1;
        for( int j=0;j<SCENENODE_INPUT_NUM;j++)
        {
            maxDepth = MX( getDepth( nodes, nodes[index].prevNode_[j], depth ), maxDepth);
        }
        int d = maxDepth + 1;
        depth[index] = d;
        return d;
    }
}
/**/
static void sortNodes( NodeExportInfos& nodes )
{
    unsigned long start = timeGetTime();
	/**/
	if( nodes.empty() )
	{ return; }

    /* 深さに対してソート */
    int maxDepth = 0;
    vector<int>depth(nodes.size(), -1);
    for( int i=0;i<nodes.size();++i)
    {
        /**/
        if( depth[i] != -1 )
        { continue; }
        /* まだ深さ設定していないノードのみを対象にする */
        depth[i] = getDepth(nodes,i,depth);
        maxDepth = MX(depth[i],maxDepth);
    }

#if 0
    NodeExportInfos tmpInfos;
    tmpInfos.reserve(nodes.size());
    for( int i=0;i<maxDepth;++i)
    {
        for( int j=0;j<depth.size();++j)
        {
            if( depth[j] ==  i )
            {
                tmpInfos.push_back(nodes[j]);
            }
        }
    }
    nodes = tmpInfos;
#endif

#if 1
    /* HACK バブルソート。ソート時間が問題になってきたら対処する */
    for( int j=0;j<nodes.size()-1;j++)
    {
        for( int i=0;i<nodes.size()-1;i++)
        {
            /*  */
            if( depth[i] > depth[i+1] )
            {
                /* 前の箇所を参照していたノードを新しく変更する */
                for( int j=0;j<nodes.size();++j)
                {
                    for( int k=0;k<SCENENODE_INPUT_NUM;++k)
                    {
                        if( nodes[j].prevNode_[k] == i )
                        {
                            nodes[j].prevNode_[k] = i+1;
                        }
                        else if( nodes[j].prevNode_[k] == i+1 )
                        {
                            nodes[j].prevNode_[k] = i;
                        }
                    }
                }
                /* スワップ */
                NodeExportInfo tmp = nodes[i];
                nodes[i] = nodes[i+1];
                nodes[i+1] = tmp;
                int tmpDepth = depth[i];
                depth[i] = depth[i+1];
                depth[i+1] = tmpDepth;
            }
        }
    }
#endif

#if 0 /* ソートされているかのチェック */
    for( int i=0;i<nodes.size();++i)
    {
        for( int j=0;j<SCENENODE_INPUT_NUM;++j)
        {
            unsigned int prevIdx = nodes[i].prevNode_[j];
            NL_ASSERT( prevIdx == -1 || prevIdx <= i );
        }
    }
#endif
}
/**/
void NodeEditor::keyPressEvent(QKeyEvent* e)
{
	/* スペースキーでタイムラインの再生を切り替えられる */
	if( e->key() == Qt::Key_Space )
	{
        bool isLoopScene = (Qt::ShiftModifier&e->modifiers());
        ui.dockTimeline->togglePlay( isLoopScene );
    }
    /* ctr+sで保存 */
    else if( (e->key() == Qt::Key_S ) && (e->modifiers() && Qt::ShiftModifier) )
    {saveCurrentScene();}
    /* 矢印キーで3秒前後する */
    else if( (e->key() == Qt::Key_Right ) )
    {ui.dockTimeline->movePosition(3000);}
    else if( (e->key() == Qt::Key_Left ) )
    {ui.dockTimeline->movePosition(-3000);}
}
/**/
void NodeEditor::updateNodePropDisplay_( QGraphicsItem* item )
{
    /* ノードであった場合そのプロパティに更新する。利用していないスピンボックスは隠す。 */
    if( currentChoiseNode_ )
    {
        unsigned long start = timeGetTime();
        /**/
        const nodeEditor_Node* node = currentChoiseNode_;
        const NodeFeature& nodeFuture = node->nodeFuture();
        const NodeDataProp& nodeProp  = node->nodeProp();
        const std::vector<NodeFeature::PropProp> props = nodeFuture.props_;

        //bool propsEnable[6] = {};
        vector<bool> propsEnable(6);
        for( int i=0;i<6;++i)
        { propsEnable[i] = (nodeFuture.props_.size()>i); }

        /* ノードのプロパティ表示非表示の設定 */
        ui.sceneEditProp0Float->setEnabled(false);
        ui.sceneEditProp1Float->setEnabled(false);
        ui.sceneEditProp2Float->setEnabled(false);
        ui.sceneEditProp3Float->setEnabled(false);
        ui.sceneEditProp4Float->setEnabled(false);
        ui.sceneEditProp0Combo->setEnabled(false);
        ui.sceneEditProp1Combo->setEnabled(false);
        ui.sceneEditProp2Combo->setEnabled(false);
        ui.sceneEditProp3Combo->setEnabled(false);
        ui.sceneEditProp4Combo->setEnabled(false);

        /* プロパティのfloat,intの変換など。数値を設定する前にやらないといけない */
        class Local
        {
        public:
            static void setUpInputPropaty(
                bool propEnable,
                ValvePropType_t type,
                QDoubleSpinBox* doubleSpinBox,
                QComboBox* propCombobox )
            {
                if( propEnable )
                {
                    switch( type )
                    {
                    case VPT_FLOAT:
                        doubleSpinBox->setValue(0.0f);
                        doubleSpinBox->setEnabled( true );
                        doubleSpinBox->setDecimals(5);
                        doubleSpinBox->setMinimum(-DBL_MAX);
                        doubleSpinBox->setMaximum(DBL_MAX);
                        doubleSpinBox->setSingleStep(0.001);
                        propCombobox->setEnabled( false );
                        break;
                    case VPT_INT:
                        doubleSpinBox->setValue(0.0f);
                        doubleSpinBox->setEnabled(true);
                        doubleSpinBox->setDecimals(0);
                        doubleSpinBox->setMinimum(-1);
                        doubleSpinBox->setMaximum(1024);
                        doubleSpinBox->setSingleStep(1.0);
                        propCombobox->setEnabled( false );
                        break;
                    case VPT_STRING_TO_ID:
                        doubleSpinBox->setEnabled(false);
                        propCombobox->setEnabled( true );
                        break;
                    default:
                        break;
                    }
                }
            }
            /**/
            static void addPropCombo( QComboBox* combo, QString text, QString target )
            {
                combo->clear();
                Ids ids = getIds( text ); 
                BOOST_FOREACH( const Id& id, ids )
                { combo->addItem(id.first); }
                /**/
                int index = combo->findText( target );
                if( index != -1 )
                { combo->setCurrentIndex( index ); }
            }
        };
        /* HACK 各種GUIに大しての基本設定の変更は、currentNodeの数値変更を招くため、一時的に退避させている。 */
        nodeEditor_Node* tmp = currentChoiseNode_;
        currentChoiseNode_ = NULL;
        if( props.size() > 0 ){ Local::setUpInputPropaty( propsEnable[0], props[0].type_, ui.sceneEditProp0Float, ui.sceneEditProp0Combo ); }
        if( props.size() > 1 ){ Local::setUpInputPropaty( propsEnable[1], props[1].type_, ui.sceneEditProp1Float, ui.sceneEditProp1Combo );}
        if( props.size() > 2 ){ Local::setUpInputPropaty( propsEnable[2], props[2].type_, ui.sceneEditProp2Float, ui.sceneEditProp2Combo );}
        if( props.size() > 3 ){ Local::setUpInputPropaty( propsEnable[3], props[3].type_, ui.sceneEditProp3Float, ui.sceneEditProp3Combo );}
        if( props.size() > 4 ){ Local::setUpInputPropaty( propsEnable[4], props[4].type_, ui.sceneEditProp4Float, ui.sceneEditProp4Combo );}
        currentChoiseNode_ = tmp;

        /* プロパティの数値の反映 */ 
        if( propsEnable[0] ){ui.sceneEditProp0Float->setValue(nodeProp.prop_[0]);}else{ ui.sceneEditProp0Float->setValue(0.0f);}
        if( propsEnable[1] ){ui.sceneEditProp1Float->setValue(nodeProp.prop_[1]);}else{ ui.sceneEditProp1Float->setValue(0.0f);}
        if( propsEnable[2] ){ui.sceneEditProp2Float->setValue(nodeProp.prop_[2]);}else{ ui.sceneEditProp2Float->setValue(0.0f);}
        if( propsEnable[3] ){ui.sceneEditProp3Float->setValue(nodeProp.prop_[3]);}else{ ui.sceneEditProp3Float->setValue(0.0f);}
        if( propsEnable[4] ){ui.sceneEditProp4Float->setValue(nodeProp.prop_[4]);}else{ ui.sceneEditProp4Float->setValue(0.0f);}

        /* ノードプロパティ名の設定 */
        if( propsEnable[0] ){ ui.sceneEditPropLabel0->setText(props[0].propName.c_str()); }else{ ui.sceneEditPropLabel0->setText("none"); }
        if( propsEnable[1] ){ ui.sceneEditPropLabel1->setText(props[1].propName.c_str()); }else{ ui.sceneEditPropLabel1->setText("none"); }
        if( propsEnable[2] ){ ui.sceneEditPropLabel2->setText(props[2].propName.c_str()); }else{ ui.sceneEditPropLabel2->setText("none"); }
        if( propsEnable[3] ){ ui.sceneEditPropLabel3->setText(props[3].propName.c_str()); }else{ ui.sceneEditPropLabel3->setText("none"); }
        if( propsEnable[4] ){ ui.sceneEditPropLabel4->setText(props[4].propName.c_str()); }else{ ui.sceneEditPropLabel4->setText("none"); }

        /* そのプロパティの候補を全て挿入する */
        Local::addPropCombo( ui.sceneEditProp0Combo, ui.sceneEditPropLabel0->text(), nodeProp.propStr_[0].c_str() );
        Local::addPropCombo( ui.sceneEditProp1Combo, ui.sceneEditPropLabel1->text(), nodeProp.propStr_[1].c_str() );
        Local::addPropCombo( ui.sceneEditProp2Combo, ui.sceneEditPropLabel2->text(), nodeProp.propStr_[2].c_str() );
        Local::addPropCombo( ui.sceneEditProp3Combo, ui.sceneEditPropLabel3->text(), nodeProp.propStr_[3].c_str() );
        Local::addPropCombo( ui.sceneEditProp4Combo, ui.sceneEditPropLabel4->text(), nodeProp.propStr_[4].c_str() );

        /* バルブ数の反映 */
        chagenSceneEditValuveNum( nodeProp.numValve_ );

        /* 利用中表示の反映 */
        extern Ui::nlSandboxClass ui;
        if( nodeProp.isUse_ ){ ui.chkStopThisNode->setCheckState( Qt::Checked ); }
        else                 { ui.chkStopThisNode->setCheckState( Qt::Unchecked ); }

        /* ノードコメントの更新 */
        ui.textComment->setText( nodeProp.comment_.c_str() );

        /* 選択中のノードの出力値を表示する */
        unsigned int nodeIdx = getNodeIndex( node );
        if( nodeIdx != -1 )
        {
            SceneArray& sceneArray = nlScene_getSceneData();
            ProcessBuffer& processBuffer = nlScene_getProcessBuffer();
            const nlNodeValveArray& arr = processBuffer[nodeIdx].output_;
            ui.sceneNodePropList->clear();
            /* 出力タイプによって表示を分ける */
            switch( nodeFuture.outEdgeType_ )
            {
            case NL_ET_None:
                break;
            case NL_ET_Vec4:
                for( int i=0;i<arr.sizeAsVec4();++i)
                {
                    QString str;
                    nlVec4 val = arr.atAsVec4(i);
                    str.sprintf("%04d:\n"
                                " %+2.2f %+2.2f %+2.2f %+2.2f",
                                i, val.x_, val.y_, val.z_, val.w_ );
                    ui.sceneNodePropList->addItem( str );
                }
                break;
            case NL_ET_Matrix:
                for( int i=0;i<arr.sizeAsMatrix();++i)
                {
                    QString str;
                    nlMatrix val = arr.atAsMatrix(i);
                    str.sprintf("%04d:\n"
                                "  %+2.2f %+2.2f %+2.2f %+2.2f\n"
                                "  %+2.2f %+2.2f %+2.2f %+2.2f\n"
                                "  %+2.2f %+2.2f %+2.2f %+2.2f\n"
                                "  %+2.2f %+2.2f %+2.2f %+2.2f\n",
                                i,
                                val._11, val._12, val._13, val._14,
                                val._21, val._22, val._23, val._24,
                                val._31, val._32, val._33, val._34,
                                val._41, val._42, val._43, val._44 );
                    ui.sceneNodePropList->addItem( str );
                }
                break;
            case NL_ET_Render:
                break;
            default:
                break;
            }
        }
        else
        {
            NL_ERR( ERR_021 );
        }
    }
}
/**/
void NodeEditor::mousePressEvent(QMouseEvent* e )
{
    setCursor(Qt::ClosedHandCursor);
	static QGraphicsItem* prevItem = NULL;
    QGraphicsItem* item = itemAt( e->pos().x(), e->pos().y() );
    currentChoiseNode_ = dynamic_cast<nodeEditor_Node*>(item);
    /* 前回と違うアイテムをクリックしていたらプロパティを更新する */
    if( prevItem != item && e->button()==Qt::LeftButton && currentChoiseNode_ )
    {
        updateNodePropDisplay_( item );
    }
	prevItem = item;

    /**/
    if( e->modifiers() & Qt::ShiftModifier )
    {
        nodeCopydatas.clear();
        setDragMode( QGraphicsView::RubberBandDrag  );
        selectStart_ = e->pos();
    }
    /* 視点の移動 */
    else if( !item && ( e->button()==Qt::LeftButton ) )
    {
        setDragMode( QGraphicsView::ScrollHandDrag );
    }

#if 0
    /* コピペのデータを取る */
    if( currentChoiseNode_ )
    {
        copyedFuture = currentChoiseNode_->nodeFuture();
        copyedProp = currentChoiseNode_->nodeProp();
    }
#endif

    QGraphicsView::mousePressEvent( e );
}
/**/
void NodeEditor::mouseMoveEvent( QMouseEvent* e )
{
    QGraphicsView::mouseMoveEvent( e );
}
/**/
void NodeEditor::mouseReleaseEvent ( QMouseEvent* e )
{
    /**/
    if( dragMode() == QGraphicsView::RubberBandDrag )
    {
        /**/
        QPoint s0 = selectStart_;
        QPoint s1 = e->pos();
        QPoint lu = QPoint( min(s0.x(),s1.x()), min(s0.y(),s1.y()) );
        QPoint rd = QPoint( max(s0.x(),s1.x()), max(s0.y(),s1.y()) );
        const QRect rect( lu, rd );
        QList<QGraphicsItem *> itemList = items(rect);
        vector<nodeEditor_Node*> nodes;
        QPointF leftUp(FLT_MAX,0.0f);
        BOOST_FOREACH( QGraphicsItem* item, itemList )
        {
            nodeEditor_Node* node = dynamic_cast<nodeEditor_Node*>(item);
            if( !node )
            { continue; }
            nodes.push_back(node);
            /* 一番左を算出 */
            float x = item->scenePos().x();
            if(x<leftUp.x())
            {
                leftUp.setX(x);
                leftUp.setY(item->scenePos().y());
            }
        }
        /**/
        BOOST_FOREACH(nodeEditor_Node* node, nodes )
        {
            QPointF pos = node->scenePos() - leftUp;
            nodeCopydatas.push_back( NodeCopyData(node->nodeFuture(),node->nodeProp(),pos) );
        }
        /**/
        NL_LOG(INF_007,nodeCopydatas.size());
    }
    /**/
    setCursor(Qt::ArrowCursor);
    /**/
    QGraphicsView::mouseReleaseEvent( e );
    /**/
    setDragMode( QGraphicsView::NoDrag );
}
/**/
void NodeEditor::wheelEvent(QWheelEvent* e )
{
    const float scaleFactor = pow( 2.0, -(-e->delta()) / 240.0);
    qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.1 || factor > 2 )
    { return; }
    scale(scaleFactor, scaleFactor);
}
/**/
void NodeEditor::mouseDoubleClickEvent( QMouseEvent* e )
{
    nodeEditor_Node* node = dynamic_cast<nodeEditor_Node*>(itemAt( e->x(), e->y()));
    /* 削除する */
    if( node )
    {
        /**/
        onDoubleClick( optional<int>() );
        /* TODO 以下をonDoubleClick()内に移動させる */
        /* 削除する前にコピーしておく */
        nodeCopydatas.clear();
        nodeCopydatas.push_back( NodeCopyData(node->nodeFuture(),node->nodeProp(),QPointF(0,0)) );
        /* removeItemを呼んだ直後には削除されないので、明示的に終了処理 */
        node->finalize();
        scene()->removeItem( node );
        currentChoiseNode_ = NULL;
        /* TODO ここでプロパティ表示もクリアする */
        /* シーンマップの更新 */
        redraw();
    }
    /* 前回選択したものと同じノードをそこに貼り付ける */
    else
    {
        /* TODO 以下をonDoubleClick()の中に移動させる */
        onDoubleClick( optional<int>(0) );
        /**/
        BOOST_FOREACH( NodeCopyData& nodeCopyData, nodeCopydatas )
        {
            const NodeFeature copyedFuture = nodeCopyData.future_;
            const NodeDataProp copyedProp = nodeCopyData.prop_;
            const QPointF position = nodeCopyData.position_;
            const string curNodeTypeName = copyedFuture.nodeName_;
            const vector<NodeFeature>& nodeFutures = desc_.nodeFeatures_;
            for( int i=0;i<nodeFutures.size();++i)
            {
                if( nodeFutures[i].nodeName_ == curNodeTypeName )
                {
                    /* 最初にSNT_INVALIDが来るノードを割り当てる  */
                    SceneArray& scenes = nlScene_getSceneData();
                    SceneMap& sceneMap = scenes[currentSceneIndex_].sceneMaps_;
                    int sceneMapIdx=0;
                    for( sceneMapIdx=0;sceneMapIdx<sceneMap.size();++sceneMapIdx)
                    { if( sceneMap[sceneMapIdx].type_ == SNT_INVALID ){ break; } }
                    /**/
                    nodeEditor_Node* newNode = new nodeEditor_Node(this,scene(), nodeFutures[i], currentSceneIndex_, sceneMapIdx );
                    newNode->setProp( copyedProp );
                    /* シーン上の位置に変換する */
                    QPointF scenePos = mapToScene( e->pos()-QPoint(0,-30) ) + position; 
                    newNode->setPos(scenePos);
                    scene()->addItem( newNode );
                    /* シーンマップ更新 */
                    nlSceneNode& sceneNode = sceneMap[sceneMapIdx];
                    sceneNode.type_ = nodeFutures[i].nodeId_;
                    for( int i=0;i<SCENENODE_PROP_NUM;++i)
                    { sceneNode.prop_[i] = copyedProp.prop_[i]; }
                    sceneNode.numValve_ = copyedProp.numValve_;
                    sceneNode.outEdgeType = copyedFuture.outEdgeType_;
                    ++scenes[currentSceneIndex_].numSceneMap_;
                    /* 再描画 */
                    emit(redraw());
                    break;
                }
            }
        }
    }
}
/**/
void NodeEditor::drawBackground(
    QPainter *painter, 
    const QRectF &rect)
{
    /* 一旦背景塗りつぶし */
    QBrush b(QColor(75,75,90));
    painter->fillRect(rect, b );
    /**/
    const QRectF sr = sceneRect();
    const float sx = sr.left();
    const float ex = sr.right();
    const float sy = sr.top();
    const float ey = sr.bottom();

    /* 注意:シーンを回転させていないことを前提にしていることに注意 */
    const float scale = transform().m11();
    const float lineWidth = 1.0f/scale;

    /* 罫線 */
    const int lineNum = 50;
    painter->setPen( QPen(Qt::black, lineWidth, Qt::SolidLine ) );
    for( int i=0;i<lineNum;++i)
    {
        float dy = (ey-sy)/lineNum;
        const float y = dy*i + sy; 
        QLineF line( QPointF(sx,y), QPointF(ex,y) );
        painter->drawLine( line );
    }
    for( int i=0;i<lineNum;++i)
    {
        float dx = (ex-sx)/lineNum;
        const float x = dx*i + sx; 
        QLineF line( QPointF(x,sy), QPointF(x,ey) );
        painter->drawLine( line );
    }
}
/**/
void NodeEditor_scene::contextMenuEvent(
	QGraphicsSceneContextMenuEvent* e )
{
	/**/
	QMenu contextMenu( "menu" );
	/* 右クリック箇所にノードが有る場合はそれを削除するメニューを出す */
	nodeEditor_Node* node = dynamic_cast<nodeEditor_Node*>( itemAt( e->scenePos() ) );
	if( node )
	{
		delAction_->setScenePos( e->scenePos() );
		contextMenu.addAction( delAction_ );            
	}
	/* ない場合はノードを追加するメニューを起動する */
	else
	{
		/*
		menu->addAction(menuLaunch->menuAction());
		みたいな感じで子メニューの追加をするようだ。
		*/
		std::vector<QMenu*> menus;
		menus.resize( NodeType_Max );
		/**/
		for( int i=0;i<menus.size();++i)
		{
			menus[i] = new QMenu();
		}
		menus[NodeType_Unknown]->setTitle("Unknown");
		menus[NodeType_Math]->setTitle("Math");
        menus[NodeType_Matrix]->setTitle("Matrix");
		menus[NodeType_Array]->setTitle("Array");
		menus[NodeType_Render]->setTitle("Render");
		menus[NodeType_Util]->setTitle("Util");
		menus[NodeType_Other]->setTitle("Other");
		menus[NodeType_Deprecated]->setTitle("Deprecated");

		for( int i=0;i<contextMenuActions_.size();++i )
		{
			RightClickAction* action = contextMenuActions_[i];
			action->setScenePos( e->scenePos() );
			menus[action->nodeType()]->addAction(action);
		}
		/* Unknownを除く全てのノードを追加する */
		for( int i=1;i<menus.size();++i)
		{
			contextMenu.addAction(menus[i]->menuAction());
		}
	}
	contextMenu.exec( e->screenPos() );
}

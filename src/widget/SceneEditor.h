#pragma once

#include "NodeEditor.h"

/**
 * @brief 
 */
class SceneEditor
    :public NodeEditor
{
    Q_OBJECT
public:
    /**
     * @brief
     */
    SceneEditor(QWidget* parent);
    /**
     * @brief
     */
    ~SceneEditor();
protected:
    /**
     * @brief 
     */
    void onAddNode();
    /**
     * @brief 
     */
    void onConnectNode();
    /**
     * @brief 
     */
    void onDisconnectNode();
    /**
     * @brief 
     */
    void onRemoveNode();
    /**
     * @brief 
     */
    void onForcusNode();
    /**
     * @brief 
     */
    void onDisforcusNode();
    /**
     * @brief 
     */
    void onClick( boost::optional<int> clickedNode );
    /**
     * @brief 
     */
    void onDoubleClick( boost::optional<int> clickedNode );
    /**
     * @brief
     */
    NodeeditorboardDesc nodeTypeList()const;
private:
};
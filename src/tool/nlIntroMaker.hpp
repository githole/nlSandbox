#ifndef _NLINTROMAKER_H_
#define _NLINTROMAKER_H_

#include "shared/scene/nlScene.hpp"

/**
* @brief
*/
class nlIntroMaker
    :public QObject
{
    Q_OBJECT
public:
    nlIntroMaker( 
        struct nlEngineContext* cxt,
            const nlArray<nlScene,MAX_SCENE_NUM>& scenes,
            QString path )
        :cxt_(cxt),
        scenes_(scenes),
        musicPath_(path)
    {}
public slots:
    void make();
private:
    bool makeMain();
private:
    struct nlEngineContext* cxt_;
    const nlArray<nlScene,MAX_SCENE_NUM>& scenes_;
    QString musicPath_;
};


#endif /* #ifndef _NLINTROMAKER_H_ */

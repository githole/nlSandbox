#pragma once

namespace Ui
{
    class nlSandboxClass;
}

/**
* @brief シーンエディタのDock
*        今は機能がついていないが、将来的にシーンエディタをまとめるのでそのときまで置いておく
*/
class SceneEditDock
    :public QDockWidget
{
    Q_OBJECT
public:
    SceneEditDock(QWidget* parent);
    ~SceneEditDock();
    void initialize( class Ui::nlSandboxClass* ui );
    void keyPressEvent(QKeyEvent* e);
private:
    class Ui::nlSandboxClass* ui_;
};

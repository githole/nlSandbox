#pragma once

namespace Ui
{
    class nlSandboxClass;
}

/**
* @brief �V�[���G�f�B�^��Dock
*        ���͋@�\�����Ă��Ȃ����A�����I�ɃV�[���G�f�B�^���܂Ƃ߂�̂ł��̂Ƃ��܂Œu���Ă���
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

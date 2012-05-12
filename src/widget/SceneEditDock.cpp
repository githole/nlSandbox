#include "main/stdafx.h"
#include "SceneEditDock.h"
#include "ui_nlsandbox.h"

/**/
SceneEditDock::SceneEditDock(QWidget* parent)
:QDockWidget( parent )
{
}
/**/
SceneEditDock::~SceneEditDock()
{
}
/**/
void SceneEditDock::initialize( class Ui::nlSandboxClass* ui )
{
    ui_ = ui;
}
/**/
void SceneEditDock::keyPressEvent(QKeyEvent* e)
{

}

#include "main/stdafx.h"
#include "widget/nlModelerWindow.h"

nlModelerWindow::nlModelerWindow( Ui_MainWindow* ui, QWidget *parent, Qt::WFlags flags )
    //:QMainWindow( parent, (Qt::WindowStaysOnTopHint ) ), /* ��ɍőO�� */
    :QMainWindow( parent ),
    ui_(ui)
{
}

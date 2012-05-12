#pragma

#include "ui_nlModel.h"

class nlModelerWindow
    :public QMainWindow
{
    Q_OBJECT
public:
    nlModelerWindow( Ui_MainWindow* ui, QWidget *parent = 0, Qt::WFlags flags = 0);
private:
    Ui_MainWindow* ui_;
};

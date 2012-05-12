
#include "tool/nlCam.h"

class widget_previewdock
    : public QDockWidget
{
    Q_OBJECT
public:
    widget_previewdock(QWidget* parent = 0 )
        :QDockWidget(parent)
    {
        /* eventFilter()を呼び出すためには必須 */
        installEventFilter(this);
    }
    /**/
    bool eventFilter(QObject *obj, QEvent *event)
    {
        if (obj == this) 
        {
            /* ダブルクリックでDockが隠れないようにする */
            if (event->type() == QEvent::NonClientAreaMouseButtonDblClick ) 
            {
                return true;
            }
            else 
            {
                return false;
            }
        }else
        {
            return QDockWidget::eventFilter(obj, event);
        }
    }
};


#include "tool/nlCam.h"

class widget_previewdock
    : public QDockWidget
{
    Q_OBJECT
public:
    widget_previewdock(QWidget* parent = 0 )
        :QDockWidget(parent)
    {
        /* eventFilter()���Ăяo�����߂ɂ͕K�{ */
        installEventFilter(this);
    }
    /**/
    bool eventFilter(QObject *obj, QEvent *event)
    {
        if (obj == this) 
        {
            /* �_�u���N���b�N��Dock���B��Ȃ��悤�ɂ��� */
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

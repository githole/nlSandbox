#include "main/stdafx.h"
#include "widget/CurveEditor.h"

/**/
CurveEditor::CurveEditor( QWidget* parent )
    :QGraphicsView(parent)
{
    /* TODO �V���O���g���ł��邱�Ƃ�ۏ؂����� */
}
/**/
CurveEditor::~CurveEditor()
{
}
/**/
void CurveEditor::drawBackground(QPainter *painter, const QRectF &rect)
{
      /* ��U�w�i�h��Ԃ� */
    QBrush b(QColor(75,75,90));
    painter->fillRect(rect, b );
}
/**/
void CurveEditor::drawForeground(QPainter *painter, const QRectF &rect)
{
}

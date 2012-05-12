#include "main/stdafx.h"
#include "widget/CurveEditor.h"

/**/
CurveEditor::CurveEditor( QWidget* parent )
    :QGraphicsView(parent)
{
    /* TODO シングルトンであることを保証させる */
}
/**/
CurveEditor::~CurveEditor()
{
}
/**/
void CurveEditor::drawBackground(QPainter *painter, const QRectF &rect)
{
      /* 一旦背景塗りつぶし */
    QBrush b(QColor(75,75,90));
    painter->fillRect(rect, b );
}
/**/
void CurveEditor::drawForeground(QPainter *painter, const QRectF &rect)
{
}

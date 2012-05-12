#include "main/stdafx.h"
#include "widget/CurveEditor.h"

/**/
CurveEditor::CurveEditor( QWidget* parent )
    :QGraphicsView(parent)
{
    /* TODO ƒVƒ“ƒOƒ‹ƒgƒ“‚Å‚ ‚é‚±‚Æ‚ð•ÛØ‚³‚¹‚é */
}
/**/
CurveEditor::~CurveEditor()
{
}
/**/
void CurveEditor::drawBackground(QPainter *painter, const QRectF &rect)
{
      /* ˆê’U”wŒi“h‚è‚Â‚Ô‚µ */
    QBrush b(QColor(75,75,90));
    painter->fillRect(rect, b );
}
/**/
void CurveEditor::drawForeground(QPainter *painter, const QRectF &rect)
{
}

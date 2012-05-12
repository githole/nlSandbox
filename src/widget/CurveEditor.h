#pragma once

/**
 * @brief カーブエディタ
 */
class CurveEditor 
    :public QGraphicsView
{
    Q_OBJECT
public:
    /**
     * @brief コンストラクタ
     */
    CurveEditor( QWidget* parent /* TODO バインド対象のデータを入れる */ );
    /**
     * @brief デストラクタ
     */
    ~CurveEditor();
    /**
     *
     */
    void drawBackground(QPainter *painter, const QRectF &rect);
    void drawForeground(QPainter *painter, const QRectF &rect);
private:
};

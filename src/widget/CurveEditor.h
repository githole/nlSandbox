#pragma once

/**
 * @brief �J�[�u�G�f�B�^
 */
class CurveEditor 
    :public QGraphicsView
{
    Q_OBJECT
public:
    /**
     * @brief �R���X�g���N�^
     */
    CurveEditor( QWidget* parent /* TODO �o�C���h�Ώۂ̃f�[�^������ */ );
    /**
     * @brief �f�X�g���N�^
     */
    ~CurveEditor();
    /**
     *
     */
    void drawBackground(QPainter *painter, const QRectF &rect);
    void drawForeground(QPainter *painter, const QRectF &rect);
private:
};

#pragma once

#include "main/stdafx.h"

namespace Ui
{
    class nlSandboxClass;
};

/**
* @brief Timeline
*/
class TimeLineDock
    : public QDockWidget
{
    Q_OBJECT
public:
    TimeLineDock( QWidget* parent );
    void initialize( class Ui::nlSandboxClass* ui );
    ~TimeLineDock();
    /**
    * @brief ���݂̃t���[�� 
    */
    int frame();
    /**
     * @brief �w��msec�������O�コ����
     *        �Đ���Ԃ�ύX���Ȃ����Ƃɒ���
     */
    void movePosition( int deltaInMs );
private:
    /**
    * @brief ���݂̌o�ߎ���
    */
    unsigned int ms();

public:
    void keyPressEvent(QKeyEvent* e);
signals:
    /**
    * @brief �Đ��������ύX�ɂȂ����Ƃ��ɁAms�P�ʂŒʒm
    */
    void changeFrameByMs( unsigned int ms );
    /**
     * @brief ���y�Đ����~�߂�^�C�~���O
     */
    void stopMusic();
    /**
     * @brief �Đ��J�n�ʒu�����Z�b�g���ꂽ�B
     */
    void startMusic( int timeInMs );

public slots:
    void togglePlay( bool isLoopScene = false );

private slots:
    void onTimeChange(int timeInMs);
    void onPlayPushed();
    void onChangeStartTime(int newStartTime );
    void onChangeEndTime(int newEndTime );
    void onPressSlider();
    void onReleaseSlider();
    void onAdvanceTime();
    void onChangeScene(int sceneIndex);
private:
    class Ui::nlSandboxClass* ui_;
    int timeStartTime_;
    int startTime_;
    int endTime_;
    bool isPlay_;
    QTimer* timer_;
    unsigned int playStartTime_;
    bool isLoopScene_;
    int currentScene_;
};

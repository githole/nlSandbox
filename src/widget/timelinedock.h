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
    * @brief 現在のフレーム 
    */
    int frame();
    /**
     * @brief 指定msec分だけ前後させる
     *        再生状態を変更しないことに注意
     */
    void movePosition( int deltaInMs );
private:
    /**
    * @brief 現在の経過時間
    */
    unsigned int ms();

public:
    void keyPressEvent(QKeyEvent* e);
signals:
    /**
    * @brief 再生時刻が変更になったときに、ms単位で通知
    */
    void changeFrameByMs( unsigned int ms );
    /**
     * @brief 音楽再生を止めるタイミング
     */
    void stopMusic();
    /**
     * @brief 再生開始位置がリセットされた。
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

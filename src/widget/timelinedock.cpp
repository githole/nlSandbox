#include "main/stdafx.h"
#include "timelineDock.h"
#include "ui_nlsandbox.h"

using namespace Ui;

/**/
TimeLineDock::TimeLineDock(QWidget* parent)
:QDockWidget(parent),
isPlay_(false),
isLoopScene_(false)
{
    timer_ = new QTimer();
    timer_->setInterval( 15 );
}
/**/
void TimeLineDock::initialize(Ui::nlSandboxClass* ui )
{
    ui_ = ui;
    connect( ui_->sliderTimeline, SIGNAL(valueChanged(int)),this,SLOT(onTimeChange(int)));
    connect( ui_->timeStartTL, SIGNAL(valueChanged(int)), this, SLOT(onChangeStartTime(int)) );
    connect( ui_->timeEndTL, SIGNAL(valueChanged(int)), this, SLOT(onChangeEndTime(int)) );
    connect( timer_, SIGNAL(timeout()), this, SLOT(onAdvanceTime()) );
    connect( ui_->sliderTimeline, SIGNAL(sliderPressed()), this, SLOT(onPressSlider()) );
    connect( ui_->sliderTimeline, SIGNAL(sliderReleased()), this, SLOT(onReleaseSlider()) );

    /* TODO 前回の設定値を取得する */
    /* HACK とりあえず0秒から190秒までを設定 */
    ui_->timeStartTL->setValue( 0 );
    ui_->timeEndTL->setValue( 190*1000 );
}
/**/
TimeLineDock::~TimeLineDock()
{
    delete timer_;
}
/**/
int TimeLineDock::frame()
{
    return ui_->sliderTimeline->value();
}
/**/
void TimeLineDock::movePosition( int deltaInMs )
{
    /* 一度音楽を止める */
    if( isPlay_ ){emit(stopMusic());}
    /**/
    const int timeInMs = MX( ui_->sliderTimeline->value()+deltaInMs, 0);
    timeStartTime_ = timeInMs;
    ui_->sliderTimeline->setValue(timeInMs);
    /* 音楽を再開させる */
    if( isPlay_ ){emit(startMusic(timeInMs));}
    /**/
    playStartTime_ = timeGetTime();
}
/**/
void TimeLineDock::keyPressEvent(QKeyEvent* e)
{
    if( e->key() == Qt::Key_Space )
    {
        togglePlay();
    }
}
/**/
void TimeLineDock::togglePlay( bool isLoopScene )
{
    /**/
    isLoopScene_ = isLoopScene;
    unsigned int nowInMs = ui_->sliderTimeline->value();
    currentScene_ = nlScene_sceneIndex(nowInMs);
    /* 現状は再生ボタンを押したときと同じ動作 */
    onPlayPushed();
}
/**/
void TimeLineDock::onTimeChange(int timeInMs )
{
    emit( changeFrameByMs(timeInMs) );
    ui_->timeNow->setValue( timeInMs );
}
/**/
void TimeLineDock::onPlayPushed()
{
    isPlay_ = !isPlay_;
    if( isPlay_ )
    {
        playStartTime_ = timeGetTime();
        timer_->start();
        /**/
        const int timeInMs = ui_->sliderTimeline->value();
        timeStartTime_ = timeInMs;
        /* 音楽を再開させる */
        emit(startMusic(timeInMs));
    }
    else
    { 
        timer_->stop();
        playStartTime_ = 0x80000000; 
        /* 音楽を停止させる */
        emit(stopMusic());
    }	
}
/**/
void TimeLineDock::onChangeStartTime( int newStartTime )
{
    /* フレーム計算結果が狂うため停止状態にする */
    if( isPlay_ ){ onPlayPushed(); };

    /* 終了時間を越えないように設定する */
    ui_->timeEndTL->setMinimum( newStartTime );
    startTime_ = newStartTime;
    /* スライダーの下限を変更 */
    ui_->sliderTimeline->setMinimum( newStartTime );
}
/**/
void TimeLineDock::onChangeEndTime( int newEndTime )
{
    /* フレーム計算結果が狂うため停止状態にする */
    if( isPlay_ ){ onPlayPushed(); };

    /* 開始時間を越えないように設定する */
    ui_->timeStartTL->setMaximum( newEndTime );
    endTime_ = newEndTime;
    /* スライダーの上限を変更 */
    ui_->sliderTimeline->setMaximum( newEndTime );
}
/**/
void TimeLineDock::onPressSlider()
{
    /* タイマー作動中なら一連の動作を停止する */
    if( isPlay_ )
    {
        onPlayPushed(); 
    };
    /* 音楽を停止させる */
    emit( stopMusic() );
}
/**/
void TimeLineDock::onReleaseSlider()
{
}
/**/
void TimeLineDock::onAdvanceTime()
{
    /* 再生開始時刻からの経過時間で現在のフレームを割り出し */
    unsigned int elapseInMs = timeGetTime() - playStartTime_;
    unsigned int nowInMs = timeStartTime_ + elapseInMs;

    /* 終了フレームを越えていたら、ループ */
    if( nowInMs > endTime_ )
    {
        nowInMs = startTime_;
        playStartTime_ = timeGetTime();
        timeStartTime_ = startTime_;
        //playStartFrame_ = qtimeToFrame(startTime_);
        /* TODO ループ開始時に音楽用のイベントを作成 */        
        emit( stopMusic() );
        emit( startMusic( nowInMs ));
    }
    /* シーンループ中でシーン終了を越えていたらループ */
    if( isLoopScene_ && (nowInMs > nlScene_timeInMs( currentScene_ + 1)) )
    {
        nowInMs = nlScene_timeInMs(currentScene_);
        playStartTime_ = timeGetTime();
        timeStartTime_ = nowInMs;
        //playStartFrame_ = qtimeToFrame(startTime_);
        /* TODO ループ開始時に音楽用のイベントを作成 */        
        emit( stopMusic() );
        emit( startMusic( nowInMs ));
    }

    /* ループ開始からの時間をスライダーの値に反映する */
    ui_->sliderTimeline->setValue( nowInMs );
    /* フレーム変更シグナルを飛ばす */
    onTimeChange( nowInMs );
}
/**/
void TimeLineDock::onChangeScene(int sceneIndex)
{
    int sceneStartTimeInMs = nlScene_timeInMs(sceneIndex);
    //QTime targetTime = frameToQtime(sceneStartFrame);
    /* 再生時間帯を超えていたら移動させる */
    /*onChangeStartTime( MN(targetTime,startTime_) );
    ui_->timeStartTL->setTime(MN(targetTime,startTime_));
    onChangeEndTime( MX(targetTime,endTime_) );
    ui_->timeEndTL->setTime(MX(targetTime,endTime_));*/
    /* 現在のフレームに移動させる */
    ui_->sliderTimeline->setValue(sceneStartTimeInMs);
}

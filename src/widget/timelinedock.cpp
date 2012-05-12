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

    /* TODO �O��̐ݒ�l���擾���� */
    /* HACK �Ƃ肠����0�b����190�b�܂ł�ݒ� */
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
    /* ��x���y���~�߂� */
    if( isPlay_ ){emit(stopMusic());}
    /**/
    const int timeInMs = MX( ui_->sliderTimeline->value()+deltaInMs, 0);
    timeStartTime_ = timeInMs;
    ui_->sliderTimeline->setValue(timeInMs);
    /* ���y���ĊJ������ */
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
    /* ����͍Đ��{�^�����������Ƃ��Ɠ������� */
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
        /* ���y���ĊJ������ */
        emit(startMusic(timeInMs));
    }
    else
    { 
        timer_->stop();
        playStartTime_ = 0x80000000; 
        /* ���y���~������ */
        emit(stopMusic());
    }	
}
/**/
void TimeLineDock::onChangeStartTime( int newStartTime )
{
    /* �t���[���v�Z���ʂ��������ߒ�~��Ԃɂ��� */
    if( isPlay_ ){ onPlayPushed(); };

    /* �I�����Ԃ��z���Ȃ��悤�ɐݒ肷�� */
    ui_->timeEndTL->setMinimum( newStartTime );
    startTime_ = newStartTime;
    /* �X���C�_�[�̉�����ύX */
    ui_->sliderTimeline->setMinimum( newStartTime );
}
/**/
void TimeLineDock::onChangeEndTime( int newEndTime )
{
    /* �t���[���v�Z���ʂ��������ߒ�~��Ԃɂ��� */
    if( isPlay_ ){ onPlayPushed(); };

    /* �J�n���Ԃ��z���Ȃ��悤�ɐݒ肷�� */
    ui_->timeStartTL->setMaximum( newEndTime );
    endTime_ = newEndTime;
    /* �X���C�_�[�̏����ύX */
    ui_->sliderTimeline->setMaximum( newEndTime );
}
/**/
void TimeLineDock::onPressSlider()
{
    /* �^�C�}�[�쓮���Ȃ��A�̓�����~���� */
    if( isPlay_ )
    {
        onPlayPushed(); 
    };
    /* ���y���~������ */
    emit( stopMusic() );
}
/**/
void TimeLineDock::onReleaseSlider()
{
}
/**/
void TimeLineDock::onAdvanceTime()
{
    /* �Đ��J�n��������̌o�ߎ��ԂŌ��݂̃t���[��������o�� */
    unsigned int elapseInMs = timeGetTime() - playStartTime_;
    unsigned int nowInMs = timeStartTime_ + elapseInMs;

    /* �I���t���[�����z���Ă�����A���[�v */
    if( nowInMs > endTime_ )
    {
        nowInMs = startTime_;
        playStartTime_ = timeGetTime();
        timeStartTime_ = startTime_;
        //playStartFrame_ = qtimeToFrame(startTime_);
        /* TODO ���[�v�J�n���ɉ��y�p�̃C�x���g���쐬 */        
        emit( stopMusic() );
        emit( startMusic( nowInMs ));
    }
    /* �V�[�����[�v���ŃV�[���I�����z���Ă����烋�[�v */
    if( isLoopScene_ && (nowInMs > nlScene_timeInMs( currentScene_ + 1)) )
    {
        nowInMs = nlScene_timeInMs(currentScene_);
        playStartTime_ = timeGetTime();
        timeStartTime_ = nowInMs;
        //playStartFrame_ = qtimeToFrame(startTime_);
        /* TODO ���[�v�J�n���ɉ��y�p�̃C�x���g���쐬 */        
        emit( stopMusic() );
        emit( startMusic( nowInMs ));
    }

    /* ���[�v�J�n����̎��Ԃ��X���C�_�[�̒l�ɔ��f���� */
    ui_->sliderTimeline->setValue( nowInMs );
    /* �t���[���ύX�V�O�i�����΂� */
    onTimeChange( nowInMs );
}
/**/
void TimeLineDock::onChangeScene(int sceneIndex)
{
    int sceneStartTimeInMs = nlScene_timeInMs(sceneIndex);
    //QTime targetTime = frameToQtime(sceneStartFrame);
    /* �Đ����ԑт𒴂��Ă�����ړ������� */
    /*onChangeStartTime( MN(targetTime,startTime_) );
    ui_->timeStartTL->setTime(MN(targetTime,startTime_));
    onChangeEndTime( MX(targetTime,endTime_) );
    ui_->timeEndTL->setTime(MX(targetTime,endTime_));*/
    /* ���݂̃t���[���Ɉړ������� */
    ui_->sliderTimeline->setValue(sceneStartTimeInMs);
}

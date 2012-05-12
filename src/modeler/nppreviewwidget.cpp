#include "main/stdafx.h"
#include "npPreviewWidget.h"
#include "shared/mesh/nlPoly.h"
#include "ui_nlModel.h"
#include "npModelCreator.h"
#include "npDxRender.h"
#include "log.h"
#include <boost/make_shared.hpp>


using namespace std;
using namespace boost;

/**/
Ui_MainWindow* g_ui = NULL;

/**/
npPreviewWidget::npPreviewWidget( QWidget *parent)
    :QWidget( parent ),
    nowDragging(false),
    eyeTheta(D3DX_PI*0.25f),
    eyeFai(D3DX_PI*0.25f),
    eyeDistance(3.0f)
{
    /* D3D11������������ */
    dxRender_ = boost::make_shared<npDxRender>(winId());

    /* ���݂̑Ώۃt�@�C����ݒ肷�� */
    targetFileNameStr.resize(MAX_PATH);
    string configPath = (QCoreApplication::applicationDirPath() + QString("./modeler.ini")).toStdString();
    GetPrivateProfileStringA( "config", "target", "", const_cast<char*>(targetFileNameStr.c_str()), targetFileNameStr.size(), configPath.c_str() );

    /* �t�@�C���ύX�����m������ */
    watcher_.addPath( QCoreApplication::applicationDirPath() + QString(targetFileNameStr.c_str()) );
    connect(&watcher_, SIGNAL( fileChanged (const QString&)), this, SLOT(onFileChange(const QString&)));

    /* ���݂̑Ώۃt�@�C����W�J���� */
    onFileChange(targetFileNameStr.c_str());

    /**/
    setAttribute(Qt::WA_PaintOnScreen, true );
    /**/
    setAcceptDrops(true);
}
/**/
void npPreviewWidget::setUi(class Ui_MainWindow* ui)
{ 
    g_ui = ui;
}
/**/
void npPreviewWidget::onFileChange( const QString& path)
{
    /**/
    watcher_.addPath( QCoreApplication::applicationDirPath()+"/test.txt" );
    /* �O��̓ǂݍ��݂����莞�ԗ����Ȃ��ƃ��[�h�ł��Ȃ� */
    time_t curTime;
    time( &curTime );
    time_t elapse = curTime - prevLoadTime;
    prevLoadTime = curTime;
    if( elapse < 1 )
    { return ;}
    /**/
    npTriangleMesh* triMesh = new npTriangleMesh();
    loadNewModel(path, triMesh );
    /**/
    dxRender_->setNewMesh( triMesh );
    delete triMesh;
}
/**/
void npPreviewWidget::Rendering()
{
    /* ���_�ʒu�̌v�Z */
    float eyePos[3] = 
    {
        eyeDistance*sin(eyeTheta)*cos(eyeFai),
        eyeDistance*cos(eyeTheta),
        eyeDistance*sin(eyeTheta)*sin(eyeFai)
    };
    /**/
    dxRender_->render(eyePos);
}
/**/
void npPreviewWidget::mousePressEvent(QMouseEvent* e)
{
    /**/
    if( Qt::RightButton == e->button() )
    {
        dragStart = e->pos();
        nowDragging = true;
        /* �ĕ`�� */
        repaint(QRect());
    }
}
void npPreviewWidget::mouseMoveEvent(QMouseEvent* e)
{
    if( nowDragging )
    {
        /* ���_�ʒu���ړ����� */
        QPoint curPos = e->pos();
        QPoint diff = dragStart - curPos;
        dragStart = curPos;
        eyeFai   -= diff.x() * 0.01f;
        eyeTheta -= diff.y() * 0.01f;
        /* fai�͖������Btheta��[-PI/2+d,PI/2-d]�̊� */
        const float d = 0.1f;
        eyeTheta = min( max( eyeTheta, -D3DX_PI*1.0f+d), D3DX_PI*0.0f-d);
        /* �ĕ`�� */
        repaint(QRect());
        ///**/
        //QString msg;
        //msg.sprintf("fai:%2.2f theta:%2.2f", eyeFai,eyeTheta);
        //log(msg);
    }
}
void npPreviewWidget::mouseReleaseEvent (QMouseEvent* e)
{
    if( Qt::RightButton == e->button() )
    {
        nowDragging = false;
        /* �ĕ`�� */
        repaint(QRect());
    }
}
void npPreviewWidget::wheelEvent (QWheelEvent* e)
{
    /* ���_-���S��������ύX */
    const float factor = e->delta() * 0.001f;
    eyeDistance *= (1.0f - factor);
    eyeDistance = min( max( eyeDistance, 0.1f ), 15.0f );
    /* �ĕ`�� */
    repaint(QRect());
}
/**/
void npPreviewWidget::dragEnterEvent(QDragEnterEvent* e)
{
    e->acceptProposedAction();
}
/**/
void npPreviewWidget::dropEvent(QDropEvent* e)
{
    /* �V�����^�[�Q�b�g�ɐݒ肷�� */
    const QMimeData* source = e->mimeData();
    QByteArray data = source->data("FileNameW");
    targetFileNameStr = QString::fromUtf16((ushort*)data.data(), data.size() / 2).toStdString();
    /* �Ď��Ώۂ�V�����^�[�Q�b�g�ɕύX���� */
    watcher_.removePaths(watcher_.files());
    watcher_.addPath( QString(targetFileNameStr.c_str()) );
    /**/
    log( (string("change new file[")+targetFileNameStr+string("]")) );
    /* ���[�h���s�� */
    onFileChange(targetFileNameStr.c_str());
    /**/
    e->acceptProposedAction();
}
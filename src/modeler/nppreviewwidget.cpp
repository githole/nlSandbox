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
    /* D3D11を初期化する */
    dxRender_ = boost::make_shared<npDxRender>(winId());

    /* 現在の対象ファイルを設定する */
    targetFileNameStr.resize(MAX_PATH);
    string configPath = (QCoreApplication::applicationDirPath() + QString("./modeler.ini")).toStdString();
    GetPrivateProfileStringA( "config", "target", "", const_cast<char*>(targetFileNameStr.c_str()), targetFileNameStr.size(), configPath.c_str() );

    /* ファイル変更を検知させる */
    watcher_.addPath( QCoreApplication::applicationDirPath() + QString(targetFileNameStr.c_str()) );
    connect(&watcher_, SIGNAL( fileChanged (const QString&)), this, SLOT(onFileChange(const QString&)));

    /* 現在の対象ファイルを展開する */
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
    /* 前回の読み込みから一定時間立たないとロードできない */
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
    /* 視点位置の計算 */
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
        /* 再描画 */
        repaint(QRect());
    }
}
void npPreviewWidget::mouseMoveEvent(QMouseEvent* e)
{
    if( nowDragging )
    {
        /* 視点位置を移動する */
        QPoint curPos = e->pos();
        QPoint diff = dragStart - curPos;
        dragStart = curPos;
        eyeFai   -= diff.x() * 0.01f;
        eyeTheta -= diff.y() * 0.01f;
        /* faiは無制限。thetaは[-PI/2+d,PI/2-d]の間 */
        const float d = 0.1f;
        eyeTheta = min( max( eyeTheta, -D3DX_PI*1.0f+d), D3DX_PI*0.0f-d);
        /* 再描画 */
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
        /* 再描画 */
        repaint(QRect());
    }
}
void npPreviewWidget::wheelEvent (QWheelEvent* e)
{
    /* 視点-中心距離感を変更 */
    const float factor = e->delta() * 0.001f;
    eyeDistance *= (1.0f - factor);
    eyeDistance = min( max( eyeDistance, 0.1f ), 15.0f );
    /* 再描画 */
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
    /* 新しいターゲットに設定する */
    const QMimeData* source = e->mimeData();
    QByteArray data = source->data("FileNameW");
    targetFileNameStr = QString::fromUtf16((ushort*)data.data(), data.size() / 2).toStdString();
    /* 監視対象を新しいターゲットに変更する */
    watcher_.removePaths(watcher_.files());
    watcher_.addPath( QString(targetFileNameStr.c_str()) );
    /**/
    log( (string("change new file[")+targetFileNameStr+string("]")) );
    /* ロードを行う */
    onFileChange(targetFileNameStr.c_str());
    /**/
    e->acceptProposedAction();
}
#ifndef _NPPREVIEWWIDGET_H_
#define _NPPREVIEWWIDGET_H_

/**
 * @brief 
 */
class npPreviewWidget
    :public QWidget
{
    Q_OBJECT
public:
    npPreviewWidget(QWidget *parent = 0 );
    void setUi(class Ui_MainWindow* ui);
    /**/
    QPaintEngine* paintEngine() const
    { return 0;  }
    void paintEvent(QPaintEvent *p)
    { Rendering();}
    void repaintCanvas()
    { Rendering(); }
    void render ( QPaintDevice * target, const QPoint & targetOffset, const QRegion & sourceRegion, RenderFlags renderFlags  )
    { Rendering(); }
    void render ( QPainter * painter, const QPoint & targetOffset, const QRegion & sourceRegion, RenderFlags renderFlags )
    { Rendering(); }
    void repaint ( int x, int y, int w, int h )
    { Rendering(); }
    void repaint ( const QRect & rect )
    { Rendering(); }
    void repaint ( const QRegion & rgn )
    { Rendering(); }
    void Rendering();
    /* マウス関連 */
    void mousePressEvent(QMouseEvent* e);
    void mouseMoveEvent(QMouseEvent* e);
    void wheelEvent (QWheelEvent* e);
    void mouseReleaseEvent (QMouseEvent* e);
    /* Drop関連 */
    void dragEnterEvent(QDragEnterEvent* e);
    void dropEvent(QDropEvent* e);
public slots:
    void onFileChange( const QString& path);
private:
    QFileSystemWatcher watcher_;
    time_t prevLoadTime;
    /**/
    boost::shared_ptr<class npDxRender> dxRender_;
    /**/
    QPoint dragStart;
    bool nowDragging;
    /**/
    float eyeTheta;
    float eyeFai;
    float eyeDistance;
    /**/
    std::string targetFileNameStr;
};

#endif


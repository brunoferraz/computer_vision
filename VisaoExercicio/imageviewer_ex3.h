#ifndef IMAGEVIEWER_EX3_H
#define IMAGEVIEWER_EX3_H

#include <imageviewer.h>

class ImageViewer_ex3 : public ImageViewer
{
    Q_OBJECT
public:
    explicit ImageViewer_ex3(QWidget *parent = 0);
    void open(QString *path =0);
    void adjustImage(float w =0, float h =0);
    void prepareDebug();
    void showResult(QSize imgSize, QVector<QPoint> areaRender);
signals:

public slots:
protected:
    void mouseReleaseEvent(QMouseEvent *ev);
};

#endif // IMAGEVIEWER_EX3_H

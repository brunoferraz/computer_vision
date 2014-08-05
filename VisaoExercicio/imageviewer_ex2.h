#ifndef IMAGEVIEWER_EX2_H
#define IMAGEVIEWER_EX2_H
#include <imageviewer.h>

class ImageViewer_ex2 : public ImageViewer
{
    Q_OBJECT
public:
    explicit ImageViewer_ex2(QWidget *parent = 0);

    void open(QString *path =0);
    void adjustImage(float w =0, float h =0);
    void adjustimageAffineSimilarity();
    void prepareDebug();
    void showResult(QSize imgSize, QVector<QPoint> areaRender);
    MatrixXf Hp;
signals:

public slots:
protected:
    void mouseReleaseEvent(QMouseEvent *ev);
};

#endif // IMAGEVIEWER_EX2_H

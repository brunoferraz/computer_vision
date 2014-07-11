#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QLabel>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QImage>
#include <QVector>
#include "pin.h"
#include <QLayout>
#include <QList>
#include <eigen3/Eigen/Dense>
#include <stdio.h>
#include <iostream>
#include <QRect>

#define MAX_PINS 4

using namespace Eigen;
using namespace std;

struct bounds{
    float top;
    float left;
    float right;
    float bottom;
    float dx;
    float dy;
};

class ImageViewer : public QLabel
{
    Q_OBJECT
    void createPin(QPoint pos= QPoint(0,0));
    QVector<QPoint> getSortedPolygonPoints();
    MatrixXf createTransformMatrix(QVector<QPoint>bp,QVector<QPoint>rp);
    void showResult();
    bounds getImageResultBounds();
public:
    QVector<Pin*> pinlist;
    float* scale;
    bool isDebug;

    MatrixXf H;
    MatrixXf Hi;
    explicit ImageViewer(QWidget *parent = 0);
    QImage imageBase;
    QImage imageResult;

    void open(QString *path = 0);
    void set_Scale(float factor);
    void adjustImage(float w, float h);

signals:

public slots:

protected:
    void mouseReleaseEvent(QMouseEvent *ev);
    void wheelEvent(QWheelEvent *ev);
};

#endif // IMAGEVIEWER_H

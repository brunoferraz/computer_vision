#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QLabel>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QImage>
#include <QVector>
#include "pin.h"
#include "pinmanager.h"
#include <QLayout>
#include <QList>
#include <eigen3/Eigen/Dense>
#include <stdio.h>
#include <iostream>
#include <QRect>
#include <cvlib.h>

#define MAX_PINS 4

using namespace Eigen;
using namespace std;

class ImageViewer : public QLabel
{
    Q_OBJECT
    void showResult(QSize imgSize, QVector<QPoint> areaRender);
public:
    QVector<Pin*> pinlist;
    PinManager *pinmanager;
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
    void chooseArea();
signals:

public slots:

protected:
    void mouseReleaseEvent(QMouseEvent *ev);
    void wheelEvent(QWheelEvent *ev);
};

#endif // IMAGEVIEWER_H

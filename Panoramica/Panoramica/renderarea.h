#ifndef RENDERAREA_H
#define RENDERAREA_H

#include <QLabel>
#include <eigen3/Eigen/Dense>
#include <QDebug>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <cvlib.h>
#include <stdio.h>
#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/nonfree/features2d.hpp"

using namespace Eigen;
using namespace cv;
using namespace std;
class RenderArea : public QLabel
{
    Q_OBJECT
public:
    explicit RenderArea(QWidget *parent = 0);
    QVector<Vector3f> pointList;
    QVector<Vector3f> pointListNormalized;
    float pointSize;
    bounds limits;
    Vector3f centroid;
    Matrix3f H;
    Mat descriptors;

    Vector3f getCentroid();
    void normalize();
    QVector<Vector3f> getNormalizedPoints();
    void addPoint(float px, float py);
    void findPoints();

signals:
    void renderAreaClicked(QMouseEvent *ev);

public slots:
    void mouseReleaseEvent(QMouseEvent *ev);
    void paintEvent(QPaintEvent *ev);
};

#endif // RENDERAREA_H

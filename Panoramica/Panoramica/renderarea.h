#ifndef RENDERAREA_H
#define RENDERAREA_H

#include <QLabel>
#include <eigen3/Eigen/Dense>
#include <QDebug>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <cvlib.h>

using namespace Eigen;
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

    Vector3f getCentroid();
    void normalize();
    QVector<Vector3f> getNormalizedPoints();
    void addPoint(float px, float py);

signals:
    void renderAreaClicked(QMouseEvent *ev);

public slots:
    void mouseReleaseEvent(QMouseEvent *ev);
    void paintEvent(QPaintEvent *ev);
};

#endif // RENDERAREA_H

#ifndef CVLIB_H
#define CVLIB_H

#include <eigen3/Eigen/Dense>
#include <QVector>
#include <QPoint>
#include <QImage>
#include <QRect>
#include <QSize>

struct bounds{
    float top;
    float left;
    float right;
    float bottom;
    float dx;
    float dy;
};

using namespace Eigen;
using namespace std;

class CVlib
{
public:
    CVlib();
//    static bounds getImageResultBounds();
    static MatrixXf calculate_H(QVector<QPoint> bp, QVector<QPoint> rp);
    static MatrixXf homography(MatrixXf v,  Matrix3f h); //Receiver vector and transform Matrix
    static QImage generateImage(QImage imageBase, QSize size, QVector<QPoint> renderArea, Matrix3f h);
    static bounds getHomographyBounds(QVector<QPoint> bp, Matrix3f H);
};

#endif // CVLIB_H

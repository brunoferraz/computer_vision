#ifndef CVLIB_H
#define CVLIB_H

#include <eigen3/Eigen/Dense>
#include <QVector>
#include <QPoint>
#include <QImage>
#include <QRect>

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
    static QImage generateImage(QImage imageBase, QRect rect, Matrix3f h);

};

#endif // CVLIB_H

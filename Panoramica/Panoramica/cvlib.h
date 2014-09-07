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
    float centroidX;
    float centroidY;
    float radius;
};

using namespace Eigen;
using namespace std;

class CVlib
{
public:
    CVlib();
//    static bounds getImageResultBounds();
    static MatrixXf calculate_H(QVector <Vector3f> bp, QVector <Vector3f> rp);
    static MatrixXf homography(MatrixXf v,  Matrix3f h); //Receiver vector and transform Matrix
    static QImage generateImage(QImage imageBase, Matrix3f h, QVector<Vector3f> *renderArea = 0);
    static bounds getHomographyBounds(QVector<Vector3f> bp, Matrix3f H);
    static bounds getBounds(QVector<Vector3f> bp);
    static QVector<Vector3f> divideByW(QVector<Vector3f> list);
};

#endif // CVLIB_H

#include "cvlib.h"
#include <eigen3/Eigen/Dense>

#include <stdio.h>
#include <iostream>


using namespace Eigen;
using namespace std;
CVlib::CVlib()
{
}

MatrixXf CVlib::calculate_H(QVector<QPoint> bp, QVector<QPoint> rp)
{
    //CREATE MATRIX
    MatrixXf m(8, 8);
    for(int i = 0; i< rp.count(); i++){
        MatrixXf lineA(1,8);
        lineA << bp[i].x(), bp[i].y(), 1, 0, 0, 0, -(bp[i].x()*rp[i].x()), -(bp[i].y()*rp[i].x());
        MatrixXf lineB(1,8);
        lineB << 0, 0, 0, bp[i].x(), bp[i].y(), 1, -(bp[i].x()*rp[i].y()), -(bp[i].y()*rp[i].y());
        m.row(i*2) << lineA;
        m.row(i*2 + 1) << lineB;
    }
    MatrixXf A  = m.inverse();
    MatrixXf B(8,1);
    for(int i = 0; i< rp.count(); i++){
        B.row(i*2) << rp.at(i).x();
        B.row(i*2 +1) << rp.at(i).y();
    }
    MatrixXf v = A*B;
    Matrix3f h;

    h << v(0), v(1), v(2), v(3), v(4), v(5), v(6), v(7), 1;

    return h;
}

MatrixXf CVlib::homography(MatrixXf v, Matrix3f h)
{
    MatrixXf r(3,1);
    r = h * v;
    r = r/r(2);
    return r;
}

QImage CVlib::generateImage(QImage imageBase, QRect rect, Matrix3f h)
{
    QImage imageResult = QImage(rect.width(), rect.height(), QImage::Format_ARGB32);
//    bounds limits = getImageResultBounds();

//    float stepX = limits.dx / n_width;
//    float stepY = limits.dy / n_height;

//    for(int j = 0; j<imageResult.height(); j++){
//        for(int i = 0; i<imageResult.width(); i++){
//            MatrixXf p(3,1);
//            p << (limits.left + stepX * i) , (limits.top + stepY * j), 1;
//            MatrixXf r(3,1);
//            r= CVlib::homography(p, Hi);
//            QPoint p0(r(0),r(1));

//            QRgb color = qRgb(0, 0, 0);
//            if((p0.x() >=0 && p0.x() < imageBase.width())&&(p0.y()>=0 && p0.y()<imageBase.height())){
//                color = imageBase.pixel(p0);
//            }

//            imageResult.setPixel(QPoint(i, j), color);
//        }
//     }
    return imageResult;
}

//bounds CVlib::getImageResultBounds()
//{

//}

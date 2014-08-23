#include "cvlib.h"
#include <eigen3/Eigen/Dense>

#include <stdio.h>
#include <iostream>
#include <QDebug>
#include <QSize>
#include <QColor>
#include <QRgb>
using namespace Eigen;
using namespace std;
CVlib::CVlib()
{

}

MatrixXf CVlib::calculate_H(QVector <Vector3f> bp, QVector <Vector3f> rp)
{
    //CREATE MATRIX
    MatrixXf m(8, 8);
    for(int i = 0; i< rp.count(); i++){
        MatrixXf lineA(1,8);
        lineA << bp[i](0), bp[i](1), 1, 0, 0, 0, -(bp[i](0)*rp[i](0)), -(bp[i](1)*rp[i](0));
        MatrixXf lineB(1,8);
        lineB << 0, 0, 0, bp[i](0), bp[i](1), 1, -(bp[i](0)*rp[i](1)), -(bp[i](1)*rp[i](1));
        m.row(i*2) << lineA;
        m.row(i*2 + 1) << lineB;
    }
    MatrixXf A  = m.inverse();
    MatrixXf B(8,1);
    for(int i = 0; i< rp.count(); i++){
        B.row(i*2) << rp.at(i)(0);
        B.row(i*2 +1) << rp.at(i)(1);
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

QImage CVlib::generateImage(QImage imageBase, Matrix3f h, QVector<QPoint> *renderArea)
{
    //TODO ImageSize must be calculated based on Bounds
    //QImage imageResult = QImage(size.width(), size.height(), QImage::Format_ARGB32);
    QVector<QPoint> areaTemp;
    if(renderArea==0){
        areaTemp.push_back(QPoint(0,0));
        areaTemp.push_back(QPoint(0,imageBase.height()));
        areaTemp.push_back(QPoint(imageBase.width(),imageBase.height()));
        areaTemp.push_back(QPoint(imageBase.width(),0));
    }else{
        areaTemp = *renderArea;
    }
    QVector<Vector3f> area;
    area.push_back(Vector3f(areaTemp.at(0).x(), areaTemp.at(0).y(), 1));
    area.push_back(Vector3f(areaTemp.at(1).x(), areaTemp.at(1).y(), 1));
    area.push_back(Vector3f(areaTemp.at(2).x(), areaTemp.at(2).y(), 1));
    area.push_back(Vector3f(areaTemp.at(3).x(), areaTemp.at(3).y(), 1));

    bounds limits = CVlib::getHomographyBounds(area, h);

    QVector<Vector3f> areaBack;
    areaBack.push_back(Vector3f(limits.left, limits.top, 1));
    areaBack.push_back(Vector3f(limits.left, limits.bottom, 1));
    areaBack.push_back(Vector3f(limits.right, limits.bottom, 1));
    areaBack.push_back(Vector3f(limits.right, limits.top, 1));

    float ratio = limits.dx/limits.dy;

    bounds limit_back = CVlib::getHomographyBounds(areaBack, h.inverse());
    //QSize size = QSize(limit_back.dx, limit_back.dy);
    QSize size = QSize(limit_back.dx, limit_back.dy);
    qDebug() << ratio;
    QImage imageResult = QImage(size.width(), size.height(), QImage::Format_ARGB32);

    float factor = 1;
    float stepX = limits.dx / size.width() / factor;
    float stepY = limits.dy / size.height() /factor;

    Matrix3f hi = h.inverse();
    for(int j = 0; j<imageResult.height(); j++){
        for(int i = 0; i<imageResult.width(); i++){
            MatrixXf p(3,1);
            p << (limits.left + stepX * i) , (limits.top + stepY * j), 1;
            MatrixXf r(3,1);
            r= CVlib::homography(p, hi);
            QPoint p0(r(0),r(1));
            QColor color(0, 0, 0);
            MatrixXf finalColor(3,1);
            finalColor << 0, 0, 0;
            float ratio = 1;
            if((p0.x() >=0 && p0.x() < imageBase.width())&&(p0.y()>=0 && p0.y()<imageBase.height())){
                ratio = 0;
                for(int offSetY = -1; offSetY <= 1; offSetY++){
                    for(int offSetX = -1; offSetX <= 1; offSetX++){
                        QPoint ptemp(p0.x() + offSetX, p0.y() + offSetY);
                        float ratioX =  r(0)-ptemp.x();
                        float ratioY =  r(1)-ptemp.y();
                        float dist   = std::abs(1 - std::sqrt(std::pow(ratioX, 2) + std::pow(ratioY,2)));
                        ratio += dist;
                        if((ptemp.x() >=0 && ptemp.x() < imageBase.width())&&(ptemp.y()>=0 && ptemp.y()<imageBase.height())){
                            QColor ctemp = imageBase.pixel(ptemp.x(), ptemp.y());
                            MatrixXf cmTemp(3,1);
                            cmTemp << ctemp.redF(), ctemp.greenF(), ctemp.blueF();
                            finalColor += cmTemp * dist;
                        }
                    }
                }
            }
            finalColor = finalColor/ratio;
            color.setRgb(finalColor(0) * 255, finalColor(1) * 255, finalColor(2) * 255);
            imageResult.setPixel(QPoint(i, j), color.rgb());
        }
     }
    return imageResult;
}

bounds CVlib::getHomographyBounds(QVector<Vector3f> bp, Matrix3f H)
{
    MatrixXf p(3,1);
    p << bp.at(0)(0), bp.at(0)(1), 1;
    MatrixXf itopLeft(3,1);
    itopLeft = H * p;
    itopLeft = itopLeft/itopLeft(2);

    p << bp.at(3)(0), bp.at(3)(1),1;
    MatrixXf itopRight(3,1);
    itopRight = H * p;
    itopRight = itopRight/itopRight(2);

    p << bp.at(1)(0), bp.at(1)(1),1;
    MatrixXf ibottomLeft(3,1);
    ibottomLeft = H * p;
    ibottomLeft = ibottomLeft/ibottomLeft(2);

    p << bp.at(2)(0), bp.at(2)(1),1;
    MatrixXf ibottomRight(3,1);
    ibottomRight = H * p;
    ibottomRight = ibottomRight/ibottomRight(2);

    bounds limits;
    limits.top = std::min(itopLeft(1),itopRight(1));
    limits.left = std::min(itopLeft(0),ibottomLeft(0));
    limits.right = std::max(ibottomRight(0), itopRight(0));
    limits.bottom = std::max(ibottomRight(1), ibottomLeft(1));
    limits.dx = limits.right - limits.left;
    limits.dy = limits.bottom - limits.top;

    return limits;
}


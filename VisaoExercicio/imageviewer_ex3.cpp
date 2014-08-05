#include "imageviewer_ex3.h"
#include <QDebug>
#include <QMouseEvent>
#include <QFileDialog>
#include <QString>
#include <QPoint>
#include <pin.h>
#include <QPoint>
#include <QLayout>
#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/LU>
#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/SVD>
#include <stdio.h>
#include <iostream>
#include <QRgb>
#include <QColor>
#include <QWindow>
#include <cvlib.h>

ImageViewer_ex3::ImageViewer_ex3(QWidget *parent) :
    ImageViewer(parent)
{
    scale = new float(1.0);
    isDebug = false;
    pinmanager = new PinManager(this);
    pinmanager->setType(pinmanager->TYPE_FIVE);
}

void ImageViewer_ex3::open(QString *path)
{
    ImageViewer::open(path);
    isDebug = false;
    if(isDebug){
        prepareDebug();
    }
}

void ImageViewer_ex3::adjustImage(float w, float h)
{
    int pairs = pinmanager->linelist.count()/2;
    MatrixXf A(pairs, 5);
    VectorXf b(5,1);
    for(int i = 0; i < pairs; i++){
        int pos = i * 2;
        Vector3f l(3,1);
        Vector3f m(3,1);
        MatrixXf r(1,5);
        l = pinmanager->getLine(pos);
        m = pinmanager->getLine(pos + 1);

        r <<    l(0) * m(0),
                (l(0) * m(1) + l(1) * m(0))/2,
                l(1) * m(1),
                (l(0) * m(2) + l(2) * m(0))/ 2,
                (l(1)*m(2) + l(2)*m(1))/2;
        A.row(i) << r;
        b.row(i) << -l(2)*m(2);
    }
    //A.row(pairs) << 0, 0, 0, 0, 0, 0;
//    JacobiSVD<MatrixXf> SVD(A, ComputeFullV);
//    VectorXf S = SVD.matrixV().col(SVD.matrixV().cols() - 1);
    MatrixXf S(5,1);
    S = A.colPivHouseholderQr().solve(b);

    std::cout<< S <<std::endl;

    Matrix3f C;
    C << S(0), S(1)/2, S(3)/2,
         S(1)/2, S(2), S(4)/2,
         S(3)/2, S(4)/2, 1;

    Matrix2f kkt;
    kkt << C(0), C(1),
            C(3), C(4);

    MatrixXf vKKt(1,2);
    vKKt << C(6), C(7);

    MatrixXf V(1,2);
    V = vKKt * kkt.inverse();

    LLT<MatrixXf> lltOfA(kkt);
    MatrixXf L = lltOfA.matrixU();
    MatrixXf M (3,3);
    M << L(0,0), L(0,1),0, L(1,0), L(1,1),0, V(0), V(1), 1;
//    M << 1, 0, 0,
//         0, 1, 0,
//         V(0), V(1), 1;
    H = M.inverse();

    QSize imgSize(this->width(), this->height());
    QVector<QPoint> areaRender;
    areaRender << QPoint(0,0) << QPoint(0, imgSize.height()) << QPoint(imgSize.width(), imgSize.height()) << QPoint(imgSize.width(), 0);
    showResult(imgSize, areaRender);

    std::cout << V << std::endl;
}

void ImageViewer_ex3::prepareDebug()
{
    pinmanager->createPin(QPoint(249,111));
    pinmanager->createPin(QPoint(248,204));
    pinmanager->createPin(QPoint(176,154));
    pinmanager->createPin(QPoint(321,152));
    pinmanager->createPin(QPoint(221,188));
    pinmanager->createPin(QPoint(295,136));
    pinmanager->createPin(QPoint(202,140));
    pinmanager->createPin(QPoint(276,186));
    pinmanager->createPin(QPoint(28,81));
    pinmanager->createPin(QPoint(191,77));
    pinmanager->createPin(QPoint(95,24));
    pinmanager->createPin(QPoint(35,156));
    pinmanager->createPin(QPoint(399,20));
    pinmanager->createPin(QPoint(460,149));
    pinmanager->createPin(QPoint(353,46));
    pinmanager->createPin(QPoint(459,45));
    pinmanager->createPin(QPoint(44,272));
    pinmanager->createPin(QPoint(161,271));
    pinmanager->createPin(QPoint(109,226));
    pinmanager->createPin(QPoint(89,323));
}

void ImageViewer_ex3::showResult(QSize imgSize, QVector<QPoint> areaRender)
{
    ImageViewer::showResult(imgSize, areaRender);
    pinmanager->hide_pins();
}

void ImageViewer_ex3::mouseReleaseEvent(QMouseEvent *ev)
{
    if(pinmanager->pinlist.count()<pinmanager->MaxPins){
        pinmanager->createPin(ev->pos());
    }else{
        //ImageViewer::printPoints();
        adjustImage(this->width(), this->height());
    }
}

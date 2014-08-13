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
#include <mainwindow.h>

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
//    isDebug = false;
    //this->parentWidget()->parentWidget()->parentWidget()->resize(QSize(501, 300));
    if(isDebug){
        prepareDebug();
    }
}

void ImageViewer_ex3::adjustImage(float w, float h)
{
    int pairs = pinmanager->linelist.count()/2;

    //SVD 6
//    MatrixXf A(pairs + 1, 6);
//    for(int i = 0; i < pairs; i++){
//        int pos = i * 2;
//        Vector3f l(3,1);
//        Vector3f m(3,1);
//        MatrixXf r(1,6);
//        l = pinmanager->getLine(pos);
//        m = pinmanager->getLine(pos + 1);

//        r <<    l(0) * m(0),
//                (l(0) * m(1) + l(1) * m(0))/2,
//                l(1) * m(1),
//                (l(0) * m(2) + l(2) * m(0))/ 2,
//                (l(1)*m(2) + l(2)*m(1))/2,
//                l(2)*m(2);
//        A.row(i) << r;
//    }
//    A.row(pairs) << 0, 0, 0, 0, 0, 1;
//    JacobiSVD<MatrixXf> SVD(A, ComputeFullV);
//    VectorXf x = SVD.matrixV().col(SVD.matrixV().cols() - 1);

    //SVD 5
    MatrixXf A(pairs, 6);
    for(int i = 0; i < pairs; i++){
        int pos = i * 2;
        Vector3f l(3,1);
        Vector3f m(3,1);
        MatrixXf r(1,6);
        l = pinmanager->getLine(pos);
        m = pinmanager->getLine(pos + 1);
        l /=l(2);
        m /=m(2);

        r <<    l(0) * m(0),
                (l(0) * m(1) + l(1) * m(0))/2,
                l(1) * m(1),
                (l(0) * m(2) + l(2) * m(0))/ 2,
                (l(1)*m(2) + l(2)*m(1))/2,
                l(2)*m(2);
        A.row(i) << r;
    }
    JacobiSVD<MatrixXf> SVD(A, ComputeFullV);
    VectorXf x = SVD.matrixV().col(SVD.matrixV().cols() - 1);
    x/= x(2);

    //QR DECOMPOSITION
//    MatrixXf A(pairs, 5);
//    VectorXf b(5,1);
//    for(int i = 0; i < pairs; i++){
//        int pos = i * 2;
//        Vector3f l(3,1);
//        Vector3f m(3,1);
//        MatrixXf r(1,5);
//        l = pinmanager->getLine(pos);
//        m = pinmanager->getLine(pos + 1);

//        r <<    l(0) * m(0),
//                (l(0) * m(1) + l(1) * m(0))/2,
//                l(1) * m(1),
//                (l(0) * m(2) + l(2) * m(0))/ 2,
//                (l(1)*m(2) + l(2)*m(1))/2;
//        A.row(i) << r;
//        b.row(i) << -l(2)*m(2);
//    }
//    MatrixXf x(5,1);
//    x = A.colPivHouseholderQr().solve(b);

    Matrix3f C;
    C << x(0), x(1)/2, x(3)/2,
         x(1)/2, x(2), x(4)/2,
         x(3)/2, x(4)/2, 1;

    Matrix2f kkt;
    kkt << C(0,0), C(0,1),
           C(1,0), C(1,1);

    MatrixXf vKKt(1,2);
    vKKt << C(2,0), C(2,1);

    MatrixXf V(1,2);
    V = vKKt * kkt.inverse();

    LLT<MatrixXf> llt(kkt);
    MatrixXf L = llt.matrixU();
    MatrixXf M (3,3);
    M << L(0,0), L(0,1),0, L(1,0), L(1,1),0, V(0), V(1), 1;
//    M << 1, 0, 0,
//         0, 1, 0,
//         V(0), V(1), 1;
    H = M.inverse();

    QSize imgSize(w, h);
    //bounds limits = CVlib::getHomographyBounds(renderArea,H);
    QVector<QPoint> areaRender;
    areaRender << QPoint(0,0) << QPoint(0, imgSize.height()) << QPoint(imgSize.width(), imgSize.height()) << QPoint(imgSize.width(), 0);
    showResult(imgSize, areaRender);
    std::cout << H << std::endl;
}

void ImageViewer_ex3::prepareDebug()
{

    //PONTOS DO LUIZ
//    pinmanager->createPin(QPoint(15,104));
//    pinmanager->createPin(QPoint(134,48));
//    pinmanager->createPin(QPoint(67,78));
//    pinmanager->createPin(QPoint(175,153));
//    pinmanager->createPin(QPoint(35,156));
//    pinmanager->createPin(QPoint(167,275));
//    pinmanager->createPin(QPoint(7,259));
//    pinmanager->createPin(QPoint(165,163));
//    pinmanager->createPin(QPoint(267,337));
//    pinmanager->createPin(QPoint(409,200));
//    pinmanager->createPin(QPoint(252,205));
//    pinmanager->createPin(QPoint(445,337));
//    pinmanager->createPin(QPoint(264,193));
//    pinmanager->createPin(QPoint(378,108));
//    pinmanager->createPin(QPoint(307,73));
//    pinmanager->createPin(QPoint(455,144));
//    pinmanager->createPin(QPoint(118,113));
//    pinmanager->createPin(QPoint(379,108));
//    pinmanager->createPin(QPoint(248,46));
//    pinmanager->createPin(QPoint(250,204));

    //PONTOS DO MARROQUIM
//    pinmanager->createPin(QPoint(57,26));
//    pinmanager->createPin(QPoint(300,204));
//    pinmanager->createPin(QPoint(304,29));
//    pinmanager->createPin(QPoint(104,200));
//    pinmanager->createPin(QPoint(559,273));
//    pinmanager->createPin(QPoint(384,270));
//    pinmanager->createPin(QPoint(457,320));
//    pinmanager->createPin(QPoint(495,210));
//    pinmanager->createPin(QPoint(141,314));
//    pinmanager->createPin(QPoint(165,396));
//    pinmanager->createPin(QPoint(228,362));
//    pinmanager->createPin(QPoint(81,358));
//    pinmanager->createPin(QPoint(450,319));
//    pinmanager->createPin(QPoint(576,413));
//    pinmanager->createPin(QPoint(586,331));
//    pinmanager->createPin(QPoint(363,426));
//    pinmanager->createPin(QPoint(124,127));
//    pinmanager->createPin(QPoint(411,131));
//    pinmanager->createPin(QPoint(303,29));
//    pinmanager->createPin(QPoint(297,317));


    pinmanager->createPin(QPoint(154,451));
    pinmanager->createPin(QPoint(152,21));
    pinmanager->createPin(QPoint(64,320));
    pinmanager->createPin(QPoint(403,336));
    pinmanager->createPin(QPoint(256,37));
    pinmanager->createPin(QPoint(755,128));
    pinmanager->createPin(QPoint(534,91));
    pinmanager->createPin(QPoint(534,340));
    pinmanager->createPin(QPoint(281,617));
    pinmanager->createPin(QPoint(723,567));
    pinmanager->createPin(QPoint(696,347));
    pinmanager->createPin(QPoint(696,515));
    pinmanager->createPin(QPoint(709,343));
    pinmanager->createPin(QPoint(705,120));
    pinmanager->createPin(QPoint(538,211));
    pinmanager->createPin(QPoint(707,232));
    pinmanager->createPin(QPoint(337,229));
    pinmanager->createPin(QPoint(538,248));
    pinmanager->createPin(QPoint(337,327));
    pinmanager->createPin(QPoint(335,118));
}

void ImageViewer_ex3::showResult(QSize imgSize, QVector<QPoint> areaRender)
{
    ImageViewer::showResult(imgSize, areaRender);
    //printPoints();
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

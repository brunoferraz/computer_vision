#include "imageviewer.h"
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

using namespace Eigen;
using namespace std;

ImageViewer::ImageViewer(QWidget *parent) :
    QLabel(parent, false)
{
    scale = new float(1.0);
    isDebug = false;
    pinmanager = new PinManager(this);
    pinmanager->setType(pinmanager->TYPE_ONE);
}
void ImageViewer::open(QString *path){
    QString fileName;
    if(path){
        isDebug = true;
        fileName = *path;
//        pinmanager->createPin(QPoint(221,187));
//        pinmanager->createPin(QPoint(278,200));
//        pinmanager->createPin(QPoint(280,113));
//        pinmanager->createPin(QPoint(223,119));
//        pinmanager->createPin(QPoint(184,27));
//        pinmanager->createPin(QPoint(200,328));
//        pinmanager->createPin(QPoint(368,238));
//        pinmanager->createPin(QPoint(367,67));
    }
    if(!isDebug){
        fileName = QFileDialog::getOpenFileName(this,
                                             tr("Open File"), QDir::currentPath());
    }
    qDebug() << fileName;
    setScaledContents(true);
    imageBase = QImage(fileName);
    setPixmap(QPixmap::fromImage(imageBase));
    pixmap();
}

void ImageViewer::set_Scale(float factor){
    *scale = factor;
    resize(factor * pixmap()->size());
}

void ImageViewer::showResult()
{
    pinmanager->hide_pins();
    float n_width = this->width();
    float n_height = this->height();

    imageResult = QImage(n_width, n_height, QImage::Format_ARGB32);
    bounds limits = getImageResultBounds();

    float stepX = limits.dx / n_width;
    float stepY = limits.dy / n_height;

    for(int j = 0; j<imageResult.height(); j++){
        for(int i = 0; i<imageResult.width(); i++){
            MatrixXf p(3,1);
            p << (limits.left + stepX * i) , (limits.top + stepY * j), 1;
            MatrixXf r(3,1);
            r= CVlib::homography(p, Hi);
            QPoint p0(r(0),r(1));

            QRgb color = qRgb(0, 0, 0);
            if((p0.x() >=0 && p0.x() < imageBase.width())&&(p0.y()>=0 && p0.y()<imageBase.height())){
                color = imageBase.pixel(p0);
            }

            imageResult.setPixel(QPoint(i, j), color);
        }
     }
    setPixmap(QPixmap::fromImage(imageResult));
    pixmap();
}

bounds ImageViewer::getImageResultBounds()
{
    MatrixXf p(3,1);
    p << 0, 0, 1;
    MatrixXf itopLeft(3,1);
    itopLeft = H * p;
    itopLeft = itopLeft/itopLeft(2);

    p << this->width(), 0,1;
    MatrixXf itopRight(3,1);
    itopRight = H * p;
    itopRight = itopRight/itopRight(2);

    p << 0, this->height(),1;
    MatrixXf ibottomLeft(3,1);
    ibottomLeft = H * p;
    ibottomLeft = ibottomLeft/ibottomLeft(2);

    p << this->width(), this->height(),1;
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
    std::cout << limits.top << std::endl;

    return limits;
}

void ImageViewer::mouseReleaseEvent(QMouseEvent *ev){
    if(pinmanager->pinlist.count()<MAX_PINS){
       pinmanager->createPin(ev->pos());
    }else{
        if(isDebug){
            this->adjustImage(8190,6130);
        }
    }
}

void ImageViewer::wheelEvent(QWheelEvent *ev){
    if(ev->delta()>0){
        this->set_Scale(*scale*1.2);
    }else{
        this->set_Scale(*scale/1.2);
    }
    ev->accept();
}

void ImageViewer::adjustImage(float w, float h){
    //verify which is higher
    float sizeFactor = 1;
    float divisor = h;
    if(h>w){
        divisor = w;
    }
    //chage size to ratio;
    w = (w/divisor) * sizeFactor;
    h = (h/divisor) * sizeFactor;

    //create list based on w,h
    QVector<QPoint> rlist;
    rlist.push_back(QPoint(0,0));
    rlist.push_back(QPoint(0,h));
    rlist.push_back(QPoint(w,h));
    rlist.push_back(QPoint(w,0));

    QVector<QPoint> blist = pinmanager->getSortedPolygonPoints();

//    H = calculate_H(blist, rlist);
    H = CVlib::calculate_H(blist, rlist);
    Hi = H.inverse();


    showResult();
}

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

using namespace Eigen;
using namespace std;

ImageViewer::ImageViewer(QWidget *parent) :
    QLabel(parent, false)
{
    scale = new float(1.0);
    isDebug = false;
}
void ImageViewer::open(QString *path){
    QString fileName;
    if(path){
        isDebug = true;
        fileName = *path;
//        this->createPin(QPoint(221,187));
//        this->createPin(QPoint(278,200));
//        this->createPin(QPoint(280,113));
//        this->createPin(QPoint(223,119));
        this->createPin(QPoint(184,27));
        this->createPin(QPoint(200,328));
        this->createPin(QPoint(368,238));
        this->createPin(QPoint(367,67));
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

void ImageViewer::createPin(QPoint pos){
    Pin *pin= new Pin(this, pos);
    pin->show();
    pinlist.push_back(pin);
}

QVector<QPoint> ImageViewer::getSortedPolygonPoints()
{
    QVector<QPoint> blist;
    blist << QPoint(0,0) << QPoint(0,0) << QPoint(0,0) << QPoint(0,0);
    QVector<QPoint> tempList;
    for(int i = 0; i< pinlist.count(); i++)
    {
        tempList.insert(i, pinlist.at(i)->pos());
    }
    //sort points based on X
    bool flag = true;
    while(flag){
        flag = false;
        for(int i=1; i<tempList.count(); i++){
            if(tempList.at(i).x() < tempList.at(i -1).x()){
                flag = true;
                QPoint temp = tempList.at(i-1);
                tempList.replace(i-1,tempList.at(i));
                tempList.replace(i, temp);
            }
        }
    }
    //from this two first get the lowest point as point 0 and the higher as 1
    int indexBottom = 1;
    int indexTop = 0;
    if(tempList.at(0).y()<tempList.at(1).y()){
        indexBottom = 0;
        indexTop = 1;
    }
    blist.replace(indexBottom, tempList.first());
    tempList.pop_front();
    blist.replace(indexTop, tempList.first());
    tempList.pop_front();
    //from the last two get the lowest point as point 1 and the higher as 2
    indexBottom = 3;
    indexTop =2;
    if(tempList.at(0).y()>tempList.at(1).y()){
        indexBottom = 2;
        indexTop = 3;
    }
    blist.replace(indexBottom, tempList.first());
    tempList.pop_front();
    blist.replace(indexTop, tempList.first());
    tempList.pop_front();

    return blist;
}

MatrixXf ImageViewer::createTransformMatrix(QVector<QPoint> bp, QVector<QPoint> rp)
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

void ImageViewer::showResult()
{
    for(int i = 0; i < pinlist.count(); i ++){
        pinlist[i]->hide();
    }

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
            r = Hi * p;
            r = r / r(2);
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
    if(pinlist.count()<MAX_PINS){
        this->createPin(ev->pos());
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

    QVector<QPoint> blist = getSortedPolygonPoints();

    H = createTransformMatrix(blist, rlist);
    Hi = H.inverse();


    showResult();
}

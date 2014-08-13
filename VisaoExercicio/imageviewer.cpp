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
        this->prepareDebug();
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
    parentWidget()->resize(QSize(int(imageBase.width()),int(imageBase.height())));
    parentWidget()->parentWidget()->resize(QSize(int(imageBase.width() + 50),int(imageBase.height() + 50)));
}

void ImageViewer::set_Scale(float factor){
    *scale = factor;
    resize(factor * pixmap()->size());
    parentWidget()->resize(QSize(int(factor * imageBase.width()),int(factor * imageBase.height())));
    parentWidget()->parentWidget()->resize(QSize(int(factor * imageBase.width() + 50),int(factor * imageBase.height() + 50)));
}

void ImageViewer::showResult(QSize imgSize, QVector<QPoint> areaRender)
{
    pinmanager->hide_pins();
    float n_width = imgSize.width();
    float n_height = imgSize.height();
    imageResult = CVlib::generateImage(imageBase, QSize(n_width, n_height), areaRender, H);
    setPixmap(QPixmap::fromImage(imageResult));

    pixmap();
}

void ImageViewer::mouseReleaseEvent(QMouseEvent *ev){
    if(pinmanager->type == pinmanager->TYPE_ONE){
        if(pinmanager->pinlist.count()<MAX_PINS){
           pinmanager->createPin(ev->pos());
        }else{
            if(isDebug){
                this->adjustImage(8190,6130);
            }
        }
    }else  if(pinmanager->type == pinmanager->TYPE_TWO){
        if(pinmanager->pinlist.count()<MAX_PINS){
            pinmanager->createPin(ev->pos());
        }else{
            chooseArea();
        }
    }
}

void ImageViewer::wheelEvent(QWheelEvent *ev){
//    if(ev->delta()>0){
//        this->set_Scale(*scale*1.2);
//    }else{
//        this->set_Scale(*scale/1.2);
//    }
//    qDebug() << *scale;
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

    QSize imgSize(this->width(), this->height());
    QVector<QPoint> areaRender;
    areaRender << QPoint(0,0) << QPoint(0, imgSize.height()) << QPoint(imgSize.width(), imgSize.height()) << QPoint(imgSize.width(), 0);
    showResult(imgSize, areaRender);
    pinmanager->setType(pinmanager->TYPE_TWO);
}

void ImageViewer::chooseArea()
{
    //Existe algum problema na equivalencia entre a homografia e a imagem original. Quando selecina os pontos
    //deve estar havendo alguma imprecisao por conta da resolucao. verificar depois
    QVector <QPoint> blist = pinmanager->getSortedPolygonPoints();
    QVector <QPoint> nlist;
    qDebug() << blist;
    for(int i = 0 ; i < blist.count(); i++){
        MatrixXf temp(3,1);
        temp << blist.at(i).x(),blist.at(i).y(), 1;
        temp = Hi * temp ;
        temp = temp/temp(2);
        nlist.push_back(QPoint(temp(0), temp(1)));
    }
    qDebug() << nlist;
    QSize imgSize(this->width(), this->height());
    imageResult = CVlib::generateImage(imageBase, imgSize, nlist, H);
    setPixmap(QPixmap::fromImage(imageResult));

    pixmap();
}

void ImageViewer::prepareDebug()
{
    //        pinmanager->createPin(QPoint(221,187));
    //        pinmanager->createPin(QPoint(278,200));
    //        pinmanager->createPin(QPoint(280,113));
    //        pinmanager->createPin(QPoint(223,119));
    //        pinmanager->createPin(QPoint(184,27));
    //        pinmanager->createPin(QPoint(200,328));
    //        pinmanager->createPin(QPoint(368,238));
    //        pinmanager->createPin(QPoint(367,67));
}

void ImageViewer::printPoints()
{
    for(int i = 0; i < pinmanager->pinlist.count(); i++)
    {
        qDebug() << pinmanager->pinlist.at(i)->getDrawCenter();
    }
}

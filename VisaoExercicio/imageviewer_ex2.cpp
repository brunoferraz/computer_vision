#include "imageviewer_ex2.h"
#include "pinmanager.h"
#include <QDebug>

ImageViewer_ex2::ImageViewer_ex2(QWidget *parent) :
    ImageViewer(parent)
{
    scale = new float(1.0);
    isDebug = false;
    pinmanager = new PinManager(this);
    pinmanager->setType(pinmanager->TYPE_THREE);
}

void ImageViewer_ex2::open(QString *path)
{
    ImageViewer::open(path);
    if(isDebug){
        prepareDebug();
    }
}

void ImageViewer_ex2::adjustImage(float w, float h)
{
    Vector3f hl(3,1);
    hl = pinmanager->getHorizonLine().transpose();
    hl = hl/ hl(2);

    MatrixXf temp(3,3);
    temp << 1, 0, 0, 0, 1, 0, hl(0), hl(1), hl(2);
    Hp = temp;

    H = Hp;
    Hi = H.inverse();

    QSize imgSize(this->width(), this->height());
    QVector<QPoint> areaRender;
    areaRender << QPoint(0,0) << QPoint(0, imgSize.height()) << QPoint(imgSize.width(), imgSize.height()) << QPoint(imgSize.width(), 0);
    showResult(imgSize, areaRender);
}

void ImageViewer_ex2::mouseReleaseEvent(QMouseEvent *ev)
{
    if(pinmanager->pinlist.count()<pinmanager->MaxPins){
        pinmanager->createPin(ev->pos());
    }else{
        adjustImage(this->width(), this->height());
    }
}
void ImageViewer_ex2::prepareDebug()
{
    pinmanager->createPin(QPoint(160,273));
    pinmanager->createPin(QPoint(379,110));
    pinmanager->createPin(QPoint(90,208));
    pinmanager->createPin(QPoint(308,74));
    pinmanager->createPin(QPoint(186,76));
    pinmanager->createPin(QPoint(409,201));
    pinmanager->createPin(QPoint(341,266));
    pinmanager->createPin(QPoint(118,113));
}

void ImageViewer_ex2::showResult(QSize imgSize, QVector<QPoint> areaRender)
{
    ImageViewer::showResult(imgSize, areaRender);
    pinmanager->hide_pins();
}

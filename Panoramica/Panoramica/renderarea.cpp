#include "renderarea.h"
#include <cvlib.h>

RenderArea::RenderArea(QWidget *parent) :
    QLabel(parent)
{
    pointSize = 4;
}

void RenderArea::normalize()
{
    limits = CVlib::getBounds(pointList);
    pointListNormalized = pointList;
    for(int i =0; i< pointListNormalized.count();i++){
        Vector3f temp;
        temp << pointListNormalized.at(i)(0)-limits.centroidX, pointListNormalized.at(i)(1)-limits.centroidY, pointListNormalized.at(i)(2);
        pointListNormalized.replace(i,temp);
    }
    CVlib::printQVector(pointListNormalized);
    limits = CVlib::getBounds(pointList);
    for(int i =0; i< pointListNormalized.count();i++){
        Vector3f temp;
        temp << pointListNormalized.at(i)(0)/ limits.radius, pointListNormalized.at(i)(1) / limits.radius, 1;
        pointListNormalized.replace(i, temp);
    }
}

QVector<Vector3f> RenderArea::getNormalizedPoints()
{
    //Adjustments needed to get only good points
    return pointListNormalized;
}
void RenderArea::mouseReleaseEvent(QMouseEvent *ev){
    Vector3f point;
    point << ev->x(), ev->y(), 1;
    pointList.push_back(point);
    update();
    if(pointList.count()>=8){
        RenderArea::normalize();
        emit renderAreaClicked(ev);
    }
}

void RenderArea::paintEvent(QPaintEvent *ev){
    QLabel::paintEvent(ev);
    for(int i = 0; i < pointList.count(); i++){
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setBrush(Qt::red);
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(pointList.at(i)(0)-pointSize/2, pointList.at(i)(1)-pointSize/2, pointSize, pointSize);
    }
}

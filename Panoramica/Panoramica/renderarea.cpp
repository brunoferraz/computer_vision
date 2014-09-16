#include "renderarea.h"
#include <cvlib.h>
#include <iostream>
#include <stdio.h>

RenderArea::RenderArea(QWidget *parent) :
    QLabel(parent)
{
    pointSize = 4;
    setStyleSheet("background-color:transparent;");
}

Vector3f RenderArea::getCentroid()
{
    //RenderArea::limits = CVlib::getBounds(pointList);
    RenderArea::centroid = CVlib::getCentroid(pointList);
    return RenderArea::centroid;
}

void RenderArea::normalize()
{
    //pointListNormalized = pointList;
    //for(int i = 0; i < pointListNormalized.count(); i ++ ){
        //std::cout << pointListNormalized.at(i) << std::endl;
        //pointListNormalized.push_back(pointList.at(i));
    //}
}

QVector<Vector3f> RenderArea::getNormalizedPoints()
{
    //Adjustments needed to get only good points
    return pointList;
}

void RenderArea::addPoint(float px, float py)
{
    Vector3f point;
    point << px, py, 1;
    pointList.push_back(point);
    RenderArea::getCentroid();

}

void RenderArea::mouseReleaseEvent(QMouseEvent *ev){
    if(ev->button()==1){
        if(pointList.count()>=8){
            normalize();
            emit renderAreaClicked(ev);
        }else{
            RenderArea::addPoint(ev->x(), ev->y());
        }
    }else if(ev->button()==2){
        if(pointList.count()>0){
            pointList.removeLast();
        }
    }
    update();
}

void RenderArea::paintEvent(QPaintEvent *ev){
    QLabel::paintEvent(ev);
    if(pointList.count()>0){
        RenderArea::getCentroid();
        QPainter painter(this);
        for(int i = 0; i < pointList.count(); i++){
            painter.setRenderHint(QPainter::Antialiasing, true);
            painter.setBrush(Qt::red);
            painter.setPen(Qt::NoPen);
            painter.drawEllipse(pointList.at(i)(0)-pointSize/2, pointList.at(i)(1)-pointSize/2, pointSize, pointSize);
        }
        painter.setBrush(Qt::blue);
        //std::cout << RenderArea::centroid.transpose() << std::endl;
        painter.drawEllipse(RenderArea::centroid(0),RenderArea::centroid(1), 10,10);
    }
    //painter.drawEllipse(50, 50, 10,10);
}

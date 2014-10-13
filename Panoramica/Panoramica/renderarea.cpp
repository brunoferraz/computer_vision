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

void RenderArea::closePointList()
{
    allPointLists.push_back(pointList);
    pointList.clear();
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
    QVector<Vector3f> allpoints;
    for(int i = 0; i < allPointLists.size(); i++)
    {
        for(int j=0; j < allPointLists.at(i).size(); j++){
            allpoints.push_back(allPointLists.at(i).at(j));
        }
    }
    return allpoints;
}

void RenderArea::addPoint(float px, float py)
{
    Vector3f point;
    point << px, py, 1;
    pointList.push_back(point);
    //RenderArea::getCentroid();
}

void RenderArea::findPoints()
{
    SiftFeatureDetector detector( CVlib::siftParameter );

    Mat img = CVlib::QImage2Mat(pixmap()->toImage());
    detector.detect( img, keypoints );

    SiftDescriptorExtractor extractor;
    extractor.compute( img, keypoints, descriptors);
}

void RenderArea::mouseReleaseEvent(QMouseEvent *ev){
    if(ev->button()==1){
        if(allPointLists.count()>=2){
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
    if(allPointLists.count()>0){
        //RenderArea::getCentroid();
        QPainter painter(this);
        for(int j = 0; j < allPointLists.size(); j++){
            for(int i = 0; i < allPointLists.at(j).size(); i++){
                Vector3f p = allPointLists.at(j).at(i);
                painter.setRenderHint(QPainter::Antialiasing, true);
                painter.setBrush(Qt::red);
                painter.setPen(Qt::NoPen);
                painter.drawEllipse(p(0)-pointSize/2, p(1)-pointSize/2, pointSize, pointSize);
            }
        }
        painter.setBrush(Qt::blue);
        //std::cout << RenderArea::centroid.transpose() << std::endl;
        //painter.drawEllipse(RenderArea::centroid(0),RenderArea::centroid(1), 10,10);
    }
    //painter.drawEllipse(50, 50, 10,10);
}

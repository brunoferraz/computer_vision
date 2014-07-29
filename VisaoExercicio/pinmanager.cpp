#include "pinmanager.h"
#include "pin.h"
#include <QDebug>
#include <stdio.h>
#include <iostream>

using namespace std;

PinManager::PinManager(QWidget *parent) :
    QWidget(parent)
{
    isGrabbing = true;
}

void PinManager::createPin(QPoint pos)
{
    Pin *pin= new Pin(this->parentWidget(), pos);
    pin->show();
    pinlist.push_back(pin);
//    qDebug() << this->type;
    switch (this->type) {
    case TYPE_TWO:

        break;
    case TYPE_THREE:
        if(pinlist.count()%2 == 0){
            createLine();
        }
        break;
    case TYPE_FOUR:
        if(pinlist.count()%2 == 0){
            createLine();
        }
        break;
    default:
        break;
    }
}

QVector<QPoint> PinManager::getSortedPolygonPoints()
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

Vector3f PinManager::getHorizonLine()
{
    Vector3f horizonLine(3,1);
    Vector3f vanishingPoint1(3,1);
    Vector3f vanishingPoint2(3,1);

    vanishingPoint1 = linelist.at(0)->projectiveLine.cross(linelist.at(1)->projectiveLine);
    vanishingPoint2 = linelist.at(2)->projectiveLine.cross(linelist.at(3)->projectiveLine);

    horizonLine = vanishingPoint1.cross(vanishingPoint2);

    return horizonLine;
}

void PinManager::hide_pins()
{
    for(int i = 0; i < pinlist.count(); i ++){
        pinlist[i]->hide();
    }
    if(type == TYPE_THREE){
       for(int i = 0; i< linelist.count(); i++){
            linelist[i]->hide();
       }
    }
}

void PinManager::setType(int t)
{
    type = t;
    switch (type) {
    case TYPE_ONE:
        MaxPins = 4;
        break;
    case TYPE_TWO:
        MaxPins = 4;
        clearPinManager();
        break;
    case TYPE_THREE:
        MaxPins = 8;
        break;
    default:
        break;
    }

}

void PinManager::createLine()
{
    //Get two last pins created to make a line
    Line *line = new Line(PinManager::parentWidget(),pinlist.at(pinlist.count()-1), pinlist.at(pinlist.count()-2));
    line->show();
    linelist.push_back(line);
}

void PinManager::clearPinManager()
{
    if(pinlist.count()!=0){
        for(int i = 0; i< pinlist.count(); i++){
            delete pinlist.at(i);
            //pinlist.pop_front();
        }
    }
    pinlist.clear();
}


#include "pinmanager.h"
#include <QDebug>

PinManager::PinManager(QObject *parent) :
    QObject(parent)
{
}
void PinManager::addPin(float _x, float _y)
{
    if(pinlist.count()<maxPoints){
        Vector3f vec;
        vec << _x, _y, 1;
        pinlist.push_back(vec);
    }else{
        emit onGetAllPins();
    }
}
void PinManager::removeLastPin()
{
    if(!pinlist.isEmpty()){
        pinlist.removeLast();
    }
}

void PinManager::removeAllPins()
{
    pinlist.clear();
}

void PinManager::printAllPins()
{
    qDebug() << pinlist.count();
     for(int i = 0; i < pinlist.count(); i ++){
         std::cout << pinlist.at(i).transpose() << std::endl;
     }
}
QVector <Vector3f> PinManager::getSortedPolygonPoints()
{
    //sort points based on X
    QVector<Vector3f> tempList;
    for(int i = 0; i < pinlist.count(); i ++){
        Vector3f vectemp;
        vectemp << pinlist.at(i)(0), pinlist.at(i)(1), pinlist.at(i)(2);
        tempList.push_back(vectemp);
    }
    bool flag = true;
    while (flag)
    {
        flag = false;
        for(int i=1; i< tempList.count(); i++)
        {
            if(tempList.at(i)(0) < tempList.at(i -1)(0))
            {
                flag = true;
                Vector3f vec =tempList.at(i-1);
                tempList.replace(i-1, tempList.at(i));
                tempList.replace(i, vec);
            }
        }
    }
    //from this two first get the lowest point as point 0 and the higher as 1
    int indexBottom = 1;
    int indexTop = 0;
    if(tempList.at(0)(1)<tempList.at(1)(1)){
        indexBottom = 0;
        indexTop = 1;
    }
    pinlist.replace(indexBottom, tempList.first());
    tempList.pop_front();
    pinlist.replace(indexTop, tempList.first());
    tempList.pop_front();
    //from the last two get the lowest point as point 1 and the higher as 2
    indexBottom = 3;
    indexTop =2;
    if(tempList.at(0)(1)>tempList.at(1)(1)){
        indexBottom = 2;
        indexTop = 3;
    }
    pinlist.replace(indexBottom, tempList.first());
    tempList.pop_front();
    pinlist.replace(indexTop, tempList.first());
    tempList.pop_front();

   return pinlist;
}
Vector3f PinManager::getHorizonLine(){
    Vector3f horizonLine;
    Vector3f vanishingPoint1;
    Vector3f vanishingPoint2;
    Vector3f projectiveLine_0;
    Vector3f projectiveLine_1;

   // projectiveLine_0 = pinlist.at(0).cross(pinlist.at(1));
   // projectiveLine_0 = projectiveLine_0/projectiveLine_0(2);
   // projectiveLine_1 = pinlist.at(2).cross(pinlist.at(3));
   // projectiveLine_1 = projectiveLine_1/projectiveLine_1(2);

    //vanishingPoint1 = projectiveLine_0.cross(projectiveLine_1);
   // vanishingPoint1 = vanishingPoint1/vanishingPoint1(2);

    //projectiveLine_0 = pinlist.at(4).cross(pinlist.at(5));
    //projectiveLine_0 = projectiveLine_0/projectiveLine_0(2);
    //projectiveLine_1 = pinlist.at(6).cross(pinlist.at(7));
   // projectiveLine_1 = projectiveLine_1/projectiveLine_1(2);

    //vanishingPoint2 = projectiveLine_0.cross(projectiveLine_1);
    //vanishingPoint2 = vanishingPoint2/vanishingPoint2(2);

    //horizonLine = vanishingPoint1.cross(vanishingPoint2);
    //horizonLine = horizonLine/horizonLine(2);

    projectiveLine_0 = getLine(0);
    projectiveLine_1 = getLine(1);
    std::cout<< "projective_0  -" << projectiveLine_0.transpose() << std::endl;
    std::cout<< "projective_1  -" << projectiveLine_1.transpose() << std::endl;
    vanishingPoint1 = projectiveLine_0.cross(projectiveLine_1);
    vanishingPoint1/=vanishingPoint1(2);
    std::cout<< "vanishingPoint1  -" << vanishingPoint1.transpose() << std::endl;

    projectiveLine_0 = getLine(2);
    projectiveLine_1 = getLine(3);
    std::cout<< "projective_2  -" << projectiveLine_0.transpose() << std::endl;
    std::cout<< "projective_3  -" << projectiveLine_1.transpose() << std::endl;
    vanishingPoint2 = projectiveLine_0.cross(projectiveLine_1);
    vanishingPoint2/=vanishingPoint2(2);
    std::cout<< "vanishingPoint2  -" << vanishingPoint2.transpose() << std::endl;

    horizonLine = vanishingPoint1.cross(vanishingPoint2);
    horizonLine = horizonLine/horizonLine(2);

    std::cout<< "horizonLine  " << horizonLine.transpose() << std::endl;
    return horizonLine;
}
Vector3f PinManager::getLine(int i){
    Vector3f projectiveLine;
    int n = (i*2);
    projectiveLine = pinlist.at(n).cross(pinlist.at(n+1));
    projectiveLine/= projectiveLine(2);
    return projectiveLine;
}

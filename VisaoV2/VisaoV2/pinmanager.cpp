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
QVector <Vector3f> PinManager::getSortedPolygonPoints()
{
    //sort points based on X
    QVector<Vector3f> tempList;
    for(int i = 0; i < pinlist.count(); i ++){
        Vector3f vectemp;
        vectemp << pinlist.at(i)(0), pinlist.at(i)(1), pinlist.at(i)(2);
        tempList.push_back(vectemp);
    }
    qDebug() << "fsdffdsfdsfsd <<<"<<tempList.count();
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

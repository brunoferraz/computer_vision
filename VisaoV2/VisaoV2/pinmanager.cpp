#include "pinmanager.h"
#include <QDebug>

PinManager::PinManager()
{
}

void PinManager::addPin(float _x, float _y)
{
    Vector3f vec;
    vec << _x, _y, 1;
    pinlist.push_back(vec);
}

void PinManager::removeLastPin()
{
    if(!pinlist.isEmpty()){
        pinlist.removeLast();
    }
}

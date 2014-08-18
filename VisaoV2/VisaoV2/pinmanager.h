#ifndef PINMANAGER_H
#define PINMANAGER_H

#include <eigen3/Eigen/Dense>
#include <QVector>
#include <stdio.h>
#include <iostream>

using namespace Eigen;
using namespace std;

class PinManager
{
public:
    PinManager();
    QVector <Vector3f> pinlist;

    void addPin(float _x, float _y);
    void removePin(int index);
    void removeLastPin();

    void addLine();
    void removeLine();
    void removeLastLine();
};

#endif // PINMANAGER_H

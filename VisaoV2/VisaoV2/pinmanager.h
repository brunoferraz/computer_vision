#ifndef PINMANAGER_H
#define PINMANAGER_H

#include <QObject>
#include <eigen3/Eigen/Dense>
#include <QVector>
#include <stdio.h>
#include <iostream>


using namespace Eigen;
using namespace std;

class PinManager : public QObject
{
    Q_OBJECT
public:
    explicit PinManager(QObject *parent = 0);
    QVector <Vector3f> pinlist;
    void addPin(float _x, float _y);
    void removePin(int index);
    void removeLastPin();

    void removeAllPins();

    void addLine();
    void removeLine();
    void removeLastLine();

    QVector <Vector3f> getSortedPolygonPoints();
    int maxPoints;
signals:
    void onGetAllPins();
};

#endif // PINMANAGER_H

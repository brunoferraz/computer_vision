#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <iostream>
#include <QVector>
#include <eigen3/Eigen/Dense>
#include <QFile>
#include <QIODevice>
#include <QDebug>
#include <QDataStream>
#include <QTextStream>


using namespace Eigen;
using namespace std;

class Util
{
public:
    Util();
    static QVector<Vector3d> parsePoints(const QFile file);
    static QVector<VectorXd> parseLines(const QFile file);
    static QString openFile(const string& str);
};

#endif // UTIL_H

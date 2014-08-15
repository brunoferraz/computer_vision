#ifndef DEBUGSET_H
#define DEBUGSET_H

#include <eigen3/Eigen/Dense>
#include <QVector>
#include <QString>
#include <QVector>

using namespace Eigen;
using namespace std;

struct DebugParameters{
    QString file;
    MatrixXf points;
    MatrixXf lines;
    int maxPoints;
};

class DebugSet
{
public:
    DebugSet();
    QVector <DebugParameters*> debugSetPack;
    int teste;
    void prepareDebug();
};

#endif // DEBUGSET_H


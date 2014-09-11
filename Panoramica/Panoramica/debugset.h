#ifndef DEBUGSET_H
#define DEBUGSET_H

#include <eigen3/Eigen/Dense>
#include <QVector>

using namespace Eigen;

class DebugSet
{
public:
    DebugSet();
    static QVector<MatrixXf> list;

    static void prepareDebug();
    static QVector<Vector3f> getDebugSet(int indice);
};

#endif // DEBUGSET_H

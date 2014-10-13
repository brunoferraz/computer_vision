#ifndef PAIRS_H
#define PAIRS_H

#include <renderarea.h>

class Pairs
{
public:
    Pairs();
    QVector<RenderArea *> renderList;
    QVector<QVector<Vector3f>> pointList;
    Matrix3f H;
};

#endif // PAIRS_H

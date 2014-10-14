#ifndef PAIRS_H
#define PAIRS_H

#include <renderarea.h>

class Pairs
{
public:
    Pairs();
    QVector<QImage *> imageList;
    QVector<RenderArea *> renderList;
    QVector<QVector<Vector3f>> pointList;
    Matrix3f H;
    Matrix3f H_relative;
};

#endif // PAIRS_H

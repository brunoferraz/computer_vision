#ifndef RENDERAREA_H
#define RENDERAREA_H

#include <QLabel>
#include <QWidget>
#include <QMouseEvent>
#include <QPainter>
#include <QPaintEvent>
#include <eigen3/Eigen/Dense>
#include <stdio.h>
#include <iostream>
#include <QVector>

using namespace Eigen;
using namespace std;

#define RENDER_POINTS   0
#define RENDER_POLYGON  1
#define RENDER_LINES    2

class RenderArea : public QLabel
{
    Q_OBJECT
public:
    explicit RenderArea(QWidget *parent = 0);
    void drawPin(float _x, float _y);
    void drawLine(float x1, float y1, float x2, float y2);

    QVector <Vector3f> pinlist;
    int renderType;
signals:
    void imageClicked(QMouseEvent *ev);

public slots:

protected:
    void mouseReleaseEvent(QMouseEvent *ev);
    void paintEvent(QPaintEvent *ev);
};

#endif // RENDERAREA_H

#include "line.h"
#include <QDebug>
#include <QPainter>
#include <QPolygon>
#include <QtCore/QtCore>
#include <stdio.h>
#include <iostream>

using namespace std;
Line::Line(QWidget *parent, Pin *p1, Pin *p2) :
    QLabel(parent)
{
    //TODO pinlist must be ordered by p position
    if(p1->getDrawCenter().x() < p2->getDrawCenter().y()){
        pinlist.push_back(p1);
        pinlist.push_back(p2);
    }else{
        pinlist.push_back(p2);
        pinlist.push_back(p1);
    }

    setStyleSheet("background-color:transparent;");
    this->resize(QSize(800, 600));

    //Calculate line from projective geometry
    Vector3f pd1(3,1);
    Vector3f pd2(3,1);

    pd1 << pinlist.at(0)->x(), pinlist.at(0)->y(), 1;
    pd2 << pinlist.at(1)->x(), pinlist.at(1)->y(), 1;
    projectiveLine = pd1.cross(pd2);
}

void Line::draw()
{

}

void Line::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setWindow(0, 0, 800, 600);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QPolygon poly;
    for(int i = 0; i< pinlist.count(); i++){
        poly << pinlist.at(i)->getDrawCenter();
    }
    QPen linepen;
    linepen.setWidth(2);
    linepen.setColor(Qt::red);

    painter.setPen(linepen);
    painter.drawPolygon(poly);
}

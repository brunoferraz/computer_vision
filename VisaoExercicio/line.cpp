#include "line.h"
#include <QDebug>
#include <QPainter>
#include <QPolygon>
#include <QtCore/QtCore>
Line::Line(QWidget *parent, Pin *p1, Pin *p2) :
    QLabel(parent)
{
    pinlist.push_back(p1);
    pinlist.push_back(p2);
    setStyleSheet("background-color:transparent;");
    this->resize(QSize(800, 600));
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

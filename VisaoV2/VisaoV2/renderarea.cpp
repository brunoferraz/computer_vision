#include "renderarea.h"
#include <QLabel>
#include <QWidget>
#include <QObject>
#include <QDebug>
#include <QPoint>
#include <QPolygon>
RenderArea::RenderArea(QWidget *parent) :
    QLabel(parent)
{
    renderType = RENDER_POINTS;
}

void RenderArea::drawPin(float _x, float _y)
{
    int size = 10;
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setBrush(Qt::red);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(_x, _y, size, size);
}
void RenderArea::mouseReleaseEvent(QMouseEvent *ev)
{
    emit imageClicked(ev);
}

void RenderArea::paintEvent(QPaintEvent *ev)
{
    QLabel::paintEvent(ev);
    float size = 5;
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setBrush(Qt::red);
    QPen pen(Qt::red, 2);
    painter.setPen(pen);
    QPolygon poly;
    switch (renderType) {
    case RENDER_POINTS:
        for(int i=0; i < pinlist.count(); i++){
            painter.drawEllipse(pinlist.at(i)(0) -size/2, pinlist.at(i)(1)-size/2, size, size);
        }
        break;
    case RENDER_LINES:
        for(int i=0; i < pinlist.count(); i++){
            painter.drawEllipse(pinlist.at(i)(0) -size/2, pinlist.at(i)(1)-size/2, size, size);
            if(i%2==1)
            {
                painter.drawLine(QPoint(pinlist.at(i-1)(0),pinlist.at(i-1)(1)),QPoint(pinlist.at(i)(0),pinlist.at(i)(1)));
            }
        }
        break;
    case RENDER_POLYGON:
        for(int i=0; i < pinlist.count(); i++){
            painter.drawEllipse(pinlist.at(i)(0) -size/2, pinlist.at(i)(1)-size/2, size, size);
            if(i!=0)
            {
                painter.drawLine(QPoint(pinlist.at(i-1)(0),pinlist.at(i-1)(1)),QPoint(pinlist.at(i)(0),pinlist.at(i)(1)));
            }
        }
        break;
    default:
        break;
    }
}

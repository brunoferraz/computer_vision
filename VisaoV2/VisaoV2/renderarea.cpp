#include "renderarea.h"
#include <QLabel>
#include <QWidget>
#include <QObject>
#include <QDebug>

RenderArea::RenderArea(QWidget *parent) :
    QLabel(parent)
{

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
}

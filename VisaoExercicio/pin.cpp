#include "pin.h"
#include <QPainter>
#include <QPixmap>
#include <QBrush>
#include <QRectF>
#include <QDebug>

Pin::Pin(QWidget *parent, QPoint p) :
    QLabel(parent)
{
    size = 10.0;
    move(QPoint(p.x()-size/2, p.y()-size/2));
    setMaximumHeight(size);
    setStyleSheet("background-color:transparent;");
    //    qDebug() << p;
}

QPoint Pin::getDrawCenter()
{
    return QPoint((this->x() + size/2),(this->y()+ size/2));
}
void Pin::paintEvent(QPaintEvent *)
{
//TODO:fix update position with rescale (use Point to register position or move Pin while rescale)
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setBrush(Qt::red);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(0, 0, size, size);
}
//TODO: 'cause circle isnt centered need to override X, Y, Pos discounting radius (size/2)

#ifndef RENDERAREA_H
#define RENDERAREA_H

#include <QLabel>
#include <QWidget>
#include <QMouseEvent>
#include <QPainter>
#include <QPaintEvent>

class RenderArea : public QLabel
{
    Q_OBJECT
public:
    explicit RenderArea(QWidget *parent = 0);
    void drawPin(float _x, float _y);
    void drawLine(float x1, float y1, float x2, float y2);

signals:
    void imageClicked(QMouseEvent *ev);

public slots:

protected:
    void mouseReleaseEvent(QMouseEvent *ev);
    void paintEvent(QPaintEvent *ev);
};

#endif // RENDERAREA_H

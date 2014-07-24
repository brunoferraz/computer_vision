#ifndef PINMANAGER_H
#define PINMANAGER_H



#include <QWidget>
#include <QVector>
#include <pin.h>
#include <line.h>

class PinManager : public QWidget
{
    Q_OBJECT
    void clearPinManager();
public:
    explicit PinManager(QWidget *parent = 0);
    void createPin(QPoint p);
    void createLine(Pin p1, Pin p2);

    QVector<QPoint> getSortedPolygonPoints();
    Vector3f getHorizonLine();
    void hide_pins();
    void setType(int t);
    static const int TYPE_ONE = 0;
    static const int TYPE_TWO = 1;
    static const int TYPE_THREE = 2;
    int type;
    int MaxPins;
    bool isGrabbing;
    QVector<Pin*> pinlist;
    QVector<Line*> linelist;
    void createLine();
signals:

public slots:

};
#endif // PINMANAGER_H

#ifndef LINE_H
#define LINE_H

#include <QLabel>
#include <QVector>
#include <pin.h>
class Line : public QLabel
{
    Q_OBJECT
public:
    explicit Line(QWidget *parent = 0, Pin *p1 = 0, Pin *p2 =0);
    QVector<Pin*> pinlist;
    void draw();
protected:
    void paintEvent(QPaintEvent *);
signals:

public slots:

};

#endif // LINE_H
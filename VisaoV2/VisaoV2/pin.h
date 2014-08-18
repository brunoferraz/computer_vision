#ifndef PIN_H
#define PIN_H

#include <QLabel>
#include <QPoint>

class Pin : public QLabel
//TODO: Change Inheritance to QImage to use antialiasing
{
    Q_OBJECT
public:
    explicit Pin(QWidget *parent = 0, QPoint p = QPoint(0, 0));
    QPoint getDrawCenter();
signals:

protected:
    void paintEvent(QPaintEvent *);
//TODO:create Drag and Drop

public slots:
private:
     float size;

};

#endif // PIN_H

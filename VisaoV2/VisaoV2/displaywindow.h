#ifndef DISPLAYWINDOW_H
#define DISPLAYWINDOW_H

#include <QDialog>
#include <QWidget>
#include <QLabel>
#include <QImage>
#include <QWheelEvent>
#include <QMouseEvent>
#include <pinmanager.h>
#include <QPaintEvent>
#include <QPainter>
#include <debugset.h>
#include <renderarea.h>
namespace Ui {
class DisplayWindow;
}

class DisplayWindow : public QDialog
{
    Q_OBJECT

public:
    explicit DisplayWindow(QWidget *parent = 0);
    ~DisplayWindow();
    void showImage(QImage img);
    void zoom(float factor);
    void updateWindow();
    void debugSetup(DebugParameters *dp);
    RenderArea *renderArea;

    float scale;
    QImage currentImage;
    PinManager *pinmanager;

public slots:
    void windowClicked(QMouseEvent *ev);
    //void on_get_all_pins();

private:
    Ui::DisplayWindow *ui;

protected:
    void paintEvent(QPaintEvent *ev);
    void wheelEvent(QWheelEvent *ev);


};

#endif // DISPLAYWINDOW_H

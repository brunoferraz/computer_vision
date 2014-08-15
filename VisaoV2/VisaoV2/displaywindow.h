#ifndef DISPLAYWINDOW_H
#define DISPLAYWINDOW_H

#include <QDialog>
#include <QWidget>
#include <QLabel>
#include <QImage>
#include <QWheelEvent>

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

    float scale;
    QImage currentImage;

private:
    Ui::DisplayWindow *ui;

protected:
    void wheelEvent(QWheelEvent *ev);
};

#endif // DISPLAYWINDOW_H

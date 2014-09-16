#ifndef RESULTWINDOW_H
#define RESULTWINDOW_H

#include <QDialog>
#include <QLabel>
#include <QWidget>
#include <renderarea.h>
#include <QVector>
#include <QWidget>
#include <eigen3/Eigen/Dense>
#include <stdio.h>
#include <iostream>
#include <QDebug>

using namespace Eigen;
using namespace std;
namespace Ui {
class ResultWindow;
}

class ResultWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ResultWindow(QWidget *parent = 0);
    ~ResultWindow();
    RenderArea *renderArea;
    QVector<RenderArea *> list;
    QWidget *container;
    void addImage(QImage img, Matrix3f H, Vector3f centroid, QVector<Vector3f> lista);
    void assembleImage();
private:
    Ui::ResultWindow *ui;
};

#endif // RESULTWINDOW_H

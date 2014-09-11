#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QString>
#include <QVector>
#include <QImage>
#include <QDebug>
#include <QLabel>
#include <QPixmap>
#include <renderarea.h>
#include <eigen3/Eigen/Dense>
#include <QMouseEvent>
#include <stdio.h>
#include <iostream>
#include <eigen3/Eigen/Cholesky>
#include <QPointF>

using namespace Eigen;
using namespace std;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void openFiles(QString *path = 0);
    QVector<QImage> list;
    QVector<RenderArea *> list_renderArea;

    static const bool isDebug = true;
    int counter;
    void adjustImage();
private:
    Ui::MainWindow *ui;

public slots:
    void getPointManual(QMouseEvent *ev);
protected:

};

#endif // MAINWINDOW_H

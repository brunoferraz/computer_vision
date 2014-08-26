#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "imagemanager.h"
#include "displaywindow.h"
#include <debugset.h>
#include <QCloseEvent>
#include <eigen3/Eigen/Dense>
#include <stdio.h>
#include <iostream>
#include <QImage>
#include <QPoint>


#define WORK_0  0
#define WORK_0a 10
#define WORK_1  1
#define WORK_2  2
#define WORK_3  3

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
    int STATE;
    bool isDebug;
    DebugSet debugSet;
    ImageManager imageManager;
    DisplayWindow displayWindow;

    void LoadImage();
    void adjust_work_0(); //projective to similarity 4 points
    void adjust_work_0a(); //interested area
    void adjust_work_1(); //projective to affine 2 parallels
    void adjust_work_2(); //affine to similarity 2 corners
    void adjust_work_3(); //projective to similarity 5 corners

    MatrixXf H;
    MatrixXf Hi;

    QImage currentImage;

protected:
    void closeEvent(QCloseEvent *ev);

private slots:
    void on_pushButton_clicked();
    void on_bt_DebugMode_toggled(bool checked);
    void onGetAllPins();
    void set_state(int w);

private:
    Ui::MainWindow *ui;
    void setupProgram();

};

#endif // MAINWINDOW_H

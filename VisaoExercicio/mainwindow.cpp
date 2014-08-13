#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "imageviewer.h"
#include <QString>
#include "pin.h"
#include <QtGui>
#include <QDebug>
#include <imageviewer_ex2.h>

MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //ui->widget->move(50,50);

    //DEBUG IS STARTED BY PATH PARAMETER ON OPEN METHOD
//    image = new ImageViewer(ui->widget);
//    QString* str = new QString("/home/bruno/dev/computer_vision/build-VisaoExercicio-Desktop-Debug/assets/brahma01.jpg");
//    image->open(str);

    //WITHOUT DEBUG, JUST UNCOMMENT
//    image->open();
//    image->set_Scale(0.7);

//    image2 = new ImageViewer_ex2(ui->widget);
//    QString* str = new QString("/home/bruno/dev/computer_vision/build-VisaoExercicio-Desktop-Debug/assets/piso-perspectiva.jpg");
//    image2->open();

    image3 = new ImageViewer_ex3(ui->widget);
//    QString* str = new QString("/home/bruno/dev/computer_vision/build-VisaoExercicio-Desktop-Debug/assets/piso-perspectiva_original.jpg");
    QString* str = new QString("/home/bruno/dev/computer_vision/build-VisaoExercicio-Desktop-Debug/assets/tile_flip.jpg");
    image3->open();
   // image3->set_Scale(0.7);
}
MainWindow::~MainWindow()
{
    delete ui;
}

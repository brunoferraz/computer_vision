#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "imageviewer.h"
#include <QString>
#include "pin.h"
#include <QtGui>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //DEBUG IS STARTED BY PATH PARAMETER ON OPEN METHOD
    image = new ImageViewer(ui->widget);
    QString* str = new QString("/home/bruno/dev/computer_vision/build-VisaoExercicio-Desktop-Debug/assets/brahma01.jpg");

    image->open(str);

    //WITHOUT DEBUG, JUST UNCOMMENT
//    image->open();

//    image->set_Scale(0.7);
}
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_adjustButton_clicked()
{
    qDebug() << "vcvcxvcx";
    image->adjustImage(819,613);
}

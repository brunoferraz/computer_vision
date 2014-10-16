#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <util.h>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QString txt = Util::openFile("assets/house.000.corners");
    qDebug() << txt;
    //QVector<Vector3d> lista = Util::parsePoints(file);
}

MainWindow::~MainWindow()
{
    delete ui;
}

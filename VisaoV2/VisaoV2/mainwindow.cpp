#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <debugset.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    MainWindow::setupProgram();

    if(isDebug){
        MainWindow::LoadImage();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::LoadImage()
{
    QString *src;
    QImage img;
    if(isDebug){
        src = new QString(debugSet.debugSetPack.at(WORK_0)->file);
        img = imageManager.openImage(src);
    }else{
        img = imageManager.openImage();
    }
    displayWindow.showImage(img);
    displayWindow.show();
}

void MainWindow::closeEvent(QCloseEvent *ev)
{
    displayWindow.close();
    ev->accept();
}

void MainWindow::setupProgram()
{
    STATE = WORK_0;
    isDebug = true;
}

void MainWindow::on_pushButton_clicked()
{
    //TODO need to change STATE based on work selected
    LoadImage();
}

void MainWindow::on_bt_DebugMode_toggled(bool checked)
{
    isDebug = checked;
}

#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace cv;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    cv::Mat A;
}

MainWindow::~MainWindow()
{
    delete ui;
}

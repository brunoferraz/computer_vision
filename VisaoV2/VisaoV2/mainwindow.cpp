#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <debugset.h>
#include <pinmanager.h>
#include <cvlib.h>

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
        displayWindow.debugSetup(debugSet.debugSetPack.at(WORK_0));
    }else{
        img = imageManager.openImage();
    }
    displayWindow.pinmanager->maxPoints = debugSet.debugSetPack.at(WORK_0)->maxPoints;
    displayWindow.showImage(img);
    displayWindow.show();
    currentImage = img;
    if(isDebug){
        displayWindow.updateWindow();
    }
    PinManager *pin;
    pin = displayWindow.pinmanager;
    connect(pin,SIGNAL(onGetAllPins()),this,SLOT(onGetAllPins()));
}

void MainWindow::adjust_work_0()
{
    float h = 819;
    float w = 613;
    float sizeFactor = 1;
    float divisor = h;
    if(h>w){
        divisor = w;
    }
    //chage size to ratio;
    w = (w/divisor) * sizeFactor;
    h = (h/divisor) * sizeFactor;

    QVector<Vector3f> rlist; // Construct based on ratio
    rlist.push_back(Vector3f(0, 0, 1));
    rlist.push_back(Vector3f(0, h, 1));
    rlist.push_back(Vector3f(w, h, 1));
    rlist.push_back(Vector3f(w, 0, 1));
    H = CVlib::calculate_H(rlist, displayWindow.pinmanager->pinlist);
    Hi = H.inverse();

    //std::cout << H << std::endl;
    //QVector<QPoint> *areaRender;
    //*areaRender << QPoint(0,0) << QPoint(0, currentImage.height()) << QPoint(currentImage.width(), currentImage.height()) << QPoint(currentImage.width(), 0);
    QImage adjustedImage = CVlib::generateImage(currentImage, Hi);
    displayWindow.showImage(adjustedImage);
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

void MainWindow::onGetAllPins()
{
    switch (STATE) {
    case WORK_0:
        displayWindow.pinmanager->pinlist = displayWindow.pinmanager->getSortedPolygonPoints();
        adjust_work_0();
        break;
    default:
        break;
    }

}

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <debugset.h>
#include <pinmanager.h>
#include <cvlib.h>
#include <QDebug>

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
    //QVector<Vector3f> *areaRender = &displayWindow.pinmanager->pinlist;
    QImage adjustedImage = CVlib::generateImage(currentImage, Hi);
    displayWindow.showImage(adjustedImage);
    displayWindow.pinmanager->removeAllPins();
    set_state(WORK_1);
}

void MainWindow::adjust_work_1()
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
    for(int i=0; i < displayWindow.pinmanager->pinlist.count(); i++){
       displayWindow.pinmanager->pinlist.replace(i, displayWindow.pinmanager->pinlist.at(i)/ 1000);
        std::cout << displayWindow.pinmanager->pinlist.at(i).transpose() << std::endl;
    }
    bounds limits = CVlib::getHomographyBounds(displayWindow.pinmanager->pinlist,H);
    std::cout << limits.left << "   " << limits.top <<  "   "<<limits.right << "   " <<limits.bottom << std::endl;

    QVector<Vector3f> area; // Construct based on ratio
    area.push_back(Vector3f(limits.left, limits.top, 1));
    area.push_back(Vector3f(limits.left, limits.bottom, 1));
    area.push_back(Vector3f(limits.right, limits.bottom, 1));
    area.push_back(Vector3f(limits.right, limits.top, 1));
    displayWindow.pinmanager->pinlist = area;
    displayWindow.pinmanager->pinlist = displayWindow.pinmanager->getSortedPolygonPoints();

    QVector<Vector3f> *areaRender = &area;
    QImage adjustedImage = CVlib::generateImage(currentImage, H, &displayWindow.pinmanager->pinlist);
    displayWindow.showImage(adjustedImage);
    displayWindow.pinmanager->removeAllPins();
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
    case WORK_1:
        displayWindow.pinmanager->pinlist = displayWindow.pinmanager->getSortedPolygonPoints();
        adjust_work_1();
        break;
    default:
        break;
    }

}

void MainWindow::set_state(int w)
{
    STATE = w;
    switch (w) {
    case WORK_0:

        break;
    case WORK_1:

        break;
    default:
        break;
    }
}

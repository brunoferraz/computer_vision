#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <debugset.h>
#include <pinmanager.h>
#include <cvlib.h>
#include <QDebug>
#include <renderarea.h>
#include <QPixmap>

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
        src = new QString(debugSet.debugSetPack.at(STATE)->file);
        img = imageManager.openImage(src);
        displayWindow.debugSetup(debugSet.debugSetPack.at(STATE));
    }else{
        img = imageManager.openImage();
    }
    displayWindow.pinmanager->maxPoints = debugSet.debugSetPack.at(STATE)->maxPoints;
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
    set_state(WORK_0a);
}

void MainWindow::adjust_work_0a()
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
       displayWindow.pinmanager->pinlist.replace(i, displayWindow.pinmanager->pinlist.at(i));
    }
    bounds limits = CVlib::getHomographyBounds(displayWindow.pinmanager->pinlist,H);

    QVector<Vector3f> area; // Construct based on ratio
    area.push_back(Vector3f(limits.left,  limits.top, 1));
    area.push_back(Vector3f(limits.left,  limits.bottom, 1));
    area.push_back(Vector3f(limits.right, limits.bottom, 1));
    area.push_back(Vector3f(limits.right, limits.top, 1));
    //displayWindow.pinmanager->pinlist = CVlib::divideByW(area);
    displayWindow.pinmanager->pinlist = displayWindow.pinmanager->getSortedPolygonPoints();

    displayWindow.pinmanager->printAllPins();

    //QVector<Vector3f> *areaRender = &area;
    //QImage adjustedImage = CVlib::generateImage(currentImage, Hi, &displayWindow.pinmanager->pinlist);
    displayWindow.showImage(currentImage);
    //displayWindow.pinmanager->removeAllPins();
}

void MainWindow::adjust_work_1()
{
    //std::cout << displayWindow.pinmanager->getHorizonLine().transpose() << std::endl;
    Vector3f hl(3,1);
    hl = displayWindow.pinmanager->getHorizonLine().transpose();

    MatrixXf temp(3,3);
    temp << 1, 0, 0, 0, 1, 0, hl(0), hl(1), hl(2);

    H = temp;
    Hi = H.inverse();

    QImage adjustedImage = CVlib::generateImage(currentImage, H);
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
    //STATE = WORK_1;
    set_state(WORK_1);
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
    case WORK_0a:
        displayWindow.pinmanager->pinlist = displayWindow.pinmanager->getSortedPolygonPoints();
        adjust_work_0a();
        break;
    case WORK_1:
        //displayWindow.pinmanager->printAllPins();
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
        displayWindow.renderArea->renderType = RenderArea::RENDER_POINTS;
        break;
    case WORK_1:
        displayWindow.renderArea->renderType = RenderArea::RENDER_LINES;
        break;
    default:
        break;
    }
}

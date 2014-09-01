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
    originalImage = img;
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
    currentImage = adjustedImage;
    set_state(WORK_2);
}

void MainWindow::adjust_work_2()
{
    Vector3f l(3,1);
    Vector3f m(3,1);
    Vector3f r1(3,1);
    Vector3f r2(3,1);
    MatrixXf A(2,3);

    l = displayWindow.pinmanager->getLine(0);
    m = displayWindow.pinmanager->getLine(1);

    r2 << l(0) * m(0), l(0) * m(1) + l(1) * m(0), l(1) * m(1);

    l = displayWindow.pinmanager->getLine(2);
    m = displayWindow.pinmanager->getLine(3);

    r1 << l(0) * m(0), l(0) * m(1) + l(1) * m(0), l(1) * m(1);

    A << r1.transpose(), r2.transpose();
    JacobiSVD<MatrixXf> SVD(A, ComputeFullV);
    VectorXf S = SVD.matrixV().col(SVD.matrixV().cols() - 1);

    S(2) = 1;

    MatrixXf kkt(2,2);
    kkt << S(0), S(1), S(1), S(2);

    LLT<MatrixXf> lltOfA(kkt);
    MatrixXf L = lltOfA.matrixU();

    H << L(0), L(1), 0, L(2), L(3), 0, 0, 0, 1;
    H = H.inverse();

    QImage adjustedImage = CVlib::generateImage(currentImage, H);
    displayWindow.showImage(adjustedImage);
    displayWindow.pinmanager->removeAllPins();
}

void MainWindow::adjust_work_3()
{
    int pairs = displayWindow.pinmanager->pinlist.count()/4;
    std::cout << pairs << std::endl;
    //SVD 5
    MatrixXf A(pairs, 6);
    for(int i = 0; i < pairs; i++){
        int pos = i * 2;
        Vector3f l(3,1);
        Vector3f m(3,1);
        MatrixXf r(1,6);
        l = displayWindow.pinmanager->getLine(pos);
        m = displayWindow.pinmanager->getLine(pos + 1);
        l /=l(2);
        m /=m(2);

        r <<    l(0) * m(0),
                (l(0) * m(1) + l(1) * m(0))/2,
                l(1) * m(1),
                (l(0) * m(2) + l(2) * m(0))/ 2,
                (l(1)*m(2) + l(2)*m(1))/2,
                l(2)*m(2);
        A.row(i) << r;
    }
    JacobiSVD<MatrixXf> SVD(A, ComputeFullV);
    VectorXf x = SVD.matrixV().col(SVD.matrixV().cols() - 1);
    x/= x(2);

    //QR DECOMPOSITION
//    MatrixXf A(pairs, 5);
//    VectorXf b(5,1);
//    for(int i = 0; i < pairs; i++){
//        int pos = i * 2;
//        Vector3f l(3,1);
//        Vector3f m(3,1);
//        MatrixXf r(1,5);
//        l = displayWindow.pinmanager->getLine(pos);
//        m = displayWindow.pinmanager->getLine(pos + 1);

//        r <<    l(0) * m(0),
//                (l(0) * m(1) + l(1) * m(0))/2,
//                l(1) * m(1),
//                (l(0) * m(2) + l(2) * m(0))/ 2,
//                (l(1)*m(2) + l(2)*m(1))/2;
//        A.row(i) << r;
//        b.row(i) << -l(2)*m(2);
//    }
//    MatrixXf x(5,1);
//    x = A.colPivHouseholderQr().solve(b);
//    x/=x(2);

    //END OF DECOMPOSITION

    Matrix3f C;
    C << x(0), x(1)/2, x(3)/2,
         x(1)/2, x(2), x(4)/2,
         x(3)/2, x(4)/2, 1;

    Matrix2f kkt;
    kkt << C(0,0), C(0,1),
           C(1,0), C(1,1);

    MatrixXf vKKt(1,2);
    vKKt << C(2,0), C(2,1);

    MatrixXf V(1,2);
    V = vKKt * kkt.inverse();

    LLT<MatrixXf> llt(kkt);
    MatrixXf L = llt.matrixU();
    MatrixXf M (3,3);
    M << L(0,0), L(0,1),0, L(1,0), L(1,1),0, V(0), V(1), 1;

    H = M.inverse();
    Hi = M;

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
    set_state(WORK_3);
    isDebug = false;
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
    case WORK_2:
        //displayWindow.pinmanager->printAllPins();
        adjust_work_2();
        break;
    case WORK_3:
        displayWindow.pinmanager->printAllPins();
        adjust_work_3();
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
    case WORK_2:
        if(isDebug){
            //displayWindow.debugSetup(debugSet.debugSetPack.at(STATE));
        }
        break;
    case WORK_3:
        displayWindow.renderArea->renderType = RenderArea::RENDER_LINES;
        break;
    default:
        break;
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    displayWindow.showImage(originalImage);
    displayWindow.pinmanager->removeAllPins();
}

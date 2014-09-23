#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <debugset.h>
#include <QDialog>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    if(isDebug)
    {
        DebugSet::prepareDebug();
        counter = 0;
        QString *str = new QString(QDir::currentPath() +"/assets");
        openFiles(str);
    }else{
        openFiles();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::openFiles(QString *path)
{
    MainWindow *telaTemp = this;
    if(!path){
        QFileDialog dialog(this);
        dialog.setDirectory(QDir::currentPath() + "/assets");
        dialog.setFileMode(QFileDialog::ExistingFile);
        dialog.setNameFilter("Images (*.png *.xpm *.jpg)");
        QStringList fileNames;
        if (dialog.exec())
        fileNames = dialog.selectedFiles();
        //qDebug() << fileNames;
    }else{
        list.push_back(QImage(QDir::currentPath() + "/assets/yosemite1.jpg"));
        list.push_back(QImage(QDir::currentPath() + "/assets/yosemite2.jpg"));
        for(int i = 0 ; i < list.count(); i++){
           RenderArea *tempArea = new RenderArea(this);
           QImage tempImg = list.at(i);
           tempArea->resize(tempImg.width(), tempImg.height());
           tempArea->move((tempImg.width()+ 10) * i + 10, 20);
           tempArea->setPixmap(QPixmap::fromImage(tempImg));
           //Get debug Points from debug list
           //tempArea->pointList = DebugSet::getDebugSet(i);
           tempArea->findPoints();

           FlannBasedMatcher matcher;
           std::vector< DMatch > matches;
           matcher.match( descriptors_1, descriptors_2, matches );

           tempArea->update();
           list_renderArea.push_back(tempArea);
           connect(tempArea,SIGNAL(renderAreaClicked(QMouseEvent*)),telaTemp,SLOT(getPointManual(QMouseEvent*)));
        }
     }
}

void MainWindow::adjustImage()
{
    qDebug() << "ajustando Imagem";
    //Iterate each image and its next
    Matrix3f H;
    QImage result;
    int i = 0;
    H = CVlib::dlt(list_renderArea.at(i)->getNormalizedPoints(),list_renderArea.at(i + 1)->getNormalizedPoints());
    result = CVlib::generateImage(list.at(i+1),H);


    RenderArea *resultArea = resultWindow.renderArea;
    Matrix3f identity;
    identity <<
            1,  0,  0,
            0,  1,  0,
            0,  0,  1;
    Vector3f teste;
    teste<< 0, 0,  1;
    resultWindow.addImage(list.at(0), identity, teste, list_renderArea.at(0)->pointList);
    resultWindow.addImage(list.at(1), H.inverse(), teste, list_renderArea.at(1)->pointList);
    resultWindow.assembleImage();
    resultWindow.exec();
}

void MainWindow::getPointManual(QMouseEvent *ev)
{
    //CVlib::printQVector(list_renderArea.at(counter)->pointList);
    counter++;
    if(isDebug){
        ev->x();
        adjustImage();
    }
}

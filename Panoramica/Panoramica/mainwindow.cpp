#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <debugset.h>


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
        list.push_back(QImage(QDir::currentPath() + "/assets/pan_0_mini.png"));
        list.push_back(QImage(QDir::currentPath() + "/assets/pan_1_mini.png"));
        for(int i = 0 ; i < list.count(); i++){
           RenderArea *tempArea = new RenderArea(this);
           QImage tempImg = list.at(i);
           tempArea->resize(tempImg.width(), tempImg.height());
           tempArea->move((tempImg.width()+ 10) * i + 10, 20);
           tempArea->setPixmap(QPixmap::fromImage(tempImg));
           //Get debug Points from debug list
           tempArea->pointList = DebugSet::getDebugSet(i);
           //qDebug() << tempArea->pointList.count();
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
    for(int i = 0; i < list.count()-1; i++)
    {
        H = CVlib::dlt(list_renderArea.at(i)->getNormalizedPoints(),list_renderArea.at(i + 1)->getNormalizedPoints());
        result = CVlib::generateImage(list.at(i+1),H);
    }
    RenderArea *resultArea = new RenderArea(this);
    Vector3f centroid = CVlib::getCentroid(CVlib::pointlistHomography(list_renderArea.at(1)->pointList, H));
   // resultArea->pointList = list_renderArea.at(0)->pointList;
    resultArea->move(10 , list.at(0).height() + 40);
    resultArea->resize(result.width(), result.height());
    QPointF *offSet_1 = new QPointF(list_renderArea.at(0)->centroid(0),list_renderArea.at(0)->centroid(1));
    QPointF *offSet_2 = new QPointF(centroid(0),centroid(1));
    result = CVlib::mergeImages(list.at(0),result, offSet_1, offSet_2);
    resultArea->setPixmap(QPixmap::fromImage(result));
    resultArea->show();
}

void MainWindow::getPointManual(QMouseEvent *ev)
{
    //CVlib::printQVector(list_renderArea)
    counter++;
    if(isDebug){
        ev->x();
        adjustImage();
    }
}

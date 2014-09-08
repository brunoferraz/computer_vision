#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    if(isDebug)
    {
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
           list_renderArea.push_back(tempArea);
           list_renderArea.at(i)->setPixmap(QPixmap::fromImage(tempImg));
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
        //list_renderArea.at(i)->hide();
    }
    RenderArea *resultArea = new RenderArea(this);
    resultArea->move(10 , list.at(0).height()+40);
    resultArea->resize(result.width(), result.height());
    resultArea->show();
    resultArea->setPixmap(QPixmap::fromImage(result));
}

void MainWindow::getPointManual(QMouseEvent *ev)
{
    //CVlib::printQVector(list_renderArea)
    counter++;
    qDebug() << ev->pos().x() << ev->pos().y();
    if(counter == 2){
        adjustImage();
    }
}

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
        list.push_back(QImage(QDir::currentPath() + "/assets/pan_0_mini.png"));
        list.push_back(QImage(QDir::currentPath() + "/assets/pan_1_mini.png"));
        for(int i = 0 ; i < list.count(); i++){
           RenderArea *tempArea = new RenderArea(this);
           QImage tempImg = list.at(i);
           tempArea->resize(tempImg.width(), tempImg.height());
           tempArea->move((tempImg.width()+ 10) * i + 10, 20);
           tempArea->setPixmap(QPixmap::fromImage(tempImg));
           //Get debug Points from debug list
           //tempArea->pointList = DebugSet::getDebugSet(i);
           tempArea->findPoints();

           tempArea->update();
           list_renderArea.push_back(tempArea);
           connect(tempArea,SIGNAL(renderAreaClicked(QMouseEvent*)),telaTemp,SLOT(getPointManual(QMouseEvent*)));
    }
        for(int j =1; j< list_renderArea.count();j++){
            //LOOK POSSIBLE PAIRS
            FlannBasedMatcher matcher;
            std::vector< DMatch > matches;
            matcher.match( list_renderArea.at(j-1)->descriptors, list_renderArea.at(j)->descriptors, matches );

            double max_dist = 0;
            double min_dist = 100;

            //LOOK FOR GOOD PAIRS
            std::vector< DMatch > good_matches;
            for(int i = 0 ; i < matches.size(); i++){
               double dist = matches[j].distance;
               if( dist < min_dist ) min_dist = dist;
               if( dist > max_dist ) max_dist = dist;
            }
           for(int i = 0 ; i < matches.size() ; i++){
               if( matches[i].distance <= std::max(2* min_dist, 0.02) )
               {
                   good_matches.push_back( matches[i]);
               }
           }
            QVector<Vector3f> listaTemp_0;
            QVector<Vector3f> listaTemp_1;

            std::cout << matches.size() << std::endl;
            for(int i = 1 ; i < good_matches.size(); i++){
                float x = list_renderArea.at(j-1)->keypoints.at(good_matches[i].queryIdx).pt.x;
                float y = list_renderArea.at(j-1)->keypoints.at(good_matches[i].queryIdx).pt.y;
                Vector3f temp;
                temp << x, y, 1;
                listaTemp_0.push_back(temp);

                x = list_renderArea.at(j)->keypoints.at(good_matches[i].trainIdx).pt.x;
                y = list_renderArea.at(j)->keypoints.at(good_matches[i].trainIdx).pt.y;
                Vector3f temp2;
                temp2 << x, y, 1;
                listaTemp_1.push_back(temp2);
                //printf( "-- Good Match [%d] Keypoint 1: %d  -- Keypoint 2: %d  \n", i, good_matches[i].queryIdx, good_matches[i].trainIdx );
            }
            list_renderArea.at(j-1)->pointList = listaTemp_0;
            list_renderArea.at(j)->pointList = listaTemp_1;
            list_renderArea.at(j-1)->update();
            list_renderArea.at(j)->update();
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
    //H = CVlib::dlt(list_renderArea.at(i)->getNormalizedPoints(),list_renderArea.at(i + 1)->getNormalizedPoints());
    H = CVlib::ransac(list_renderArea.at(i)->getNormalizedPoints(),list_renderArea.at(i + 1)->getNormalizedPoints());
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
        //ev->x();
        adjustImage();
    }
}

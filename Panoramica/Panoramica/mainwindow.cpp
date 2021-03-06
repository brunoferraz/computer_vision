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
//            list.push_back(new QImage(QDir::currentPath() + "/assets/pan_0_mini.png"));
//            list.push_back(new QImage(QDir::currentPath() + "/assets/pan_1_mini.png"));
//            list.push_back(new QImage(QDir::currentPath() + "/assets/pan_2_mini.png"));

//        list.push_back(new QImage(QDir::currentPath() + "/assets/yosemite1.jpg"));
//        list.push_back(new QImage(QDir::currentPath() + "/assets/yosemite2.jpg"));
//        list.push_back(new QImage(QDir::currentPath() + "/assets/yosemite3.jpg"));
//        list.push_back(new QImage(QDir::currentPath() + "/assets/yosemite4.jpg"));
//        list.push_back(new QImage(QDir::currentPath() + "/assets/yosemite5.jpg"));
        //list.push_back(new QImage(QDir::currentPath() + "/assets/yosemite6.jpg"));

        for(int i = 0 ; i < list.count(); i++){
           RenderArea *tempArea = new RenderArea(this);
           QImage *tempImg = list.at(i);
           tempArea->resize(tempImg->width(), tempImg->height());
           tempArea->move((tempImg->width()+ 10) * i + 10, 20);
           tempArea->setPixmap(QPixmap::fromImage(*tempImg));
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

            //std::cout << matches.size() << std::endl;
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
            for(int  i = 0; i < listaTemp_0.size(); i++){
                list_renderArea.at(j-1)->addPoint(listaTemp_0.at(i)(0), listaTemp_0.at(i)(1));
            }
            for(int  i = 0; i < listaTemp_1.size(); i++){
                list_renderArea.at(j)->addPoint(listaTemp_1.at(i)(0), listaTemp_1.at(i)(1));
            }

            list_renderArea.at(j-1)->closePointList();
            list_renderArea.at(j)->closePointList();


            Pairs *pairTemp = new Pairs();
            pairTemp->pointList.push_back(listaTemp_0);
            pairTemp->pointList.push_back(listaTemp_1);
            pairTemp->renderList.push_back(list_renderArea.at(j-1));
            pairTemp->renderList.push_back(list_renderArea.at(j));
            pairTemp->imageList.push_back(list.at(j-1));
            pairTemp->imageList.push_back(list.at(j));


            list_Pairs.push_back(pairTemp);

            list_renderArea.at(j-1)->update();
            list_renderArea.at(j)->update();
       }
   }
    //this->adjustSize(800, 600);
}

void MainWindow::adjustImage()
{
    qDebug() << "ajustando Imagem";
    //Iterate each image and its next
    Matrix3f H;
    QImage result;
    int i = 0;
    //H = CVlib::dlt(list_renderArea.at(i)->getNormalizedPoints(),list_renderArea.at(i + 1)->getNormalizedPoints());
    Matrix3f identity;
    Vector3f translation;
    translation << 0, 0, 0;
    identity <<
            1,  0,  0,
            0,  1,  0,
            0,  0,  1;
    H = identity;
    for(int i = 0; i < list_Pairs.size(); i ++)
    {
        H = CVlib::ransac(list_Pairs.at(i)->pointList.at(0), list_Pairs.at(i)->pointList.at(1));
        list_Pairs.at(i)->H = H;
    }
    H = identity;
    int limitPair = 0;
    int imageBase = CVlib::imageCenter;
    int pair = std::max(imageBase -1, 0);
    int limit = 0;
    if(CVlib::imageCenter == 0) limit =1;
    for(int i = pair; i >=limit; i--){
        H *= list_Pairs.at(i)->H;
        list_Pairs.at(i)->H_absolut = H;
    }
    resultWindow.addImage(*list_Pairs.at(0)->imageList.at(0), H, translation, list_Pairs.at(0)->pointList.at(0));
    int img = 1;
    for(int i = 0; i < list_Pairs.count(); i++){
        H *=  list_Pairs.at(i)->H.inverse();
        resultWindow.addImage(*list_Pairs.at(i)->imageList.at(img), H, translation, list_Pairs.at(i)->pointList.at(img));
        std::cout << H  <<std::endl;
        std::cout << " "  <<std::endl;
    }



//    pair = imageBase;
//    for(int i = pair; i < list_Pairs.count(); i++){
//        H *= list_Pairs.at(i)->H.inverse();
//        list_Pairs.at(i)->H_absolut = H;
//    }
//    int counter = 1;
//    //H = identity;
//    for(int i = 0; i < list_Pairs.count(); i++){
//        //H *=  list_Pairs.at(i)->H.inverse();
//        counter++;
//        int img = 1;
//        if(counter == imageBase){
//           // H = identity;
//        }else{
//            H *= list_Pairs.at(i)->H_absolut;
//        }
//        resultWindow.addImage(*list_Pairs.at(i)->imageList.at(img), H, translation, list_Pairs.at(i)->pointList.at(img));
//    }



//    resultWindow.addImage(*list_Pairs.at(limitPair)->imageList.at(0), H, translation, list_Pairs.at(limitPair)->pointList.at(0));
//    H *=  list_Pairs.at(0)->H.inverse();
//    resultWindow.addImage(*list_Pairs.at(0)->imageList.at(1), H, translation, list_Pairs.at(0)->pointList.at(1));
//    H *=  list_Pairs.at(1)->H.inverse();
//    resultWindow.addImage(*list_Pairs.at(1)->imageList.at(1), H, translation, list_Pairs.at(1)->pointList.at(1));







    resultWindow.assembleImage();
    resultWindow.exec();
}

void MainWindow::getPointManual(QMouseEvent *ev)
{
    //CVlib::printQVector(list_renderArea.at(counter)->pointList);
    counter++;
    //qDebug() << counter;
    if(isDebug){
        //ev->x();
        adjustImage();
    }
}

void MainWindow::paintEvent(QPaintEvent *ev)
{
    //QPainter p;
    //p.scale(0.1,0.1);
}

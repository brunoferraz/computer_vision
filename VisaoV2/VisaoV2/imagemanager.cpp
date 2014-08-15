#include "imagemanager.h"

ImageManager::ImageManager(QWidget *parent) :
    QWidget(parent)
{

}

QImage ImageManager::openImage(QString *path)
{
    QString fileName;
    if(path){
        fileName = *path;
    }else{
        fileName = QFileDialog::getOpenFileName(this,
                                             tr("Open File"), QDir::currentPath());
    }
    QImage *imageBase = new QImage(fileName);
    //setPixmap(QPixmap::fromImage(imageBase));
    qDebug() << fileName;
    return *imageBase;
}

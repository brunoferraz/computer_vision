#include "resultwindow.h"
#include "ui_resultwindow.h"

ResultWindow::ResultWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ResultWindow)
{
    ui->setupUi(this);
    renderArea = ui->label;
    container = ui->widget;
    container->resize(1000, 500);
    this->resize(1000,500);
}

ResultWindow::~ResultWindow()
{
    delete ui;
}

void ResultWindow::addImage(QImage img, Matrix3f H, Vector3f centroid, QVector<Vector3f> lista)
{
//    QImage newImage = CVlib::generateImage(img,H);
//    RenderArea *render = new RenderArea(container);
//    render->setPixmap(QPixmap::fromImage(newImage));
//    render->centroid = H * centroid;
//    render->centroid /= render->centroid(2);
//    render->limits = CVlib::getHomographyBounds(lista, H);
//    render->show();
//    list.push_back(render);
    RenderArea *render = new RenderArea();
    render->setPixmap(QPixmap::fromImage(img));
    render->H = H;
    list.push_back(render);
}

void ResultWindow::assembleImage()
{
    QImage newImage = QImage(2000, 2000, QImage::Format_ARGB32);
    QImage img = QImage(list.at(0)->pixmap()->width(), list.at(0)->pixmap()->height(), QImage::Format_ARGB32);
    qDebug() << newImage.height();
    for(int j=1; j < j <img.height(); j++){
        for(int i=1; i< img.width(); i++ ){
            Vector3f pos;
            pos << i, j, 1;
            pos = list.at(0)->H * pos;
            pos /= pos(2);
            std::cout << pos << std::endl;
            newImage.setPixel(pos(0), pos(1), img.pixel(i,j));
        }
    }
    renderArea->setPixmap(QPixmap::fromImage(newImage));
}

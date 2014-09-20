#include "resultwindow.h"
#include "ui_resultwindow.h"

ResultWindow::ResultWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ResultWindow)
{
    ui->setupUi(this);
    renderArea = ui->label;
    //container = ui->widget;
    //container->resize(1000, 500);
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

    Vector3f TL;
    TL << 0, 0, 1;
    Vector3f BL;
    BL << 0, img.height(),1;
    Vector3f BR;
    BR << img.width(), img.height(), 1;
    Vector3f TR;
    TR << img.width(), 0, 1;
    QVector<Vector3f> L;
    L.push_back(TL);
    L.push_back(BL);
    L.push_back(BR);
    L.push_back(TR);

    render->limits = CVlib::getHomographyBounds(L, H);
    list.push_back(render);
}

void ResultWindow::assembleImage()
{

    QImage newImage = QImage(list.at(1)->limits.right,  std::max(list.at(1)->limits.bottom, list.at(0)->limits.bottom), QImage::Format_ARGB32);
    renderArea->resize(list.at(1)->limits.right, std::max(list.at(1)->limits.bottom, list.at(0)->limits.bottom));
//    QImage img;
//    for(int k = 0; k <list.count();k++){
//        img = list.at(k)->pixmap()->toImage();
//        for(int j=0; j < img.height(); j++){
//            for(int i=0; i < img.width(); i++){
//                Vector3f pos;
//                pos << i, j, 1;
//                pos = list.at(k)->H * pos;
//                pos /= pos(2);
//                QColor color = img.pixel(i,j);
//                if(pos(0)>0 && pos(0)<newImage.width() && pos(1) > 0 && pos(1)< newImage.height())
//                {
//                    newImage.setPixel(pos(0), pos(1), color.rgba());
//                }
//            }
//        }
//    }
//    renderArea->setPixmap(QPixmap::fromImage(newImage));
    QVector<QImage> imageList;
    for(int k = 0; k < list.count(); k++)
    {
        QImage img = QImage(newImage.width(), newImage.height(),QImage::Format_ARGB32);
        for(int j=0; j < newImage.height(); j++)
        {
            for(int i=0; i< newImage.width(); i++)
            {
                Vector3f pos;
                pos << i, j, 1;
                pos = list.at(k)->H.inverse() * pos;
                pos/=pos(2);
                QColor color(0, 0, 0, 0);
                if(pos(0)>0 && pos(0)<list.at(k)->pixmap()->width() && pos(1)>0 && pos(1)<list.at(k)->pixmap()->height()){
                    color = list.at(k)->pixmap()->toImage().pixel(pos(0),pos(1));
                    //color = CVlib::interpolate(list.at(k)->pixmap()->toImage(), pos);
                    newImage.setPixel(i,j,color.rgba());
                }
                img.setPixel(i, j,color.rgba());
            }
        }
        imageList.push_back(img);
    }
    QPixmap pix = QPixmap::fromImage(newImage);
    pix.scaledToWidth(100);
    QImage finalImage = pix.toImage();
    renderArea->setPixmap(QPixmap::fromImage(finalImage));
}

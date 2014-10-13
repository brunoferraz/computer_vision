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
    bounds realLimit;
    realLimit.left = 0;
    realLimit.right = 0;
    realLimit.bottom = 0;
    realLimit.top = 0 ;
    for(int i = 0; i < list.size(); i++){
        realLimit.left = std::min(realLimit.left, list.at(i)->limits.left);
        realLimit.top = std::min(realLimit.top, list.at(i)->limits.top);
        realLimit.right = std::max(realLimit.right, list.at(i)->limits.right);
        realLimit.bottom = std::max(realLimit.bottom, list.at(i)->limits.bottom);
    }
    float ty = 0;
    if(realLimit.top < 0 ){
        ty = std::abs(realLimit.top);
    }
    QImage newImage = QImage(realLimit.right,  realLimit.bottom + ty, QImage::Format_ARGB32);
    renderArea->resize(realLimit.right, realLimit.bottom + ty);

    QVector<QVector<QVector<Vector4f>>> imageList;
    for(int k = 0; k < list.count(); k++)
    {
        //QImage img = QImage(newImage.width(), newImage.height(),QImage::Format_ARGB32);
         QVector<QVector<Vector4f>> img;
        for(int j=0; j < newImage.height(); j++)
        {
            QVector<Vector4f> line;
            for(int i=0; i< newImage.width(); i++)
            {
                Vector3f pos;
                pos << i, j-ty, 1;
                pos = list.at(k)->H.inverse() * pos;
                pos/=pos(2);
                QColor color(0, 0, 0, 0);
                int alpha = 0 ;
                if(pos(0)>0 && pos(0)<list.at(k)->pixmap()->width() && pos(1)>0 && pos(1)<list.at(k)->pixmap()->height()){
                    color = list.at(k)->pixmap()->toImage().pixel(pos(0),pos(1));
                    alpha = 255;
                    //color = CVlib::interpolate(list.at(k)->pixmap()->toImage(), pos);
                    //img.setPixel(i, j,color.rgba());

                }
                color.setAlpha(alpha);
                line.push_back(CVlib::colorToVector(color));
                //img.setPixel(i, j,color.rgba());
            }
            img.push_back(line);
        }
        imageList.push_back(img);
    }
    QVector<QVector<Vector4f>> img;
    for(int j=0; j< newImage.height(); j++)
    {
        QVector<Vector4f> line;
        for(int i=0; i< newImage.width(); i++)
        {
            Vector4f color;
            color << 0, 0, 0, 0;
            line.push_back(color);
        }
        img.push_back(line);
    }
    for(int k =0; k < imageList.count(); k++)
    {
        for(int j=0; j< newImage.height(); j++)
        {
            QVector<Vector4f> line;
            for(int i=0; i< newImage.width(); i++)
            {
                Vector4f color;
                if(img.at(j).at(i)(3) ==0){
                    color =  imageList.at(k).at(j).at(i);
                }else{
                    color = img.at(j).at(i);
                }
                line.push_back(color);
            }
            img.replace(j, line);
        }
    }
    for(int j=0; j< newImage.height(); j++)
    {
        for(int i=0; i< newImage.width(); i++)
        {
            QColor color =CVlib::vectorToColor(img.at(j).at(i));
            newImage.setPixel(i, j, color.rgba());
        }
     }

//    for(int j=0; j < newImage.height(); j++)
//    {
//        for(int i=0; i< newImage.width(); i++)
//        {
//            Vector4f colorVec;
//            colorVec << 0, 0, 0, 0;
//            int ratio = 1;
//            for(int k =0; k < imageList.count(); k++)
//            {
//                Vector4f color = imageList.at(k).at(j).at(i);
//                if(color(3)!=0){
//                    colorVec += color;
//                    //ratio++;
//                }
//            }
//            //std::cout << colorVec.transpose() << std::endl;
//            colorVec /= ratio;
//            colorVec(3) = 255;
//            newImage.setPixel(i,j, CVlib::vectorToColor(colorVec).rgba());
//        }
//    }
    QPixmap pix = QPixmap::fromImage(newImage);
    pix.scaledToWidth(100);
    QImage finalImage = pix.toImage();
    renderArea->setPixmap(QPixmap::fromImage(finalImage));
}

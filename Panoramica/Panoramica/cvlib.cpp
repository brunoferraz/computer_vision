#include "cvlib.h"
#include <eigen3/Eigen/Dense>

#include <stdio.h>
#include <iostream>
#include <QDebug>
#include <QSize>
#include <QColor>
#include <QRgb>
#include <math.h>
#include <qmath.h>
#include <QPointF>
using namespace Eigen;
using namespace std;
using namespace cv;
CVlib::CVlib()
{

}

MatrixXf CVlib::calculate_H(QVector <Vector3f> bp, QVector <Vector3f> rp)
{
    //CREATE MATRIX
    MatrixXf m(8, 8);
    for(int i = 0; i< rp.count(); i++){
        MatrixXf lineA(1,8);
        lineA << bp[i](0), bp[i](1), 1, 0, 0, 0, -(bp[i](0)*rp[i](0)), -(bp[i](1)*rp[i](0));
        MatrixXf lineB(1,8);
        lineB << 0, 0, 0, bp[i](0), bp[i](1), 1, -(bp[i](0)*rp[i](1)), -(bp[i](1)*rp[i](1));
        m.row(i*2) << lineA;
        m.row(i*2 + 1) << lineB;
    }
    MatrixXf A  = m.inverse();
    MatrixXf B(8,1);
    for(int i = 0; i< rp.count(); i++){
        B.row(i*2) << rp.at(i)(0);
        B.row(i*2 +1) << rp.at(i)(1);
    }
    MatrixXf v = A*B;
    Matrix3f h;

    h << v(0), v(1), v(2), v(3), v(4), v(5), v(6), v(7), 1;

    return h;
}


MatrixXf CVlib::homography(MatrixXf v, Matrix3f h)
{
    MatrixXf r(3,1);
    r = h * v;
    r = r/r(2);
    return r;
}

QImage CVlib::generateImage(QImage imageBase, Matrix3f h, QVector<Vector3f> *renderArea)
{
    QVector<Vector3f> areaTemp;
    if(renderArea==0){
        areaTemp.push_back(Vector3f(0,0,1));
        areaTemp.push_back(Vector3f(0,imageBase.height(),1));
        areaTemp.push_back(Vector3f(imageBase.width(),imageBase.height(),1));
        areaTemp.push_back(Vector3f(imageBase.width(),0,1));
    }else{
        areaTemp = *renderArea;
    }
    bounds limits = CVlib::getHomographyBounds(areaTemp, h);

   QSize size;
   float ratio;
   float tamanhoBase =500;
   if(limits.dx < limits.dy){
       qDebug() << "dx";
       ratio = limits.dx/ limits.dy;
       size = QSize(tamanhoBase* ratio, tamanhoBase);
   }else{
       qDebug() << "dy";
      ratio = limits.dy/ limits.dx;
      size = QSize(tamanhoBase , tamanhoBase*ratio);
   }
    QImage imageResult = QImage(size.width(), size.height(), QImage::Format_ARGB32);

    //TODO testar imagem horizontal e vertical
    //TODO implementar outros trabalhos
    //Todo entrada de paramentro baseado no tamanho de saida da imagem

    float factor = 1;
    float stepX = limits.dx / size.width() / factor;
    float stepY = limits.dy / size.height() /factor;

    Matrix3f hi = h.inverse().eval();
    for(int j = 0; j<imageResult.height(); j++){
        for(int i = 0; i<imageResult.width(); i++){
            MatrixXf p(3,1);
            p << (limits.left + i * stepX) , (limits.top + j * stepY), 1;
            MatrixXf r(3,1);
            r = CVlib::homography(p, hi);
            //std::cout << r.transpose() << std::endl;
            QColor color(0, 0, 0, 0);
            Vector4f finalColor;
            finalColor << 0, 0, 0, 0;
            if(r(0) > 0 && r(0) < imageBase.width() && r(1) > 0 && r(1) < imageBase.height()){
                color = CVlib::interpolate(imageBase, r);
                imageResult.setPixel(i, j, color.rgba());
            }
        }
     }
    return imageResult;
}

QColor CVlib::interpolate(QImage img, Vector3f point)
{
    QColor color(0, 0, 0, 0);
    double pixX;
    double pixY;
    float fragX = std::abs(std::modf(point(0), &pixX));
    float fragY = std::abs(std::modf(point(1), &pixY));
    if((fragX == 0.5 && fragY == 0.5)||
            (point(0)<1)||(point(0)>img.width()-2)||
            (point(1)<1)||(point(1)>img.height()-2))
    {

        color.setRgba(img.pixel(point(0), point(1)));
        color.setAlpha(0);
        return color;
    }
    color = CVlib::bilerp(img.pixel(pixX -1, pixY),
                          img.pixel(pixX +1, pixY),
                          img.pixel(pixX, pixY + 1),
                          img.pixel(pixX, pixY -1),
                          fragX,
                          fragY);
    return color;
}

bounds CVlib::getHomographyBounds(QVector<Vector3f> bp, Matrix3f H)
{
    MatrixXf p(3,1);
    p << bp.at(0)(0), bp.at(0)(1), 1;
    MatrixXf itopLeft(3,1);
    itopLeft = H * p;
    itopLeft = itopLeft/itopLeft(2);

    p << bp.at(3)(0), bp.at(3)(1),1;
    MatrixXf itopRight(3,1);
    itopRight = H * p;
    itopRight = itopRight/itopRight(2);

    p << bp.at(1)(0), bp.at(1)(1),1;
    MatrixXf ibottomLeft(3,1);
    ibottomLeft = H * p;
    ibottomLeft = ibottomLeft/ibottomLeft(2);

    p << bp.at(2)(0), bp.at(2)(1),1;
    MatrixXf ibottomRight(3,1);
    ibottomRight = H * p;
    ibottomRight = ibottomRight/ibottomRight(2);

    bounds limits;
    limits.top = std::min(itopLeft(1),itopRight(1));
    limits.left = std::min(itopLeft(0),ibottomLeft(0));
    limits.right = std::max(ibottomRight(0), itopRight(0));
    limits.bottom = std::max(ibottomRight(1), ibottomLeft(1));
    limits.dx = limits.right - limits.left;
    limits.dy = limits.bottom - limits.top;

    return limits;
}

bounds CVlib::getBounds(QVector<Vector3f> bp)
{
    //CVlib::printQVector(bp);
    MatrixXf p(3,1);
    p << bp.at(0)(0), bp.at(0)(1), 1;
    MatrixXf itopLeft(3,1);
    itopLeft = p;
    itopLeft = itopLeft/itopLeft(2);

    p << bp.at(3)(0), bp.at(3)(1),1;
    MatrixXf itopRight(3,1);
    itopRight = p;
    itopRight = itopRight/itopRight(2);

    p << bp.at(1)(0), bp.at(1)(1),1;
    MatrixXf ibottomLeft(3,1);
    ibottomLeft = p;
    ibottomLeft = ibottomLeft/ibottomLeft(2);

    p << bp.at(2)(0), bp.at(2)(1),1;
    MatrixXf ibottomRight(3,1);
    ibottomRight = p;
    ibottomRight = ibottomRight/ibottomRight(2);

    QVector<Vector3f> listTemp;
    for(int i = 0; i < bp.count(); i++)
    {
        Vector3f temp;
        temp << bp.at(i)(0), bp.at(i)(1), float(qSqrt(bp.at(i)(0)*bp.at(i)(0) + bp.at(i)(1)*bp.at(i)(1)));
        listTemp.push_back(temp);
    }
   // CVlib::printQVector(listTemp);
    bool flag = true;
    while (flag) {
        flag = false;
        for(int i = 0; i < listTemp.count() -1; i++)
        {
            if(listTemp.at(i)(2) < listTemp.at(i + 1)(2))
            {
                Vector3f temp = listTemp.at(i);
                listTemp.replace(i, listTemp.at(i+1));
                listTemp.replace(i+1, temp);
                flag = true;
            }
        }
    }

    bounds limits;
    limits.top = std::min(itopLeft(1),itopRight(1));
    limits.left = std::min(itopLeft(0),ibottomLeft(0));
    limits.right = std::max(ibottomRight(0), itopRight(0));
    limits.bottom = std::max(ibottomRight(1), ibottomLeft(1));
    limits.dx = limits.right - limits.left;
    limits.dy = limits.bottom - limits.top;
    limits.centroidX = limits.left + limits.dx/2;
    limits.centroidY = limits.top + limits.dy/2;
    limits.radius   = listTemp.at(0)(2);

    return limits;
}

QVector<Vector3f> CVlib::pointlistHomography(QVector<Vector3f> list, Matrix3f h)
{
    for(int i = 0 ; i < list.count(); i++){
        Vector3f vec;
        vec = h * list.at(i);
        vec /=vec(2);
        list.replace(i, vec);
    }
    return list;
}

QVector<Vector3f> CVlib::listDivideByW(QVector<Vector3f> list)
{
    for(int i = 0 ; i < list.count(); i++){
        list.replace(i, list.at(i)/list.at(i)(2));
    }
    return list;
}

Matrix3f CVlib::dlt(QVector<Vector3f> pointsA, QVector<Vector3f> pointsB)
{
    int numRows = pointsA.count() * 2;
    MatrixXf A(numRows, 9);
    for(int j = 0; j < pointsA.count();j++){
        float x = pointsA.at(j)(0);
        float y = pointsA.at(j)(1);
        float w = pointsA.at(j)(2);
        float x2 = pointsB.at(j)(0);
        float y2 = pointsB.at(j)(1);
        //float w2 = pointsB.at(j)(2);
        MatrixXf lineA(1,9);
        MatrixXf lineB(1,9);

        lineA<< 0,
                0,
                0,
                -x,
                -y,
                -w,
                y2 * x,
                y2 * y,
                y2 * w;

        lineB << x,
                 y,
                 w,
                0,
                0,
                0,
                -x2 * x,
                -x2 * y,
                -x2 * w;

        A.row(j * 2) << lineA;
        A.row((j * 2) + 1) << lineB;
    }
    //JacobiSVD<MatrixXf> SVD(A, ComputeFullU | ComputeFullV);
    JacobiSVD<MatrixXf> SVD(A, Eigen::ComputeThinV);
    VectorXf h = SVD.matrixV().col(SVD.matrixV().cols() - 1);
    Matrix3f H;
    H <<    h(0), h(1), h(2),
            h(3), h(4), h(5),
            h(6), h(7), h(8);
    return H;
}

QImage CVlib::mergeImages(QImage img1, QImage img2, QPointF *offSet_1, QPointF *offSet_2)
{
    if(offSet_2==0){
        offSet_2 =new QPointF(0,0);
    }
    if(offSet_1==0){
        offSet_1 =new QPointF(0,0);
    }
    float width     = offSet_1->x() - offSet_2->x() + img2.width();
    float height    = offSet_2->y() - offSet_2->y() + img2.height();
    QImage nova     = QImage(width, height, QImage::Format_ARGB32);
    QPoint p1;
    QPoint p2;

    for(int j = 0; j < nova.height(); j++)
    {
        for(int i = 0; i< nova.width(); i++)
        {
            p1 = QPoint(i, j);
            p2 = QPoint(offSet_1->x() - offSet_2->x() + i,
                        offSet_1->y() - offSet_2->y() + j);
            if(i>0 && i<img2.width() && j>0 && j < img2.height()){
                QColor color = img2.pixel(i,j);
                nova.setPixel(p2.x() ,p2.y(), color.rgba());
            }
            if(i>0 && i<img1.width() && j>0 && j < img1.height()){
                QColor color = img1.pixel(i,j);
                nova.setPixel(i ,j, color.rgba());
            }
        }
    }
    return nova;
}

Vector3f CVlib::getCentroid(QVector<Vector3f> list)
{
    float left = list.at(0)(0);
    float right = list.at(0)(0);
    float top = list.at(0)(1);
    float bottom = list.at(0)(1);
    for(int i =0; i < list.count(); i++){
        left    = std::min(left,    list.at(i)(0));
        right   = std::max(right,   list.at(i)(0));
        top     = std::min(top,     list.at(i)(1));
        bottom  = std::max(bottom,  list.at(i)(1));
    }
    float dx = right-left;
    float dy = bottom- top;
    Vector3f centroid;
    centroid <<  (dx/2) + left, (dy/2) + top, 1;
    return centroid;
}

void CVlib::printQVector(QVector<Vector3f> list)
{
    for(int i = 0; i < list.count(); i ++)
    {
        std::cout << list.at(i).transpose() << std::endl;
    }
}

QColor CVlib::vectorToColor(Vector4f color)
{
    QColor finalColor(color(0),
                      color(1),
                      color(2),
                      color(3));
    return finalColor;
}

Vector4f CVlib::colorToVector(QColor color)
{
    Vector4f finalColor;
    finalColor << color.red(), color.green(), color.blue(), color.alpha();
    return finalColor;
}

QColor CVlib::lerp(QColor v0, QColor v1, float t)
{
    Vector4f cv0 = CVlib::colorToVector(v0);
    Vector4f cv1 = CVlib::colorToVector(v1);
    Vector4f fColor = (1 - t)*cv0 + t * cv1;
    //Vector4f fColor = cv0 + t*(cv1 - cv0);
    QColor finalColor = CVlib::vectorToColor(fColor);
    return finalColor;
}

QColor CVlib::bilerp(QColor v0, QColor v1, QColor v2, QColor v3, float t0, float t1)
{
    QColor c1 = CVlib::lerp(v0, v1, t0);
    QColor c2 = CVlib::lerp(v2, v3, t0);
    QColor finalColor = CVlib::lerp(c1, c2, t1);
    return finalColor;
}

cv::Mat CVlib::QImageToCvMat(const QImage &inImage, bool inCloneImageData)
{
    switch ( inImage.format() )
    {
         // 8-bit, 4 channel
         case QImage::Format_RGB32:
         {
            cv::Mat  mat( inImage.height(), inImage.width(), CV_8UC4, const_cast<uchar*>(inImage.bits()), inImage.bytesPerLine() );
            return (inCloneImageData ? mat.clone() : mat);
         }
         // 8-bit, 3 channel
         case QImage::Format_RGB888:
         {
//            if ( !inCloneImageData )
//               qWarning() << "ASM::QImageToCvMat() - Conversion requires cloning since we use a temporary QImage";

//            QImage   swapped = inImage.rgbSwapped();
//            return cv::Mat( swapped.height(), swapped.width(), CV_8UC3, const_cast<uchar*>(swapped.bits()), swapped.bytesPerLine() ).clone();
         }
         // 8-bit, 1 channel
         case QImage::Format_Indexed8:
         {
//            cv::Mat  mat( inImage.height(), inImage.width(), CV_8UC1, const_cast<uchar*>(inImage.bits()), inImage.bytesPerLine() );
//            return (inCloneImageData ? mat.clone() : mat);
         }
         default:
            qWarning() << "ASM::QImageToCvMat() - QImage format not handled in switch:" << inImage.format();
            break;
    }
    return cv::Mat();
}

       // If inPixmap exists for the lifetime of the resulting cv::Mat, pass false to inCloneImageData to share inPixmap's data
       // with the cv::Mat directly
       //    NOTE: Format_RGB888 is an exception since we need to use a local QImage and thus must clone the data regardless
Mat CVlib::QPixmapToCvMat( const QPixmap &inPixmap, bool inCloneImageData)
{
   return CVlib::QImageToCvMat( inPixmap.toImage(), inCloneImageData );
}
QImage CVlib::MatToQImage(const cv::Mat &inMat)
{
//    switch ( inMat.type() )
//    {
//     // 8-bit, 4 channel
//     case CV_8UC4:
//     {
//        QImage image( inMat.data, inMat.cols, inMat.rows, inMat.step, QImage::Format_RGB32 );
//        return image;
//     }

//     // 8-bit, 3 channel
//     case CV_8UC3:
//     {
//        QImage image( inMat.data, inMat.cols, inMat.rows, inMat.step, QImage::Format_RGB888 );
//        return image.rgbSwapped();
//     }

//     // 8-bit, 1 channel
//     case CV_8UC1:
//     {
//        static QVector<QRgb>  sColorTable;
//        // only create our color table once
//        if ( sColorTable.isEmpty() )
//        {
//           for ( int i = 0; i < 256; ++i )
//              sColorTable.push_back( qRgb( i, i, i ) );
//        }

//        QImage image( inMat.data, inMat.cols, inMat.rows, inMat.step, QImage::Format_Indexed8 );

//        image.setColorTable( sColorTable );

//        return image;
//     }

//     default:
//        qWarning() << "ASM::cvMatToQImage() - cv::Mat image type not handled in switch:" << inMat.type();
//        break;
//    }
    return QImage();
}


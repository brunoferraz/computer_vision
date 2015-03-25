#include "imagecanvas.h"

#include <QPalette>
#include <QBrush>
#include <pin.h>
#include <Eigen/Householder>
#include <Eigen/QR>
#include <Eigen/Eigen>

ImageCanvas::ImageCanvas(QWidget *parent) :
    QWidget(parent)
{
}

void ImageCanvas::findPoints()
{
    //cast listPoints to cv::KeyPoint
    std::vector<cv::Point2f> points;
    for(int i = 0 ; i < listPoints.length(); i++){
        cv::Point2f p;
        p.x = listPoints.at(i)(0);
        p.y = listPoints.at(i)(1);
        points.push_back(p);
        cv::KeyPoint k = cv::KeyPoint(p, 0);
        listKeyPoints.push_back(k);
    }
    descriptors = cv::Mat(points);
//    std::cout << points << std::endl;
//    std::cout << descriptors << std::endl;


    //after automatize this finding points;
    cv::SiftFeatureDetector detector(DebugSet::siftParameter);
    cv::Mat imgMat = Util::QImage2Mat(img);
    detector.detect(imgMat, listKeyPoints);

    cv::SiftDescriptorExtractor extractor;
    extractor.compute(imgMat, listKeyPoints, descriptors);
}

void ImageCanvas::normalize()
{
    QList<Eigen::Vector3f> *L;
    if(DebugSet::isDebug){
        L = &listPointsDebug;
    }else{
        L = &listPoints;
    }
    //get max min
    float xmax, xmin, ymax, ymin;
    xmax = 0;
    xmin = 0;
    ymax = 0;
    ymin = 0;
    for(int i = 0; i < L->length(); i++){
        xmax = std::max(L->at(i)(0), xmax);
        ymax = std::max(L->at(i)(1), ymax);
        xmin = std::min(L->at(i)(0), xmin);
        ymin = std::min(L->at(i)(1), ymin);
    }
    Eigen::Vector3f c;
    c << -(xmax - xmin)/2, -(ymax - ymin)/2, 1;
    float distance = 0;
    for(int i = 0; i < L->length(); i++){
        Eigen::Vector3f d;
        d = L->at(i) - c;
        distance = std::max(distance, d.norm());
    }
    float r2 = std::sqrt(2);
    float factor = r2/distance;
    Eigen::Matrix3f s = Eigen::MatrixXf::Identity(3,3);
    Eigen::Matrix3f m = Eigen::MatrixXf::Identity(3,3);
    //calculate translate matrix
    m.col(2) << c;
    //calculate scaleMatrix
    m = m * factor;
    T = m;
    Tl = T.inverse();
    //Multiply points per normalization Matrix
    for(int i = 0; i < L->length(); i++){
        Eigen::Vector3f v;
        v = T * L->at(i);
        L->replace(i,v);
    }
}

void ImageCanvas::calculateK()
{
    Eigen::Matrix3f R, Q;
    Util::RQdecomposition(P, R, Q);
    K = R;
}

void ImageCanvas::start(DebugPack p)
{
    pack    = &p;
    img     = Util::openImage(pack->imgPath);

    QLabel *l = new QLabel(this);
    l->setPixmap(QPixmap::fromImage(img));
    l->resize(img.size());

    if(DebugSet::isDebug){
        QString points      = Util::openFile(pack->pointsPath.toStdString());
        QString lines       = Util::openFile(pack->linesPath.toStdString());
        QString camMatrix   = Util::openFile(pack->camMatrixPath.toStdString());

        listPoints = Util::parsePoints(points);
        listLines = Util::parseLines(lines);
        P = Util::parseCamMatrix(camMatrix);
        showPoints();
    }else{
        findPoints();
    }
    this->resize(img.size());
}

void ImageCanvas::showPoints()
{
//    for(int i = 0; i < listPoints.length(); i++){
//        Pin *pin = new Pin(this);
//        pin->size   = 4;
//        pin->move(listPoints.at(i)(0), listPoints.at(i)(1));
//        pin->show();
//    }

//    for(int i = 0; i < listKeyPoints.size(); i++){
//        Pin *pin = new Pin(this);
//        cv::KeyPoint *k = &listKeyPoints.at(i);
//        pin->color  = Qt::blue;
//        pin->size   = 4;
//        pin->move(k->pt.x, k->pt.y);
//        pin->show();
//    }
    if(DebugSet::isDebug){
        for(int i = 0; i < listPointsDebug.length(); i++){
            Pin *pin = new Pin(this);
            pin->size   = 4;
            pin->move(listPointsDebug.at(i)(0), listPointsDebug.at(i)(1));
            pin->show();
        }
    }
}

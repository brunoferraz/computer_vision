#ifndef IMAGECANVAS_H
#define IMAGECANVAS_H

#include <QWidget>
#include <QString>
#include <debugset.h>
#include <Eigen/Eigen>
#include <QLabel>
#include <util.h>
#include <QPainter>
#include <QPixmap>
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/nonfree/features2d.hpp"
class ImageCanvas : public QWidget
{
    Q_OBJECT

public:
    explicit ImageCanvas(QWidget *parent = 0);
    QList<Eigen::Vector3f> listPoints;
    QList<Eigen::Vector3f> listPointsDebug;
    QList<Eigen::MatrixXf> listLines;

    std::vector<cv::KeyPoint> listKeyPoints;
    cv::Mat descriptors;
    void findPoints(); //Analize image to get Points

    QList<Eigen::Vector3f *> matchPoints;

    Eigen::MatrixXf P;
    Eigen::Matrix3f T;
    Eigen::Matrix3f Tl;
    Eigen::Matrix3f K;
    QList<Eigen::Matrix3f> listP;
    void normalize();

    void calculateK();

    QImage img;
    void start(DebugPack p);
    void showPoints();

    QLabel *markers;
private:
    DebugPack *pack;
signals:

public slots:

};

#endif // IMAGECANVAS_H

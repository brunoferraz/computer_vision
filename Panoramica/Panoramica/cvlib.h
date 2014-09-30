#ifndef CVLIB_H
#define CVLIB_H

#include <eigen3/Eigen/Dense>
#include <QVector>
#include <QPoint>
#include <QImage>
#include <QRect>
#include <QSize>
#include <QMatrix>
#include <QColor>
#include <QPixmap>
#include <stdio.h>
#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/nonfree/features2d.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/imgproc.hpp>


struct bounds{
    float top;
    float left;
    float right;
    float bottom;
    float dx;
    float dy;
    float centroidX;
    float centroidY;
    float radius;
};

using namespace Eigen;
using namespace std;
using namespace cv;

class CVlib
{
public:
    CVlib();
//    static bounds getImageResultBounds();
    static MatrixXf calculate_H(QVector <Vector3f> bp, QVector <Vector3f> rp);
    static MatrixXf homography(MatrixXf v,  Matrix3f h); //Receiver vector and transform Matrix
    static QImage generateImage(QImage imageBase, Matrix3f h, QVector<Vector3f> *renderArea = 0);
    static QColor interpolate(QImage img, Vector3f point);
    static bounds getHomographyBounds(QVector<Vector3f> bp, Matrix3f H);
    static bounds getBounds(QVector<Vector3f> bp);
    static QVector<Vector3f> listDivideByW(QVector<Vector3f> list);
    static QVector<Vector3f> pointlistHomography(QVector<Vector3f> list, Matrix3f h);
    static Matrix3f dlt(QVector<Vector3f> pointsA, QVector<Vector3f> pointsB);
    static QMatrix convertToQMatrix(Matrix3f m);
    static QImage mergeImages(QImage img1, QImage img2, QPointF *offSet_1 = 0, QPointF *offSet_2 = 0);
    static Vector3f getCentroid(QVector<Vector3f> list);

    static void printQVector(QVector<Vector3f> list);
    static void printArray(int l[]);

    static QColor vectorToColor(Vector4f color);
    static Vector4f colorToVector(QColor color);
    static QColor lerp(QColor v0, QColor v1, float t);
    static QColor bilerp(QColor v0, QColor v1, QColor v2, QColor v3, float t0, float t1);

    static QImage Mat2QImage(const cv::Mat &inMat );
    static cv::Mat QImage2Mat(const QImage &inImage, bool inCloneImageData = true );
    static QPixmap Mat2QPixmap( const cv::Mat &inMat );

    static Matrix3f ransac(QVector<Vector3f> pA, QVector<Vector3f> pB);
    static float getDistance(Vector3f va, Vector3f vb);


    static const int siftParameter = 100;
    static float ransacThreshold;
};

#endif // CVLIB_H

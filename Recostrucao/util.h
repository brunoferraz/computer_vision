#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <QVector>
#include <Eigen/Dense>
#include <QFile>
#include <QIODevice>
#include <QDebug>
#include <QDataStream>
#include <QTextStream>
#include <QStringList>
#include <QImage>
#include <QFileDialog>
#include <QString>
#include <QImage>
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/nonfree/features2d.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <QFile>
#include <QCoreApplication>
#include <QTextStream>

using namespace std;

class Util
{
public:
    Util();
    static QList<Eigen::Vector3f> parsePoints(const QString str);
    static QList<Eigen::MatrixXf> parseLines(const QString str);
    static QList<Eigen::Vector2i> parseMatchs(const QString str, int, int);
    static Eigen::MatrixXf parseCamMatrix(const QString str);
    static QString openFile(const string& str);
    static void saveFile(const QString path, const QString text);

    static void printQList(const QList<Eigen::Vector3f> v);
    static void printQList(const QList<Eigen::VectorXi> v);
    static void printQList(const QList<Eigen::Vector2i> v);
    static void printQList(const QList<Eigen::MatrixXf> v);
    static void printQList(const QList<int> v);

    static QImage openImage(QString path);

    static QImage Mat2QImage(const cv::Mat &inMat );
    static cv::Mat QImage2Mat(const QImage &inImage, bool inCloneImageData = true );
    static QPixmap Mat2QPixmap( const cv::Mat &inMat );

    static Eigen::VectorXf linearTriangulation(const Eigen::Vector3f v, const Eigen::MatrixXf P, const Eigen::Vector3f vl);

    static void SolveCS(float& c, float& s, float a, float b);
    static void RQdecomposition(Eigen::MatrixXf A, Eigen::Matrix3f& R, Eigen::Matrix3f& Q);

};

#endif // UTIL_H

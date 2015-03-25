#ifndef PAIR_H
#define PAIR_H

#include <imagecanvas.h>
#include <QList>
#include <debugset.h>
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/nonfree/features2d.hpp"

class Pair
{
public:
    Pair(ImageCanvas *i1, ImageCanvas *i2);
    QList<ImageCanvas *> list;

    std::vector< cv::DMatch > goodMatches;
    QList<Eigen::Vector2i> goodMatchesDebug;

    QList<Eigen::MatrixXf> listP;

    void autoMode();
    void debugMode();
};

#endif // PAIR_H

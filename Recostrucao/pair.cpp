#include "pair.h"
#include <QDebug>

Pair::Pair(ImageCanvas *i1, ImageCanvas *i2)
{
    list.push_back(i1);
    list.push_back(i2);

    if(DebugSet::isDebug){
        debugMode();
    }else{
        autoMode();
    }
}

void Pair::autoMode()
{
    cv::FlannBasedMatcher matcher;
    std::vector< cv::DMatch > matches;

    matcher.match(list.at(0)->descriptors, list.at(1)->descriptors, matches);
    double max_dist = 0;
    double min_dist = 100;

    //LOOK FOR GOOD PAIRS
    std::vector< cv::DMatch > good_matches;
    for(int i = 0 ; i < matches.size(); i++){
       double dist = matches[i].distance;
       if( dist < min_dist ) min_dist = dist;
       if( dist > max_dist ) max_dist = dist;
    }
    for(int i = 0 ; i < matches.size() ; i++){
        cv::DMatch *m = &matches[i];

        if( matches[i].distance <= std::max(2* min_dist, 0.02) )
        {
            good_matches.push_back( matches[i]);
        }
    }

    QList<Eigen::Vector3f> lista_0;
    QList<Eigen::Vector3f> lista_1;
    for(int i = 1 ; i < good_matches.size(); i++){
        cv::DMatch *m = &good_matches[i];
        cv::KeyPoint *k = &list.at(0)->listKeyPoints.at(m->queryIdx);
        float x = k->pt.x;
        float y = k->pt.y;
        Eigen::Vector3f v;
        v<< x, y, 1;
        lista_0.push_back(v);

        k = &list.at(1)->listKeyPoints.at(m->trainIdx);
        x = k->pt.x;
        y = k->pt.y;
        Eigen::Vector3f v2;
        v2 << x, y, 1;
        lista_1.push_back(v2);
    }

    list.at(0)->listPoints.clear();
    list.at(1)->listPoints.clear();

    list.at(0)->listPoints = lista_0;
    list.at(1)->listPoints = lista_1;

    list.at(0)->showPoints();
    list.at(1)->showPoints();
}

void Pair::debugMode()
{
   QString match = Util::openFile(DebugSet::matchFilePath.toStdString());
   goodMatchesDebug =  Util::parseMatchs(match, 0, 1);

   ImageCanvas *p1 = list.at(0);
   ImageCanvas *p2 = list.at(1);
   p1->listPointsDebug.clear();
   for(int i= 0; i< goodMatchesDebug.length(); i++){
        p1->listPointsDebug.push_back(p1->listPoints.at(goodMatchesDebug.at(i)(0)));
        p2->listPointsDebug.push_back(p2->listPoints.at(goodMatchesDebug.at(i)(1)));
   }
   p1->showPoints();
   p2->showPoints();
}

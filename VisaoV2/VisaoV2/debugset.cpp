#include "debugset.h"
#include <QDebug>
#include <QString>
#include <iostream>
#include <stdio.h>

DebugSet::DebugSet()
{
    prepareDebug();
}
using namespace std;
void DebugSet::prepareDebug()
{
    //INSERE INFORMACOES SOBRE DEBUG
    DebugParameters *trabalho_1 = new DebugParameters;
    trabalho_1->file = QString("/home/bruno/dev/computer_vision/VisaoV2/build-VisaoV2-Desktop-Debug/assets/brahma01.jpg");
    trabalho_1->maxPoints = 4;
    MatrixXf pointList(trabalho_1->maxPoints, 2);
    pointList <<
                 200,  52,
                 213, 306,
                 363, 232,
                 362,  76;
    trabalho_1->points = pointList;
    //std::cout << pointList << std::endl;
    debugSetPack.push_back(trabalho_1);

    DebugParameters *trabalho_2 = new DebugParameters;
    trabalho_2->file = QString("/home/bruno/dev/computer_vision/VisaoV2/build-VisaoV2-Desktop-Debug/assets/piso-perspectiva.jpg");
    trabalho_2->maxPoints = 8;
    MatrixXf pointList2(trabalho_2->maxPoints, 2);
    pointList2 <<
                  22,   251,
                 355,   46,
                  50,   355,
                 411,   85,
                 97,    24,
                 459,   228,
                 55,    69,
                 429,   324;
    trabalho_2->points = pointList2;
    //std::cout << pointList << std::endl;
    debugSetPack.push_back(trabalho_2);

    DebugParameters *trabalho_2a = new DebugParameters;
    trabalho_2a->file = QString("/home/bruno/dev/computer_vision/VisaoV2/build-VisaoV2-Desktop-Debug/assets/piso-perspectiva.jpg");
    trabalho_2a->maxPoints = 8;
    MatrixXf pointList2a(trabalho_2a->maxPoints, 2);
    pointList2a <<
                   93, 257,
                  434, 142,
                  261, 145,
                  436, 255,
                  373, 106,
                  209, 330,
                   41, 220,
                  543, 215;
    trabalho_2a->points = pointList2a;
    //std::cout << pointList << std::endl;
    debugSetPack.push_back(trabalho_2a);

    DebugParameters *trabalho_3 = new DebugParameters;
    trabalho_3->file = QString("/home/bruno/dev/computer_vision/VisaoV2/build-VisaoV2-Desktop-Debug/assets/piso-perspectiva.jpg");
    trabalho_3->maxPoints = 20;
    MatrixXf pointList3(trabalho_3->maxPoints, 2);
    pointList3 <<
                  134,  48,
                   48,  354,
                   43,  272,
                  458,  263,
                  356,  45,
                  453,  344,
                  424,  74,
                  69,   80,
                   89,  322,
                  462,  46,
                  26,   51,
                  423,  322,
                   18,  253,
                  335,  57,
                  106,  32,
                  484,  243,
                   35,  157,
                  461,  149,
                  246,  45,
                  253,  352;
    trabalho_3->points = pointList3;
    //std::cout << pointList << std::endl;
    debugSetPack.push_back(trabalho_3);
}

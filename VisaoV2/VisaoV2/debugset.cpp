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
}

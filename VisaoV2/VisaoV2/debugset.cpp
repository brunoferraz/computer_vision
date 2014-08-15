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
            20, 30,
            0,  60,
            50, 40,
            30, 20;
    trabalho_1->points = pointList;
    //std::cout << pointList << std::endl;
    debugSetPack.push_back(trabalho_1);
}

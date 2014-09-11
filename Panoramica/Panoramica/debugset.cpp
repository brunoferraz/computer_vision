#include "debugset.h"
#include <QDebug>

DebugSet::DebugSet()
{
}

void DebugSet::prepareDebug()
{
    MatrixXf primeira(8,3);
    primeira << 134,  54,   1,
                212, 192,   1,
                211,  48,   1,
                346, 235,   1,
                361,  66,   1,
                403, 184,   1,
                418,  48,   1,
                289,  35,   1;

    MatrixXf segunda(8,3);
    segunda << 64,   54,   1,
               153, 197,   1,
               146,  46,   1,
               287, 229,   1,
               295,  62,   1,
               339, 178,   1,
               349,  46,   1,
               226,  33,   1;

    DebugSet::list.push_back(primeira);
    DebugSet::list.push_back(segunda);
}

QVector<Vector3f> DebugSet::getDebugSet(int indice)
{
    QVector<Vector3f> returnList;
    for(int i = 0; i < DebugSet::list.at(indice).rows(); i++){
        Vector3f vec;
        vec << DebugSet::list.at(indice)(i, 0), DebugSet::list.at(indice)(i, 1), DebugSet::list.at(indice)(i, 2);
        returnList.push_back(vec);
    }
    return returnList;
}
//Initialize static variables
QVector<MatrixXf> DebugSet::list = QVector<MatrixXf>();

#include "debugset.h"

DebugSet::DebugSet()
{

}

void DebugSet::start()
{
    packList.push_back(DebugPack("assets/houseBruno.000.corners",
                                  "assets/house.000.lines",
                                  "assets/house.000.pgm",
                                  "assets/house.000.P"));
    packList.push_back(DebugPack("assets/houseBruno.001.corners",
                                  "assets/house.001.lines",
                                  "assets/house.001.pgm",
                                  "assets/house.001.P"));
    packList.push_back(DebugPack("assets/houseLuis.000.corners",
                                  "assets/house.000.lines",
                                  "assets/houseLuis.000.pgm",
                                  "assets/house.000.P"));
    packList.push_back(DebugPack("assets/houseLuis.001.corners",
                                  "assets/house.001.lines",
                                  "assets/houseLuis.001.pgm",
                                  "assets/house.001.P"));
    packList.push_back(DebugPack("assets/house.000.corners",
                                  "assets/house.000.lines",
                                  "assets/house.000.pgm",
                                  "assets/house.000.P"));
    packList.push_back(DebugPack("assets/house.001.corners",
                                  "assets/house.001.lines",
                                  "assets/house.001.pgm",
                                  "assets/house.001.P"));
    packList.push_back(DebugPack("assets/house.002.corners",
                                  "assets/house.002.lines",
                                  "assets/house.002.pgm",
                                  "assets/house.002.P"));
    packList.push_back(DebugPack("assets/house.003.corners",
                                  "assets/house.003.lines",
                                  "assets/house.003.pgm",
                                  "assets/house.003.P"));
    packList.push_back(DebugPack("assets/house.004.corners",
                                  "assets/house.004.lines",
                                  "assets/house.004.pgm",
                                  "assets/house.004.P"));
    packList.push_back(DebugPack("assets/house.005.corners",
                                  "assets/house.005.lines",
                                  "assets/house.005.pgm",
                                  "assets/house.005.P"));
    packList.push_back(DebugPack("assets/house.006.corners",
                                  "assets/house.006.lines",
                                  "assets/house.006.pgm",
                                  "assets/house.006.P"));
    packList.push_back(DebugPack("assets/house.007.corners",
                                  "assets/house.007.lines",
                                  "assets/house.007.pgm",
                                  "assets/house.007.P"));
    packList.push_back(DebugPack("assets/house.008.corners",
                                  "assets/house.008.lines",
                                  "assets/house.008.pgm",
                                  "assets/house.008.P"));
    packList.push_back(DebugPack("assets/house.009.corners",
                                  "assets/house.009.lines",
                                  "assets/house.009.pgm",
                                  "assets/house.009.P"));
}
QList<DebugPack> DebugSet::packList;
int DebugSet::imageNumbers = 2;
//QString DebugSet::matchFilePath = "assets/house.nview-corners";
//QString DebugSet::matchFilePath = "assets/houseLuis.nview-corners";
QString DebugSet::matchFilePath = "assets/houseBruno.nview-corners";

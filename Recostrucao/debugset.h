#ifndef DEBUGSET_H
#define DEBUGSET_H

#include <QString>
#include <QList>

class DebugPack{
public:
    DebugPack(QString p, QString l, QString i, QString c):
        pointsPath(p), linesPath(l), imgPath(i), camMatrixPath(c){}
    QString pointsPath;
    QString linesPath;
    QString imgPath;
    QString camMatrixPath;
};

class DebugSet
{
public:
    DebugSet();
    static const bool isDebug = true;
    static const bool isTrackBackMode = false;
    static void start();
    static QList<DebugPack> packList;
    static QString matchFilePath;
    static int imageNumbers;
    static const int siftParameter = 500;
};

#endif // DEBUGSET_H

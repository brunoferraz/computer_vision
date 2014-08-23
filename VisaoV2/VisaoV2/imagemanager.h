#ifndef IMAGEMANAGER_H
#define IMAGEMANAGER_H

#include <QWidget>
#include <QImage>
#include <QFileDialog>
#include <QDebug>
#include <QString>

class ImageManager : public QWidget
{
    Q_OBJECT
public:
    explicit ImageManager(QWidget *parent = 0);
    QImage openImage(QString *path =0);
signals:
public slots:

};

#endif // IMAGEMANAGER_H

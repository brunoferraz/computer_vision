#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextStream>
#include <QList>
#include <imagecanvas.h>
#include <pair.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QList <ImageCanvas *> listCanvas;
    QList <Pair *> listPair;

    Eigen::Matrix3f F;
    Eigen::Matrix3f Fl;
    Eigen::Matrix3f E;
    QList<Eigen::Vector4f> finalList;
    QList<QList<Eigen::Vector4f>> newfinalList;

    void findPairs();
    void normalize();

    void calculateF();
    void applySingularContraints();
    void denormalize();
    void calculateK();
    void calculateE();
    void calculateP();

    void triangulation();
    void triangulation(Pair& pair);
    void triangulation2(Pair& pair);

    void writeFile();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H

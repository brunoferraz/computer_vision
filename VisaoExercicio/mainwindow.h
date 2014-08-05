#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include "imageviewer.h"
#include "imageviewer_ex2.h"
#include "imageviewer_ex3.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ImageViewer *image;
    ImageViewer_ex2 *image2;
    ImageViewer_ex3 *image3;

    ~MainWindow();

private slots:
    void on_adjustButton_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H

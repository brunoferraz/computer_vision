#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "imagemanager.h"
#include "displaywindow.h"
#include <debugset.h>
#include <QCloseEvent>

#define WORK_0  0
#define WORK_1  1
#define WORK_2  2
#define WORK_3  3

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    int STATE;
    bool isDebug;
    DebugSet debugSet;
    ImageManager imageManager;
    DisplayWindow displayWindow;

    void LoadImage();
protected:
    void closeEvent(QCloseEvent *ev);

private slots:
    void on_pushButton_clicked();
    void on_bt_DebugMode_toggled(bool checked);

private:
    Ui::MainWindow *ui;
    void setupProgram();

};

#endif // MAINWINDOW_H

#ifndef RESULTWINDOW_H
#define RESULTWINDOW_H

#include <QDialog>
#include <QLabel>
#include <QWidget>
#include <renderarea.h>

namespace Ui {
class ResultWindow;
}

class ResultWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ResultWindow(QWidget *parent = 0);
    ~ResultWindow();
    RenderArea *renderArea;
private:
    Ui::ResultWindow *ui;
};

#endif // RESULTWINDOW_H

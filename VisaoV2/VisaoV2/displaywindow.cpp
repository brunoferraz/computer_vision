#include "displaywindow.h"
#include "ui_displaywindow.h"
#include <QDebug>
#include <QDesktopWidget>
#include <renderarea.h>

DisplayWindow::DisplayWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DisplayWindow)
{
    ui->setupUi(this);
    RenderArea *area = ui->label;
    connect(area,SIGNAL(imageClicked(QMouseEvent*)),this,SLOT(windowClicked(QMouseEvent*)));
}

DisplayWindow::~DisplayWindow()
{
    delete ui;
}

void DisplayWindow::showImage(QImage img)
{
    currentImage = img;
    scale = 1.0;
    ui->label->resize(img.width(), img.height());
    DisplayWindow::resize(img.width(), img.height());
    ui->label->setPixmap(QPixmap::fromImage(img));
}

void DisplayWindow::zoom(float factor)
{
    scale = factor;
    QPixmap map = QPixmap::fromImage(currentImage);
    map = map.scaledToHeight(map.width() * factor);
    DisplayWindow::resize(map.width(), map.height());
    ui->label->resize(map.width(), map.height());
    ui->label->setPixmap(map);
}

void DisplayWindow::updateWindow()
{
    //Show or hide pins and lines based on pinmanagerlist
}

void DisplayWindow::windowClicked(QMouseEvent *ev)
{
    if(ev->button()==1)
    {
        pinmanager.addPin(ev->pos().x(), ev->pos().y());
    }
    else if(ev->button()==2)
    {
        pinmanager.removeLastPin();
    }
    updateWindow();
}

void DisplayWindow::paintEvent(QPaintEvent *ev)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setBrush(Qt::red);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(0, 0, 50, 50);

}

void DisplayWindow::wheelEvent(QWheelEvent *ev)
{
    if(ev->delta()>0){
        this->zoom(scale*1.1);
    }else{
        this->zoom(scale/1.1);
    }
    ev->accept();
}

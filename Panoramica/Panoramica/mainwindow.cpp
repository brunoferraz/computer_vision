#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    if(isDebug)
    {
        counter = 0;
        QString *str = new QString(QDir::currentPath() +"/assets");
        openFiles(str);
    }else{
        openFiles();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::openFiles(QString *path)
{
    MainWindow *telaTemp = this;
    if(!path){
        QFileDialog dialog(this);
        dialog.setDirectory(QDir::currentPath() + "/assets");
        dialog.setFileMode(QFileDialog::ExistingFile);
        dialog.setNameFilter("Images (*.png *.xpm *.jpg)");
        QStringList fileNames;
        if (dialog.exec())
        fileNames = dialog.selectedFiles();
        //qDebug() << fileNames;
    }else{
        list.push_back(QImage(QDir::currentPath() + "/assets/pan_0_mini.png"));
        list.push_back(QImage(QDir::currentPath() + "/assets/pan_1_mini.png"));
        for(int i = 0 ; i < list.count(); i++){
           RenderArea *tempArea = new RenderArea(this);
           QImage tempImg = list.at(i);
           tempArea->resize(tempImg.width(), tempImg.height());
           tempArea->move((tempImg.width()+ 10) * i + 10, 20);
           list_renderArea.push_back(tempArea);
           list_renderArea.at(i)->setPixmap(QPixmap::fromImage(tempImg));
           connect(tempArea,SIGNAL(renderAreaClicked(QMouseEvent*)),telaTemp,SLOT(getPointManual(QMouseEvent*)));
        }
     }

}

void MainWindow::adjustImage()
{
    qDebug() << "ajustando Imagem";
    //Iterate each image and its next
    for(int i = 0; i < list.count()-1; i++)
    {
        int numRows = list_renderArea.at(i)->pointList.count() * 2;
        MatrixXf A(numRows, 9);
        QVector<Vector3f> O = list_renderArea.at(i)->getNormalizedPoints();
        QVector<Vector3f> T = list_renderArea.at(i + 1)->getNormalizedPoints();
        MatrixXf lineA(1,9);
        MatrixXf lineB(1,9);
        for(int j = 0; j < O.count();j++){
            lineA << 0, 0, 0,
                    -T.at(j)(2) * O.at(j)(0),
                    -T.at(j)(2) * O.at(j)(1),
                    -T.at(j)(2) * O.at(j)(2),
                    T.at(j)(1) * O.at(j)(0),
                    T.at(j)(1) * O.at(j)(1),
                    T.at(j)(1) * O.at(j)(2);

            lineB << T.at(j)(2) * O.at(j)(0),
                    T.at(j)(2) * O.at(j)(1),
                    T.at(j)(2) * O.at(j)(2),
                    0,  0,  0,
                    -T.at(j)(0) * O.at(j)(0),
                    -T.at(j)(0) * O.at(j)(1),
                    -T.at(j)(0) * O.at(j)(2);
            A.row(i) << lineA;
            A.row(i + 1) << lineB;
        }
        JacobiSVD<MatrixXf> SVD(A, ComputeFullU | ComputeFullV);
        VectorXf S = SVD.matrixV().col(SVD.matrixV().cols() - 1);

        Matrix3f H;
        H <<    S(0), S(1), S(2),
                S(3), S(4), S(5),
                S(6), S(7), S(8);

        std::cout << H << std::endl;
    }
}

void MainWindow::getPointManual(QMouseEvent *ev)
{
    counter++;
    qDebug() << ev->pos().x() << ev->pos().y();
    if(counter == 2){
        adjustImage();
    }
}

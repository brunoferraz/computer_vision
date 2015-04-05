#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <util.h>
#include <Eigen/Eigen>
#include <Eigen/Dense>
#include <eigen3/Eigen/LU>
#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/SVD>
#include <debugset.h>
#include <QLabel>
#include <QImage>
#include <QPixmap>
#include <QTabWidget>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    DebugSet::start();


    QTabWidget *qtab = new QTabWidget(this);
    ImageCanvas *c = new ImageCanvas(qtab);
    c->start(DebugSet::packList.at(0));
    this->resize(c->size());
    qtab->setFixedSize(c->size());
    qtab->addTab(c, "0");
    listCanvas.push_back(c);
    for(int i = 1; i < DebugSet::imageNumbers; i++){
        ImageCanvas *c = new ImageCanvas(qtab);
        c->start(DebugSet::packList.at(i));
        QString t = QString::number(i);
        qtab->addTab(c, t);
        qtab->show();
        listCanvas.push_back(c);
    }
    for(int i = 0; i < listCanvas.length()-1; i++){
        Pair *p = new Pair(listCanvas.at(i),listCanvas.at(i+1));
        listPair.push_back(p);
    }
    normalize();
    //montar uma estrutura que sirva tanto para o data set quanto para os pontos do SIFT

    //    1 calcular F usando o método de 8 ou 7 pontos, comecem com o de 8 que é mais fácil
    calculateF();
//    2 com a câmera calibrada vocês já tem K, então usem K para achar E a partir de F
    calculateK();
    calculateE();
//    3 a partir de E encontrem P e P', são quatro soluções possíveis,  devem achar a justa
    calculateP();
//    4 com P e P' façam a triangulação para encontrar os pontos em 3D, tem dois métodos,  não precisa fazer o de polinômio de sexto grau, o outro funciona bem
    triangulation(*listPair.at(0));
//    triangulation();
    writeFile();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::normalize()
{
    //find centroid
    for(int i = 0; i < listCanvas.length(); i++){
        listCanvas.at(i)->normalize();
    }
}

void MainWindow::calculateF()
{
    int total = listCanvas.at(0)->listPointsDebug.length();
    QList<int> randomPairs;
    int counter = 8;
    while(counter){
        bool flag = false;
        while(!flag){
            int random = std::rand() % total;
            if(randomPairs.indexOf(random)==-1){
                randomPairs.push_back(random);
                flag = true;
                counter--;
            }
        }
    }
    Eigen::MatrixXf A(total,9);
    //for(int i = 0; i < randomPairs.length();i++){
    for(int i = 0; i < total;i++){
        Eigen::VectorXf r(9);
        Eigen::Vector3f x;
        Eigen::Vector3f xl;
        //x = listCanvas.at(0)->listPointsDebug.at(randomPairs.at(i));
        //xl = listCanvas.at(1)->listPointsDebug.at(randomPairs.at(i));
        x =  listCanvas.at(0)->listPointsDebug.at(i);
        xl = listCanvas.at(1)->listPointsDebug.at(i);
        r<<     xl(0) * x(0),
                xl(0) * x(1),
                xl(0),
                xl(1) * x(0),
                xl(1) * x(1),
                xl(1),
                x(0),
                x(1),
                1;
       A.row(i) << r.transpose();
    }
    //std::cout << A << std::endl;
    Eigen::JacobiSVD<Eigen::MatrixXf> SVD(A, Eigen::ComputeFullV);
    Eigen::VectorXf x = SVD.matrixV().col(SVD.matrixV().cols() - 1);
    F <<    x(0), x(1), x(2),
            x(3), x(4), x(5),
            x(6), x(7), x(8);

    applySingularContraints();
    denormalize();
    F/= F(2,2);
    F*=1.06047;
    if(DebugSet::isTrackBackMode){
        F <<     2.53304e-06, -3.55338e-05,   0.00943417,
                 3.95356e-05,  2.05369e-06,   -0.0133847,
                  -0.0124648,    0.0103014,      1.06047;
    }
   // std::cout << F << std::endl;
}

void MainWindow::applySingularContraints()
{
    Eigen::JacobiSVD<Eigen::MatrixXf> SVD(F, Eigen::ComputeFullV | Eigen::ComputeFullU);
    Eigen::VectorXf singularValues;
    singularValues = SVD.singularValues();
//    std::cout << singularValues << std::endl;
//    Eigen::MatrixXf D = F * SVD.matrixU().inverse() * SVD.matrixV().transpose().inverse();
    Eigen::DiagonalMatrix<float, 3,3> D(singularValues(0), singularValues(1), 0);
    Eigen::Matrix3f DMat = D.toDenseMatrix();
    Fl = SVD.matrixU() * DMat * SVD.matrixV().transpose();
    F = Fl;
}

void MainWindow::denormalize()
{
    Eigen::Matrix3f Tl = listPair.at(0)->list.at(1)->T;
    Eigen::Matrix3f T =  listPair.at(0)->list.at(0)->T;

    F = Tl.transpose() * F * T;

    listPair.at(0)->list.at(0)->denormalize();
    listPair.at(0)->list.at(1)->denormalize();
}

void MainWindow::calculateK()
{
    for(int i = 0; i < listCanvas.length(); i++){
        listCanvas.at(i)->calculateK();
    }
    if(DebugSet::isTrackBackMode){
        listCanvas.at(0)->K << 667.077,     -1.91488,      399.499,
                               -3.39867e-14,      673.565,      266.288,
                                8.83083e-17,            0,      1.00122;

        listCanvas.at(1)->K << 655.409,     -1.95048,      389.643,
                               -7.10543e-14,        652.5,      275.113,
                               -2.77556e-17,            0,     0.989163;
    }
}

void MainWindow::calculateE()
{
    E = listCanvas.at(1)->K.transpose() * F * listCanvas.at(0)->K;

    Eigen::JacobiSVD<Eigen::MatrixXf> SVD(E, Eigen::ComputeFullV | Eigen::ComputeFullU);
    Eigen::VectorXf singularValues;
    singularValues = SVD.singularValues();
    float a, b;
    a = singularValues(0);
    b = singularValues(1);

    cout << "E " << a << "  " << b << endl;

    // Eigen::DiagonalMatrix<float, 3,3> D((a+b)/2, (a+b)/2, 0);
    Eigen::DiagonalMatrix<float, 3,3> D(1,1, 0);
    Eigen::Matrix3f DMat = D.toDenseMatrix();
    std::cout << DMat << std::endl;
    E = SVD.matrixU() * DMat * SVD.matrixV().transpose();

    E /= E(2,2);
    E *= 0.034732;

    if(DebugSet::isTrackBackMode){
        E<< 1.10747,  -15.6899, 0.652399,
            17.2053,  0.899896,    1.9166,
          -0.310845,  -2.08089,  0.034732;
    }
}

void MainWindow::calculateP()
{
     Eigen::Matrix3f W;
     W <<   0,  -1,  0,
            1,   0,  0,
            0,   0,  1;
     Eigen::Matrix3f Z;
     Z <<   0,  1,  0,
           -1,  0,  0,
            0,  0,  0;
     Eigen::Matrix3f U, V, D;
     Eigen::JacobiSVD<Eigen::MatrixXf> SVD(E, Eigen::ComputeFullU | Eigen::ComputeFullV);
     Eigen::VectorXf x = SVD.matrixV().col(SVD.matrixV().cols() - 1);
     U = SVD.matrixU();
     V = SVD.matrixV();
     D <<   1, 0,  0,
            0, 1,  0,
            0, 0,  0;

    if((U*V.transpose()).determinant() < 0)
    {
        V.col(2) *= -1;
    }

    Eigen::Vector3f u3;
//    u3 = U.col(U.cols() -1);
    u3 = U.col(2);
    Eigen::MatrixXf temp;
    Eigen::MatrixXf P1(3,4);
    Eigen::MatrixXf P2(3,4);
    Eigen::MatrixXf P3(3,4);
    Eigen::MatrixXf P4(3,4);

    P1.topLeftCorner(3,3) = U * W * V.transpose();
    P1.col(3) = u3;

    P2.topLeftCorner(3,3) = U * W * V.transpose();
    P2.col(3) = -u3;

    P3.topLeftCorner(3,3) = U * W.transpose() * V.transpose();
    P3.col(3) = u3;

    P4.topLeftCorner(3,3) = U * W.transpose() * V.transpose();
    P4.col(3) = -u3;

    listPair.at(0)->listP.push_back(P1);
    listPair.at(0)->listP.push_back(P2);
    listPair.at(0)->listP.push_back(P3);
    listPair.at(0)->listP.push_back(P4);

//    QString camMatrix   = Util::openFile(DebugSet::packList.at(1).camMatrixPath.toStdString());

    //P1 = Util::parseCamMatrix(camMatrix);

//    camMatrix   = Util::openFile(DebugSet::packList.at(0).camMatrixPath.toStdString());
//    P2 = Util::parseCamMatrix(camMatrix);

//    P1 = P1 * P2.inverse();
    //listPair.at(0)->listP.push_back(P1);

//    Util::printQList(listPair.at(0)->listP);
}

void MainWindow::triangulation()
{
    int total = listPair.at(0)->goodMatchesDebug.length();
    //std::cout << listPair.at(0)->list.at(0)->P << std::endl;
    for(int i = 0; i < total ;i++){
        int a, b;
        a = listPair.at(0)->goodMatchesDebug.at(i)(0);
        b = listPair.at(0)->goodMatchesDebug.at(i)(1);
        if(a < total || b < total){
            QList<Eigen::Vector3f> *L;
            QList<Eigen::Vector3f> *Ll;
            if(DebugSet::isDebug){
                L   = &listPair.at(0)->list.at(0)->listPoints;
                Ll  = &listPair.at(0)->list.at(1)->listPoints;
            }else{
                L   = &listPair.at(0)->list.at(0)->listPoints;
                Ll  = &listPair.at(0)->list.at(1)->listPoints;
            }

            Eigen::Vector3f x   = L->at(a);
            Eigen::Vector3f xl  = Ll->at(b);

            Eigen::MatrixXf p   = listPair.at(0)->list.at(0)->P;
            Eigen::MatrixXf pl  = listPair.at(0)->list.at(1)->P;
            Eigen::Matrix4f A;

            Eigen::Vector4f v0, v1, v2, v3;
            v0 = x(0) * p.row(2) - p.row(0);
            v1 = x(1) * p.row(2) - p.row(1);
            v2 = xl(0) * pl.row(2) - pl.row(0);
            v3 = xl(1) * pl.row(2) - pl.row(1);

            A << v0.transpose(), v1.transpose(), v2.transpose(), v3.transpose();
            Eigen::JacobiSVD<Eigen::MatrixXf> SVD(A, Eigen::ComputeFullV);
            Eigen::VectorXf X = SVD.matrixV().col(SVD.matrixV().cols() - 1);
            X /= X(3);


            finalList.push_back(X);
        }
    }
    newfinalList.push_back(finalList);
}

void MainWindow::triangulation(Pair& pair)
{
    int total = listPair.at(0)->goodMatchesDebug.length();
    //std::cout << listPair.at(0)->list.at(0)->P << std::endl;
    for(int j = 0; j < pair.listP.length() ;j++){
        finalList.clear();
        for(int i = 0; i < total ;i++){
            int a, b;
            a = pair.goodMatchesDebug.at(i)(0);
            b = pair.goodMatchesDebug.at(i)(1);
            if(a < total || b < total){
                QList<Eigen::Vector3f> *L;
                QList<Eigen::Vector3f> *Ll;

                if(DebugSet::isDebug){
                    L   = &pair.list.at(0)->listPoints;
                    Ll  = &pair.list.at(1)->listPoints;
                }else{
                    L   = &pair.list.at(0)->listPoints;
                    Ll  = &pair.list.at(1)->listPoints;
                }
                Eigen::Vector3f x   = L->at(a);
                Eigen::Vector3f xl  = Ll->at(b);
                x = pair.list.at(0)->K.inverse() * x;
                xl = pair.list.at(1)->K.inverse() * xl;
//                Eigen::VectorXf X = Util::linearTriangulation(x, pair.listP.at(j), xl);
                Eigen::MatrixXf p(3,4);
                p<<    1,  0,  0,  0,
                       0,  1,  0,  0,
                       0,  0,  1,  0;
                //p = pair.list.at(0)->P;
                Eigen::MatrixXf pl(3,4);
                pl = pair.listP.at(j);
                Eigen::Matrix4f A;

                Eigen::Vector4f v0, v1, v2, v3;
                v0 = x(0) * p.row(2) - p.row(0);
                v1 = x(1) * p.row(2) - p.row(1);
                v2 = xl(0) * pl.row(2) - pl.row(0);
                v3 = xl(1) * pl.row(2) - pl.row(1);

                A << v0.transpose(), v1.transpose(), v2.transpose(), v3.transpose();
                Eigen::JacobiSVD<Eigen::MatrixXf> SVD(A, Eigen::ComputeFullV);
                Eigen::VectorXf X = SVD.matrixV().col(SVD.matrixV().cols() - 1);
                X /= X(3);

                finalList.push_back(X);
            }
        }
        newfinalList.push_back(finalList);
    }
}

void MainWindow::writeFile()
{
    for(int j=0; j< newfinalList.length(); j++){
        QString file;
        QList<Eigen::Vector4f> l;
        l = newfinalList.at(j);
        for(int i = 0 ; i < l.length(); i++){
            float x = l.at(i)(0);
            float y = l.at(i)(1);
            float z = l.at(i)(2);
            file +=  "v " + QString::number(x) + " " + QString::number(y) + " " + QString::number(z) + "\n";
        }
        file += "f 0 1 2\n";
        file += "f 0 2 3\n";
        file += "f 0 3 4\n";
        file += "f 4 3 5\n";
        file += "f 6 4 5\n";
        file += "f 6 5 7\n";
        file += "f 8 5 7\n";
        file += "f 9 8 7\n";
        file += "f 9 7 10\n";
        file += "f 6 5 7\n";
        file += "f 10 7 5\n";
        file += "f 10 5 11\n";
        file += "f 11 3 2\n";
        file += "f 11 2 12\n";


        Util::saveFile("assets/points_" + QString::number(j) + ".obj", file);
    }

    std::ofstream reg("assets/DATA.txt");
    reg << "Fundamental Matrix: " << endl << F << endl;
    reg << endl;
    reg << "Essential Matrix: " << endl << E << endl;
    reg << endl;
    reg << "Camera Matrices" << endl;

     for(int i = 0 ; i < listPair.at(0)->listP.length() ; i++){
        reg << "P0: " << endl
        << listPair.at(0)->listP.at(i) <<endl;
    }
    reg << endl;
    reg << "Calibration Matrices" << endl << "K: " << endl << listCanvas.at(0)->K << endl << "K': " << endl << listCanvas.at(1)->K << endl;
    reg.close();
}

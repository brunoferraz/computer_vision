#include "imageviewer_ex2.h"
#include "pinmanager.h"
#include <QDebug>
#include <eigen3/Eigen/Cholesky>

ImageViewer_ex2::ImageViewer_ex2(QWidget *parent) :
    ImageViewer(parent)
{
    scale = new float(1.0);
    isDebug = false;
    pinmanager = new PinManager(this);
    pinmanager->setType(pinmanager->TYPE_THREE);
}

void ImageViewer_ex2::open(QString *path)
{
    ImageViewer::open(path);
    isDebug = false;
    if(isDebug){
        prepareDebug();
    }

}

void ImageViewer_ex2::adjustImage(float w, float h)
{
    Vector3f hl(3,1);
    hl = pinmanager->getHorizonLine().transpose();
    hl = hl/ hl(2);

    MatrixXf temp(3,3);
    temp << 1, 0, 0, 0, 1, 0, hl(0), hl(1), hl(2);
    Hp = temp;

    H = Hp;
    Hi = H.inverse();

    QSize imgSize(this->width(), this->height());
    QVector<QPoint> areaRender;
    areaRender << QPoint(0,0) << QPoint(0, imgSize.height()) << QPoint(imgSize.width(), imgSize.height()) << QPoint(imgSize.width(), 0);
    showResult(imgSize, areaRender);
    imageBase = imageResult;
    pinmanager->setType(pinmanager->TYPE_FOUR);
    if(isDebug){
        prepareDebug();
    }
}
void ImageViewer_ex2::adjustimageAffineSimilarity()
{
    Vector3f l(3,1);
    Vector3f m(3,1);
    Vector3f r1(3,1);
    Vector3f r2(3,1);
    MatrixXf A(2,3);

    l = pinmanager->getLine(0);
    m = pinmanager->getLine(1);

    r2 << l(0) * m(0), l(0) * m(1) + l(1) * m(0), l(1) * m(1);

    l = pinmanager->getLine(2);
    m = pinmanager->getLine(3);

    r1 << l(0) * m(0), l(0) * m(1) + l(1) * m(0), l(1) * m(1);

    A << r1.transpose(), r2.transpose();
    JacobiSVD<MatrixXf> SVD(A, ComputeFullV);
    VectorXf S = SVD.matrixV().col(SVD.matrixV().cols() - 1);

    //S /= S(2);
    S(2) = 1;

    MatrixXf kkt(2,2);
    kkt << S(0), S(1), S(1), S(2);

    LLT<MatrixXf> lltOfA(kkt);
    MatrixXf L = lltOfA.matrixU();

    H << L(0), L(1), 0, L(2), L(3), 0, 0, 0, 1;
    //std::cout << H << std::endl;
    H = H.inverse();

    QSize imgSize(this->width(), this->height());
    QVector<QPoint> areaRender;
    areaRender << QPoint(0,0) << QPoint(0, imgSize.height()) << QPoint(imgSize.width(), imgSize.height()) << QPoint(imgSize.width(), 0);
    showResult(imgSize, areaRender);
}

void ImageViewer_ex2::mouseReleaseEvent(QMouseEvent *ev)
{
    if(pinmanager->pinlist.count()<pinmanager->MaxPins){
        pinmanager->createPin(ev->pos());
    }else{
        switch (pinmanager->type) {
        case PinManager::TYPE_THREE:
            adjustImage(this->width(), this->height());
            break;
        case PinManager::TYPE_FOUR:
            adjustimageAffineSimilarity();
            break;
        default:
            break;
        }
    }
}
void ImageViewer_ex2::prepareDebug()
{
    switch (pinmanager->type) {
    case PinManager::TYPE_THREE:
        pinmanager->createPin(QPoint(160,273));
        pinmanager->createPin(QPoint(379,110));
        pinmanager->createPin(QPoint(90,208));
        pinmanager->createPin(QPoint(308,74));
        pinmanager->createPin(QPoint(186,76));
        pinmanager->createPin(QPoint(409,201));
        pinmanager->createPin(QPoint(341,266));
        pinmanager->createPin(QPoint(118,113));
        break;
    case PinManager::TYPE_FOUR:
        pinmanager->createPin(QPoint(226,88));
        pinmanager->createPin(QPoint(324,217));
        pinmanager->createPin(QPoint(223,220));
        pinmanager->createPin(QPoint(354,126));
        pinmanager->createPin(QPoint(160,271));
        pinmanager->createPin(QPoint(209,132));
        pinmanager->createPin(QPoint(126,181));
        pinmanager->createPin(QPoint(255,175));
        break;
    default:
        break;
    }
}

void ImageViewer_ex2::showResult(QSize imgSize, QVector<QPoint> areaRender)
{
    ImageViewer::showResult(imgSize, areaRender);
    pinmanager->hide_pins();
}

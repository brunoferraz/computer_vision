#include "util.h"

Util::Util()
{

}

QList<Eigen::Vector3f> Util::parsePoints(const QString str)
{
    QStringList list;
    QList<Eigen::Vector3f> pointList;
    list = str.split("\n");
    for(int i = 0; i< list.length()-1; i++){
        Eigen::Vector3f v;
        QStringList point = list.at(i).split(" ");
        v << point.at(0).toFloat(), point.at(1).toFloat(), 1;
        pointList.push_back(v);
    }
    return pointList;
}

QList<Eigen::MatrixXf> Util::parseLines(const QString str)
{
    QList<Eigen::MatrixXf> list;
    std::istringstream iss(str.toStdString().c_str());
    while(!iss.eof()){
        float x1, y1, x2, y2;
        iss >> x1;
        iss >> y1;
        iss >> x2;
        iss >> y2;
        Eigen::MatrixXf m(2,2);
        m << x1, y1, x2, y2;
        list.push_back(m);
    }
    return list;
}

QList<Eigen::Vector2i> Util::parseMatchs(const QString str, int a, int b)
{
    QList<Eigen::Vector2i> list;
    std::istringstream iss(str.toStdString().c_str());
    while(!iss.eof()){
        QList<int> l;
        for(int i = 0; i < 10; i++){
            std::string n;
            iss >> n;
            //std::cout << atoi(n.c_str()) << std::endl;
            if(n!="*"){
                l.push_back(atoi(n.c_str()));
            }else{
                l.push_back(-1);
            }
        }
        if(l.at(a)!=-1 && l.at(b)!=-1){
            Eigen::Vector2i v;
            v << l.at(a), l.at(b);
            list.push_back(v);
        }
    }
    return list;
}
Eigen::MatrixXf Util::parseCamMatrix(const QString str)
{
    std::istringstream iss(str.toStdString().c_str());
    int rows = 3;
    Eigen::MatrixXf m(rows,4);
    int counter  = 0;
    while(!iss.eof()){
        float a, b, c, d;
        iss >> a;
        iss >> b;
        iss >> c;
        iss >> d;
        Eigen::Vector4f v;
        if(counter<rows){
            v << a, b, c, d;
            m.row(counter) << v.transpose();
        }
        counter++;
    }
    return m;
}

QString Util::openFile(const string& str)
{
    QFile file(str.c_str());
    file.open(QIODevice::ReadOnly|QFile::Text);
    QTextStream in(&file);    // read the data serialized from the file
    QString stra = in.readAll();

//    std::ifstream ifile(str.c_str());
//    while(!ifile.eof())
//    {
//        float a1, a2, a3, a4, a5, a6;
//        ifile >> a1 >> a2 >> a3 >> a4 >> a5 >> a6;
//        qDebug() << a1;
//    }

//    ifile.close();


    return stra;
}

void Util::saveFile(const QString path, const QString text)
{
    QFile file(path);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);
    out << text;
       // optional, as QFile destructor will already do it:
    file.close();
}

void Util::printQList(const QList<Eigen::Vector3f> v)
{
    for(int i = 0; i < v.length(); i ++)
    {
        std::cout << v.at(i).transpose() << std::endl;
    }
}

void Util::printQList(const QList<Eigen::VectorXi> v)
{
    for(int i = 0; i < v.length(); i ++)
    {
        std::cout << v.at(i).transpose() << std::endl;
    }
}

void Util::printQList(const QList<Eigen::Vector2i> v)
{
    for(int i = 0; i < v.length(); i ++)
    {
        std::cout << v.at(i).transpose() << std::endl;
    }
}
void Util::printQList(const QList<Eigen::MatrixXf> v)
{
    for(int i = 0; i < v.length(); i ++)
    {
        std::cout << v.at(i) << std::endl;
    }
}
void Util::printQList(const QList<int> v)
{
    for(int i = 0; i < v.length(); i ++)
    {
        qDebug() << v.at(i);
    }
}

QImage Util::openImage(QString path)
{
    QString fileName = QDir::currentPath() + "/" + path;
    qDebug() << fileName;
    QImage *imageBase = new QImage(fileName);
    return *imageBase;
}

QImage Util::Mat2QImage(const cv::Mat &inMat)
{

    switch ( inMat.type() )
    {
     // 8-bit, 4 channel
     case CV_8UC4:
     {
        QImage image( inMat.data, inMat.cols, inMat.rows, inMat.step, QImage::Format_RGB32 );
        return image;
     }

     // 8-bit, 3 channel
     case CV_8UC3:
     {
        QImage image( inMat.data, inMat.cols, inMat.rows, inMat.step, QImage::Format_RGB888 );
        return image.rgbSwapped();
     }

     // 8-bit, 1 channel
     case CV_8UC1:
     {
        static QVector<QRgb>  sColorTable;
        // only create our color table once
        if ( sColorTable.isEmpty() )
        {
           for ( int i = 0; i < 256; ++i )
              sColorTable.push_back( qRgb( i, i, i ) );
        }
        QImage image( inMat.data, inMat.cols, inMat.rows, inMat.step, QImage::Format_Indexed8 );
        image.setColorTable( sColorTable );
        return image;
     }

     default:
        qWarning() << "ASM::cvMatToQImage() - cv::Mat image type not handled in switch:" << inMat.type();
        break;
    }
    return QImage();
}

cv::Mat Util::QImage2Mat(const QImage &inImage, bool inCloneImageData)
{
    switch ( inImage.format() )
    {
       // 8-bit, 4 channel
       case QImage::Format_RGB32:
       {
          cv::Mat  mat( inImage.height(), inImage.width(), CV_8UC4, const_cast<uchar*>(inImage.bits()), inImage.bytesPerLine() );
          return (inCloneImageData ? mat.clone() : mat);
       }
       // 8-bit, 3 channel
       case QImage::Format_RGB888:
       {
          if ( !inCloneImageData )
             qWarning() << "ASM::QImageToCvMat() - Conversion requires cloning since we use a temporary QImage";

          QImage   swapped = inImage.rgbSwapped();
          return cv::Mat( swapped.height(), swapped.width(), CV_8UC3, const_cast<uchar*>(swapped.bits()), swapped.bytesPerLine() ).clone();
       }
       // 8-bit, 1 channel
       case QImage::Format_Indexed8:
       {
          cv::Mat  mat( inImage.height(), inImage.width(), CV_8UC1, const_cast<uchar*>(inImage.bits()), inImage.bytesPerLine() );
          return (inCloneImageData ? mat.clone() : mat);
       }
       default:
          qWarning() << "ASM::QImageToCvMat() - QImage format not handled in switch:" << inImage.format();
          break;
    }
    return cv::Mat();
}

QPixmap Util::Mat2QPixmap(const cv::Mat &inMat)
{
    QPixmap pix = QPixmap::fromImage(Util::Mat2QImage(inMat));
    return pix;
}

Eigen::VectorXf Util::linearTriangulation(const Eigen::Vector3f v, const Eigen::MatrixXf P, const Eigen::Vector3f vl)
{
    Eigen::Vector3f x   = v;
    Eigen::Vector3f xl  = vl;
    Eigen::MatrixXf p(3,4);
    p<<    1,  0,  0,  0,
           0,  1,  0,  0,
           0,  0,  1,  0;

    Eigen::MatrixXf pl  = P;
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
    return X;
}

void Util::SolveCS(float &c, float &s, float a, float b)
{
    float den = (a * a) + (b * b);
    c = -b/std::sqrt(den);
    s = a/std::sqrt(den);
}

void Util::RQdecomposition(Eigen::MatrixXf A, Eigen::Matrix3f &R, Eigen::Matrix3f &Q)
{
    float c, s;
    Eigen::Matrix3f Qx, Qy, Qz;
    Eigen::Matrix3f M;
    M <<    A(0,0), A(0,1), A(0,2),
            A(1,0), A(1,1), A(1,2),
            A(2,0), A(2,1), A(2,2);

    R = M;
    Util::SolveCS(c,s, R(2,1), R(2,2));
    Qx <<   1,  0,  0,
            0,  c,  -s,
            0,  s,  c;
    R*= Qx;

    Util::SolveCS(c,s, R(2,0), -R(2,2));
    Qy <<   c,  0,  s,
            0,  1,  0,
           -s,  0,  c;
    R*= Qy;

    Util::SolveCS(c,s, R(1,0), R(1,1));
    Qz <<   c,  -s, 0,
            s,  c,  0,
            0,  0,  1;
    R*= Qz;

    if(std::abs(R(1,0)) > 0.00005 || std::abs(R(2,0)) > 0.00005 || std::abs(R(2,1)) > 0.00005) qDebug() << "PROBLEM WITH RQdecomposition";

    if(R(1,0)!= 0) R(1,0) = 0;
    if(R(2,0)!= 0) R(2,0) = 0;
    if(R(2,1)!= 0) R(2,1) = 0;

    Q = Qx.transpose() * Qy.transpose() * Qz.transpose();
}

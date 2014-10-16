#include "util.h"

Util::Util()
{
}


QString Util::openFile(const string& str)
{
    QFile file(str.c_str());
    file.open(QIODevice::ReadOnly|QFile::Text);
    QTextStream in(&file);    // read the data serialized from the file
    QString stra = in.readAll();

    return stra;
}


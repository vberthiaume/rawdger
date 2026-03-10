#include <QCoreApplication>
#include <QDebug>
#include "rawdger/Core.h"

int main (int argc, char* argv[])
{
    QCoreApplication app (argc, argv);
    qDebug() << "Qt app says:" << rawdger::getString().c_str();
    return 0;
}

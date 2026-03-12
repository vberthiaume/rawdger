#include <QApplication>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QDebug>
#include "rawdger/Core.h"

int main (int argc, char* argv[])
{
    QApplication app (argc, argv);

    QWidget window;
    window.setWindowTitle ("Rawdger Qt");
    window.resize (300, 200);

    auto* layout = new QVBoxLayout (&window);
    auto* recordButton = new QPushButton ("Record");
    recordButton->setCheckable (true);
    layout->addWidget (recordButton);

    QObject::connect (recordButton, &QPushButton::toggled, [] (bool checked)
    {
        qDebug() << (checked ? "on" : "off");
    });

    window.show();
    return app.exec();
}

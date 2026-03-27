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

    // this looks like it might create a memory leak, but the parent passed in a QObject constructor
    // adds that object as a child in its object tree and takes ownership of it
    auto* layout = new QVBoxLayout (&window);

    auto* recordButton = new QPushButton ("Record");
    recordButton->setCheckable (true);
    // similarly, adding a widget to a layout also adds it to its object tree
    layout->addWidget (recordButton);

    // connect recordButton's toggled signal to a callback (can be a lambda, a function, or a QObject slot)
    QObject::connect (recordButton, &QPushButton::toggled, [] (bool checked)
    {
        // qDebug() is Qt's logging function, similar to std::cerr << "yada";
        qDebug() << rawdger::getString() << (checked ?  "on" : "off");
    });

    window.show();
    return app.exec();
}

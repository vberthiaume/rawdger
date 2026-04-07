#include <QApplication>
#include <QPushButton>
#include <QStandardPaths>
#include <QVBoxLayout>
#include <QWidget>
#include <QDebug>

#include "rawdger/Recorder.h"

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

    rawdger::Recorder recorder;

    // connect recordButton's toggled signal to a callback (can be a lambda, a function, or a QObject slot)
    QObject::connect (recordButton, &QPushButton::toggled, [&recorder] (bool checked)
                      {
        if (checked)
        {
            auto desktop = QStandardPaths::writableLocation (QStandardPaths::DesktopLocation);
            auto path = (desktop + "/" + QString::fromStdString (rawdger::generateWavFileName ("qt"))).toStdString();
            recorder.startRecording (path);
            qDebug() << "Recording to:" << path.c_str();
        }
        else
        {
            recorder.stopRecording();
            qDebug() << "Recording stopped.";
        } });

    window.show();
    return app.exec();
}

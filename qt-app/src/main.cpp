#include <QApplication>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QDebug>
#include "rawdger/Core.h"
#include "AudioRecorder.h"

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

    AudioRecorder recorder;

    QObject::connect (&recorder, &AudioRecorder::recordingStarted, [] (const QString& path)
    {
        qDebug() << "Recording started:" << path;
    });

    QObject::connect (&recorder, &AudioRecorder::recordingStopped, [] (const QString& path)
    {
        qDebug() << "Recording stopped:" << path;
    });

    QObject::connect (&recorder, &AudioRecorder::errorOccurred, [recordButton] (const QString& msg)
    {
        qDebug() << "Recording error:" << msg;
        recordButton->setChecked (false);
    });

    QObject::connect (recordButton, &QPushButton::toggled, [&recorder] (bool checked)
    {
        if (checked)
            recorder.startRecording();
        else
            recorder.stopRecording();
    });

    window.show();
    return app.exec();
}

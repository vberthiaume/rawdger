#pragma once

#include <QObject>
#include <QAudioSource>
#include <QMediaDevices>
#include <memory>

class WavFileWriter;

class AudioRecorder : public QObject
{
    Q_OBJECT

public:
    explicit AudioRecorder (QObject* parent = nullptr);
    ~AudioRecorder() override;

    void startRecording();
    void stopRecording();
    bool isRecording() const;

signals:
    void recordingStarted (const QString& path);
    void recordingStopped (const QString& path);
    void errorOccurred (const QString& message);

private:
    QString getNextRecordingFile() const;

    std::unique_ptr<QAudioSource> m_audioSource;
    std::unique_ptr<WavFileWriter> m_wavWriter;
    QString m_currentFile;
    bool m_recording = false;
};

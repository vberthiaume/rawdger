#include "AudioRecorder.h"
#include "WavFileWriter.h"
#include <QAudioFormat>
#include <QMediaDevices>
#include <QDir>
#include <QStandardPaths>
#include <QDateTime>

AudioRecorder::AudioRecorder (QObject* parent)
    : QObject (parent)
{
}

AudioRecorder::~AudioRecorder()
{
    if (m_recording)
        stopRecording();
}

void AudioRecorder::startRecording()
{
    if (m_recording)
        return;

    QAudioFormat format;
    format.setSampleRate (44100);
    format.setChannelCount (1);
    format.setSampleFormat (QAudioFormat::Int16);

    auto inputDevice = QMediaDevices::defaultAudioInput();
    if (! inputDevice.isFormatSupported (format))
    {
        emit errorOccurred ("Default audio input does not support the requested format.");
        return;
    }

    m_currentFile = getNextRecordingFile();

    // Ensure output directory exists
    QFileInfo fileInfo (m_currentFile);
    QDir().mkpath (fileInfo.absolutePath());

    m_wavWriter = std::make_unique<WavFileWriter> (m_currentFile);
    if (! m_wavWriter->open (QIODevice::WriteOnly))
    {
        emit errorOccurred ("Failed to open file: " + m_currentFile);
        m_wavWriter.reset();
        return;
    }

    m_audioSource = std::make_unique<QAudioSource> (inputDevice, format);
    m_audioSource->start (m_wavWriter.get());

    m_recording = true;
    emit recordingStarted (m_currentFile);
}

void AudioRecorder::stopRecording()
{
    if (! m_recording)
        return;

    m_audioSource->stop();
    m_wavWriter->close();

    m_audioSource.reset();
    m_wavWriter.reset();

    m_recording = false;
    emit recordingStopped (m_currentFile);
}

bool AudioRecorder::isRecording() const
{
    return m_recording;
}

QString AudioRecorder::getNextRecordingFile() const
{
    auto musicDir = QStandardPaths::writableLocation (QStandardPaths::MusicLocation);
    auto timestamp = QDateTime::currentDateTime().toString ("yyyyMMdd_HHmmss");
    return musicDir + "/Rawdger/recording_" + timestamp + ".wav";
}

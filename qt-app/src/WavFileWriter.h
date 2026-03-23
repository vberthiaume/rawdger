#pragma once

#include <QIODevice>
#include <QFile>
#include <memory>

class WavFileWriter : public QIODevice
{
    Q_OBJECT

public:
    explicit WavFileWriter (const QString& filePath, int sampleRate = 44100,
                            int bitsPerSample = 16, int numChannels = 1,
                            QObject* parent = nullptr);
    ~WavFileWriter() override;

    bool open (OpenMode mode) override;
    void close() override;

protected:
    qint64 readData (char* data, qint64 maxSize) override;
    qint64 writeData (const char* data, qint64 maxSize) override;

private:
    void writeHeader();
    void patchHeader();

    QString m_filePath;
    std::unique_ptr<QFile> m_file;
    int m_sampleRate;
    int m_bitsPerSample;
    int m_numChannels;
    quint32 m_dataSize = 0;
};

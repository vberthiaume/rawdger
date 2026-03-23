#include "WavFileWriter.h"
#include <QDataStream>

WavFileWriter::WavFileWriter (const QString& filePath, int sampleRate,
                              int bitsPerSample, int numChannels, QObject* parent)
    : QIODevice (parent)
    , m_filePath (filePath)
    , m_sampleRate (sampleRate)
    , m_bitsPerSample (bitsPerSample)
    , m_numChannels (numChannels)
{
}

WavFileWriter::~WavFileWriter()
{
    if (isOpen())
        close();
}

bool WavFileWriter::open (OpenMode mode)
{
    m_file = std::make_unique<QFile> (m_filePath);
    if (! m_file->open (QIODevice::WriteOnly))
        return false;

    m_dataSize = 0;
    writeHeader();
    QIODevice::open (mode);
    return true;
}

void WavFileWriter::close()
{
    if (m_file && m_file->isOpen())
    {
        patchHeader();
        m_file->close();
    }
    m_file.reset();
    QIODevice::close();
}

qint64 WavFileWriter::readData (char*, qint64)
{
    return -1;
}

qint64 WavFileWriter::writeData (const char* data, qint64 maxSize)
{
    if (! m_file || ! m_file->isOpen())
        return -1;

    auto written = m_file->write (data, maxSize);
    if (written > 0)
        m_dataSize += static_cast<quint32> (written);
    return written;
}

void WavFileWriter::writeHeader()
{
    QDataStream out (m_file.get());
    out.setByteOrder (QDataStream::LittleEndian);

    int byteRate = m_sampleRate * m_numChannels * m_bitsPerSample / 8;
    int blockAlign = m_numChannels * m_bitsPerSample / 8;

    // RIFF header
    out.writeRawData ("RIFF", 4);
    out << quint32 (0);  // placeholder for file size - 8
    out.writeRawData ("WAVE", 4);

    // fmt sub-chunk
    out.writeRawData ("fmt ", 4);
    out << quint32 (16);                             // sub-chunk size
    out << quint16 (1);                              // PCM format
    out << quint16 (static_cast<quint16> (m_numChannels));
    out << quint32 (static_cast<quint32> (m_sampleRate));
    out << quint32 (static_cast<quint32> (byteRate));
    out << quint16 (static_cast<quint16> (blockAlign));
    out << quint16 (static_cast<quint16> (m_bitsPerSample));

    // data sub-chunk
    out.writeRawData ("data", 4);
    out << quint32 (0);  // placeholder for data size
}

void WavFileWriter::patchHeader()
{
    m_file->seek (4);
    QDataStream out (m_file.get());
    out.setByteOrder (QDataStream::LittleEndian);
    out << quint32 (36 + m_dataSize);  // RIFF chunk size

    m_file->seek (40);
    QDataStream out2 (m_file.get());
    out2.setByteOrder (QDataStream::LittleEndian);
    out2 << quint32 (m_dataSize);      // data chunk size
}

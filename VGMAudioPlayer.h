#ifndef VGMAUDIOPLAYER_H
#define VGMAUDIOPLAYER_H

#include <QObject>
#include <QAudioSink>
#include <QAudioFormat>
#include <QIODevice>
#include <QTimer>
#include <memory>

extern "C" {
    #include "vgmstream.h"
}

class VGMIODevice;

class VGMAudioPlayer : public QObject
{
    Q_OBJECT

public:
    explicit VGMAudioPlayer(QObject *parent = nullptr);
    ~VGMAudioPlayer();

    bool loadFile(const QString& filePath);
    void play();
    void pause();
    void stop();
    void setVolume(float volume);
    void seek(qint64 positionMs);
    
    bool isPlaying() const;
    bool isPaused() const;
    
    qint64 duration() const;  // in milliseconds
    qint64 position() const;  // in milliseconds
    QString formatInfo() const;

signals:
    void positionChanged(qint64 position);
    void durationChanged(qint64 duration);
    void stateChanged();

private slots:
    void handleAudioStateChanged();

private:
    void setupAudio();
    void cleanup();
    
    VGMSTREAM* m_vgmstream;
    QAudioSink* m_audioSink;
    VGMIODevice* m_ioDevice;
    QTimer* m_timer;
    
    bool m_isLoaded;
    bool m_hasStarted;
    bool m_isStopping;
    QString m_filePath;
    QString m_formatInfo;
    
    // Loop information
    qint64 m_loopStart;  // in samples
    qint64 m_loopEnd;    // in samples
    bool m_hasLoop;
};

// Custom QIODevice for streaming vgmstream audio data
class VGMIODevice : public QIODevice
{
    Q_OBJECT

public:
    explicit VGMIODevice(VGMSTREAM* vgmstream, QObject* parent = nullptr);
    
    bool isSequential() const override;
    qint64 readData(char* data, qint64 maxlen) override;
    qint64 writeData(const char* data, qint64 len) override;
    
    bool reset() override;
    bool seek(qint64 pos) override;
    qint64 position() const;
    qint64 totalSamples() const;
    void seekToSample(qint64 sample);

private:
    VGMSTREAM* m_vgmstream;
    qint64 m_position;
    static const int BUFFER_SIZE = 1024;
    sample_t m_sampleBuffer[BUFFER_SIZE * 2]; // stereo buffer
};

#endif 
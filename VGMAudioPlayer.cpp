#include "VGMAudioPlayer.h"
#include <QDebug>
#include <QFileInfo>

VGMAudioPlayer::VGMAudioPlayer(QObject *parent)
    : QObject(parent)
    , m_vgmstream(nullptr)
    , m_audioSink(nullptr)
    , m_ioDevice(nullptr)
    , m_timer(new QTimer(this))
    , m_isLoaded(false)
    , m_hasStarted(false)
    , m_isStopping(false)
    , m_loopStart(0)
    , m_loopEnd(0)
    , m_hasLoop(false)
{
    m_timer->setInterval(50); // Update position every 100ms
    connect(m_timer, &QTimer::timeout, this, [this]() {
        // Don't do anything if we're in the process of stopping
        if (m_isStopping) return;
        
        if (m_hasLoop && m_ioDevice && m_vgmstream && isPlaying() && !m_isStopping) {
            qint64 currentSample = m_ioDevice->position();
            
            // Check if we're approaching or at the loop end
            // We check a bit before the actual end to ensure smooth transition
            qint64 threshold = m_vgmstream->sample_rate / 20; // 50ms before end
            
            if (currentSample >= (m_loopEnd - threshold) || currentSample >= m_vgmstream->num_samples - threshold) {
                // Double-check we're not stopping before doing the seek
                if (!m_isStopping) {
                    qDebug() << "Near loop end at sample" << currentSample << ", seeking to loop start" << m_loopStart;
                    
                    // Seamlessly seek back to loop start without stopping audio
                    m_ioDevice->seekToSample(m_loopStart);
                    
                    qDebug() << "Seamless loop completed";
                }
            }
        }
        
        // Only emit position updates if not stopping
        if (!m_isStopping) {
            emit positionChanged(position());
        }
    });
}

VGMAudioPlayer::~VGMAudioPlayer()
{
    cleanup();
}

bool VGMAudioPlayer::loadFile(const QString& filePath)
{
    cleanup();
    
    m_filePath = filePath;
    
    // Initialize vgmstream with the file
    m_vgmstream = init_vgmstream(filePath.toUtf8().constData());
    if (!m_vgmstream) {
        qDebug() << "Failed to load vgmstream file:" << filePath;
        return false;
    }
    
    // Prepare vgmstream for playback
    setup_vgmstream_play_state(m_vgmstream);
    
    // Generate format info string
    char description[1024];
    describe_vgmstream(m_vgmstream, description, sizeof(description));
    m_formatInfo = QString::fromUtf8(description);
    
    // Extract loop information
    if (m_vgmstream->loop_flag) {
        m_hasLoop = true;
        m_loopStart = m_vgmstream->loop_start_sample;
        m_loopEnd = m_vgmstream->loop_end_sample;
        
        qDebug() << "Loop detected:";
        qDebug() << "  Loop start:" << m_loopStart << "samples";
        qDebug() << "  Loop end:" << m_loopEnd << "samples";
        
        // Add loop info to format string
        qint64 loopStartMs = (m_loopStart * 1000) / m_vgmstream->sample_rate;
        qint64 loopEndMs = (m_loopEnd * 1000) / m_vgmstream->sample_rate;
        m_formatInfo += QString("\nLoop: %1ms - %2ms (seamless)")
                       .arg(loopStartMs)
                       .arg(loopEndMs);
    } else {
        m_hasLoop = false;
        m_loopStart = 0;
        m_loopEnd = 0;
    }
    
    m_isLoaded = true;
    m_hasStarted = false;
    m_isStopping = false;
    setupAudio();
    
    emit durationChanged(duration());
    return true;
}

void VGMAudioPlayer::setupAudio()
{
    if (!m_vgmstream) return;
    
    // Setup audio format
    QAudioFormat format;
    format.setSampleRate(m_vgmstream->sample_rate);
    format.setChannelCount(m_vgmstream->channels);
    format.setSampleFormat(QAudioFormat::Int16);
    
    // Create audio sink
    m_audioSink = new QAudioSink(format, this);
    m_audioSink->setVolume(0.7f);
    
    connect(m_audioSink, &QAudioSink::stateChanged,
            this, &VGMAudioPlayer::handleAudioStateChanged);
    
    // Create our custom IO device
    m_ioDevice = new VGMIODevice(m_vgmstream, this);
    m_ioDevice->open(QIODevice::ReadOnly);
}

void VGMAudioPlayer::play()
{
    if (!m_isLoaded || !m_audioSink || !m_ioDevice) return;
    
    // Clear stopping flag in case it was set
    m_isStopping = false;
    
    if (isPaused()) {
        // Resume from pause
        m_audioSink->resume();
    } else {
        // Start fresh or restart from current position
        m_audioSink->start(m_ioDevice);
        m_hasStarted = true;
    }
    
    m_timer->start();
    emit stateChanged();
}

void VGMAudioPlayer::pause()
{
    if (!m_audioSink) return;
    
    m_audioSink->suspend();
    m_timer->stop();
    emit stateChanged();
}

void VGMAudioPlayer::stop()
{
    if (!m_audioSink) return;
    
    // Set stopping flag to prevent timer callbacks
    m_isStopping = true;
    
    // Stop the timer first to prevent race conditions
    m_timer->stop();
    
    // Stop the audio sink
    m_audioSink->stop();
    
    // Reset state
    m_hasStarted = false;
    
    // Reset the IO device and vgmstream
    if (m_ioDevice) {
        m_ioDevice->reset();
    }
    
    if (m_vgmstream) {
        reset_vgmstream(m_vgmstream);
    }
    
    // Clear stopping flag
    m_isStopping = false;
    
    // Emit signals last to avoid callback loops
    emit positionChanged(0);
    emit stateChanged();
}

void VGMAudioPlayer::setVolume(float volume)
{
    if (m_audioSink) {
        m_audioSink->setVolume(qBound(0.0f, volume, 1.0f));
    }
}

void VGMAudioPlayer::seek(qint64 positionMs)
{
    if (!m_vgmstream || !m_ioDevice) return;
    
    // Convert milliseconds to sample position
    qint64 targetSample = (positionMs * m_vgmstream->sample_rate) / 1000;
    
    // Clamp to valid range
    targetSample = qBound(0LL, targetSample, (qint64)m_vgmstream->num_samples);
    
    // Seek in the IO device
    m_ioDevice->seekToSample(targetSample);
    
    emit positionChanged(positionMs);
}

bool VGMAudioPlayer::isPlaying() const
{
    return m_audioSink && m_audioSink->state() == QAudio::ActiveState;
}

bool VGMAudioPlayer::isPaused() const
{
    return m_audioSink && m_audioSink->state() == QAudio::SuspendedState;
}

qint64 VGMAudioPlayer::duration() const
{
    if (!m_vgmstream) return 0;
    
    qint64 sampleRate = m_vgmstream->sample_rate;
    
    // For looped songs, show duration up to loop end
    if (m_hasLoop) {
        return (m_loopEnd * 1000) / sampleRate;
    } else {
        // For non-looped songs, show full duration
        qint64 totalSamples = m_vgmstream->num_samples;
        return (totalSamples * 1000) / sampleRate;
    }
}

qint64 VGMAudioPlayer::position() const
{
    if (!m_vgmstream || !m_ioDevice) return 0;
    
    qint64 currentSample = m_ioDevice->position();
    qint64 sampleRate = m_vgmstream->sample_rate;
    
    // Clamp position to loop bounds for display purposes
    if (m_hasLoop && currentSample > m_loopEnd) {
        currentSample = m_loopEnd;
    }
    
    return (currentSample * 1000) / sampleRate; // Convert to milliseconds
}

QString VGMAudioPlayer::formatInfo() const
{
    return m_formatInfo;
}

void VGMAudioPlayer::handleAudioStateChanged()
{
    emit stateChanged();
}

void VGMAudioPlayer::cleanup()
{
    if (m_audioSink) {
        m_audioSink->stop();
        m_audioSink->deleteLater();
        m_audioSink = nullptr;
    }
    
    if (m_ioDevice) {
        m_ioDevice->close();
        m_ioDevice->deleteLater();
        m_ioDevice = nullptr;
    }
    
    if (m_vgmstream) {
        close_vgmstream(m_vgmstream);
        m_vgmstream = nullptr;
    }
    
    m_timer->stop();
    m_isLoaded = false;
    m_hasStarted = false;
    m_isStopping = false;
    m_hasLoop = false;
    m_loopStart = 0;
    m_loopEnd = 0;
}

// VGMIODevice implementation
VGMIODevice::VGMIODevice(VGMSTREAM* vgmstream, QObject* parent)
    : QIODevice(parent)
    , m_vgmstream(vgmstream)
    , m_position(0)
{
}

bool VGMIODevice::isSequential() const
{
    return true;
}

qint64 VGMIODevice::readData(char* data, qint64 maxlen)
{
    if (!m_vgmstream || maxlen <= 0) return 0;
    
    // Calculate how many samples we can read
    int channels = m_vgmstream->channels;
    int bytesPerSample = sizeof(sample_t);
    int samplesRequested = maxlen / (channels * bytesPerSample);
    
    // Limit to our buffer size
    samplesRequested = qMin(samplesRequested, BUFFER_SIZE);
    
    if (samplesRequested <= 0) return 0;
    
    // Check if we've reached the end
    if (m_position >= m_vgmstream->num_samples) {
        return 0; // EOF
    }
    
    // Make sure we don't read past the end
    qint64 samplesLeft = m_vgmstream->num_samples - m_position;
    samplesRequested = qMin((qint64)samplesRequested, samplesLeft);
    
    // Render audio samples
    int samplesRendered = render_vgmstream2(m_sampleBuffer, samplesRequested, m_vgmstream);
    
    if (samplesRendered <= 0) return 0;
    
    // Copy to output buffer
    int bytesToCopy = samplesRendered * channels * bytesPerSample;
    memcpy(data, m_sampleBuffer, bytesToCopy);
    
    m_position += samplesRendered;
    
    return bytesToCopy;
}

qint64 VGMIODevice::writeData(const char* data, qint64 len)
{
    Q_UNUSED(data)
    Q_UNUSED(len)
    return -1; // We're read-only
}

bool VGMIODevice::reset()
{
    m_position = 0;
    if (m_vgmstream) {
        reset_vgmstream(m_vgmstream);
    }
    return true;
}

qint64 VGMIODevice::position() const
{
    return m_position;
}

qint64 VGMIODevice::totalSamples() const
{
    return m_vgmstream ? m_vgmstream->num_samples : 0;
}

bool VGMIODevice::seek(qint64 pos)
{
    // QIODevice seek - pos is in bytes
    // For seeking by samples, use seekToSample instead
    return false; // We don't support byte-level seeking
}

void VGMIODevice::seekToSample(qint64 sample)
{
    if (!m_vgmstream) return;
    
    // Clamp to valid range
    sample = qBound(0LL, sample, (qint64)m_vgmstream->num_samples);
    
    // Use vgmstream's seek function
    seek_vgmstream(m_vgmstream, sample);
    
    // Update our position
    m_position = sample;
}

 
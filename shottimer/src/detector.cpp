#include "detector.h"
#include <QMediaDevices>
#include <QAudioDevice>
#include <QAudioInput>
#include <QtMath>
#include <QDebug>

Detector::Detector(QObject *parent)
    : QIODevice{parent}
{
    open(QIODevice::WriteOnly);

    // Default audio format in mono
    QAudioDevice defaultDevice = QMediaDevices::defaultAudioInput();
    format = defaultDevice.preferredFormat();
    format.setChannelCount(1);
    if (defaultDevice.isFormatSupported(format)) {
        format = defaultDevice.preferredFormat();
    }

    // Since timer display on significant to 10 msec, this should be fine
    rollingTimer.setInterval(10);
    connect(&rollingTimer, &QTimer::timeout, this, &Detector::updateRollingWindow);

    // Setup audio input device
    audioSource = new QAudioSource(format, this);
    // handle state change?
}

Detector::~Detector() {
    stop();
    close();
}

int Detector::threshold() const
{
    return qRound(shotThreshold * 100 / 2);
}

void Detector::setThreshold(int newThreshold)
{
    shotThreshold = (float) newThreshold * 2 / 100;
}

void Detector::start()
{
    maxAmplitude = 0;
    minAmplitude = 0;
    waitForReset = false;
    rollingTimer.start();
    audioSource->start(this);
}

void Detector::stop()
{
    rollingTimer.stop();
    audioSource->stop();
}

void Detector::updateRollingWindow()
{
    // FIXME: a better detection algorithm would be checking for large transient
    // this way also don't have to deal with reset

    // Check if reset is complete
    if (waitForReset) {
        if (minAmplitude > resetThreshold) {
            qDebug() << "waiting for reset " << minAmplitude << " < " << resetThreshold;
            return;
        }
        else
            waitForReset = false;
    }

    float level = maxAmplitude - minAmplitude;
    qDebug() << "level " << maxAmplitude << " - " << minAmplitude << " = " << level;
    if (level >= shotThreshold) {
        qDebug() << " > " << shotThreshold;
        waitForReset = true;
        resetThreshold = (maxAmplitude + minAmplitude) / 2;
        maxAmplitude = 0;
        minAmplitude = 0;
        emit shotDetected();
    }
}

qint64 Detector::readData(char *data, qint64 maxlen)
{
    return 0;
}

qint64 Detector::writeData(const char *data, qint64 len)
{
    // max and min for this sample write
    float max, min;

    const int sampleSize = format.bytesPerSample();
    const int channelCount = format.channelCount();
    if (sampleSize == 0 || channelCount == 0) {
        qWarning() << "invlaid audio format!";
        return 0;
    }
    // each sample contains {bytesPerSamle} for a sample for each channel
    const int numSamples = len / sampleSize / channelCount;

    // find local max and min
    for (int i = 0; i < numSamples; i++) {
        for (int j = 0; j < channelCount; j++) {
            float level = format.normalizedSampleValue(data);
            if (abs(level) > 1)
                qDebug() << "abnormal level " << level;
            if (level > max)
                max = level;
            else if (level < min)
                min = level;
            data += sampleSize;
        }
    }

    // update rolling sample window max and min
    if (max > maxAmplitude)
        maxAmplitude = max;
    else if (min < minAmplitude)
        minAmplitude = min;

    return len;
}



#ifndef DETECTOR_H
#define DETECTOR_H

#include <QIODevice>
#include <QAudioSource>
#include <QAudioFormat>
#include <QTimer>
#include <QQmlEngine>

/**
 * @brief Shot detector
 * @remark Using QAudioSource inside QIODevice is not conventional paradigm
 *         where they should be two classes. Inheritance of QIODevice is only
 *         used to access implementation. Should probably change this.
 */
class Detector : public QIODevice
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(float threshold READ threshold WRITE setThreshold NOTIFY thresholdChanged)

public:
    explicit Detector(QObject *parent = nullptr);
    ~Detector() override;

    // Normalized threshold between 1 and 100
    int threshold() const;
    void setThreshold(int newThreshold);

public slots:
    void start();
    void stop();

signals:
    void shotDetected();
    void thresholdChanged();

private:
    // Prefered format
    QAudioFormat format;
    // Current max amplitude in this rolling sample window, normalized
    float maxAmplitude;
    // Current min apmlitude in this rolling sample window, normalized
    float minAmplitude;
    // Threshold level for shot detection
    float shotThreshold = 1;

    // Prevent counting a single shot multple times
    bool waitForReset = false;
    // Reset threshold
    float resetThreshold;

    // Rolling sample window update timer
    // Not so much as "rolling" since sample windows don't overlap
    QTimer rollingTimer;

    // Audio input device
    QAudioSource *audioSource;

private slots:
    // Check for shot detection in rolling sample window
    void updateRollingWindow();

protected:
    qint64 readData(char *data, qint64 maxlen) override;
    qint64 writeData(const char *data, qint64 len) override;
};

#endif // DETECTOR_H

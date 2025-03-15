#ifndef TIMER_H
#define TIMER_H

#include <QObject>
#include <QQmlEngine>
#include <QVector>
#include <QElapsedTimer>
#include <QTimer>
#include <detector.h>

/**
 * @brief Main logic for the ShotTimer app
 * @todo make this singleton
 */
class ShotTimer : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    // Current elapsed time
    Q_PROPERTY(qint64 currentTime READ currentTime NOTIFY currentTimeChanged)
    Q_PROPERTY(qint64 latestShotTime READ latestShotTime NOTIFY shotTimeChanged)
    Q_PROPERTY(qint64 lastShotTime READ lastShotTime NOTIFY shotTimeChanged)
    Q_PROPERTY(Detector* detector READ shotdetector CONSTANT)

public:
    explicit ShotTimer(QObject *parent = nullptr);

    // Current elapsed time
    qint64 currentTime() const;

    // Latest shot time in seconds
    qint64 latestShotTime() const;

    // Last shot time in seconds
    qint64 lastShotTime() const;

    // Shot detector
    Detector *shotdetector();

public slots:
    // Start monitoring for shots and calculate times
    void startMonitoring(qint64 delay = 0);
    void stopMonitoring();

signals:
    void currentTimeChanged();
    void shotTimeChanged();
    void thresholdChanged();

private:
    // For shot times
    QElapsedTimer elapsedTimer;
    // Update currentTime at fixed interval
    QTimer updateTimer;
    // Shot detector
    Detector detector;
    // Shot times history
    QVector<qint64> shottimes;

    // Record a shot
    void recordShot();
};

#endif // TIMER_H

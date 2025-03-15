#include "timer.h"
#include <QObject>
#include <QTimer>
#include <QDebug>

ShotTimer::ShotTimer(QObject *parent)
    : QObject{parent}
{
    updateTimer.setInterval(10);
    connect(&updateTimer, &QTimer::timeout, this, [&](){
        emit currentTimeChanged();
    });
    connect(&detector, &Detector::shotDetected, this, &ShotTimer::recordShot);
}

void ShotTimer::startMonitoring(qint64 delay) {
    if (delay > 0) {
        // return QTimer::singleShot(delay, &ShotTimer::startMonitoring);
        QTimer timer;
        timer.setInterval(delay);
        timer.setSingleShot(true);
        timer.start();
        while(timer.remainingTime() > 0);
    }

    shottimes.clear();
    elapsedTimer.start();
    updateTimer.start();

    // start audio monitoring
    qDebug() << "monitoring shots";
    detector.start();

    // test shot triggering
    // QTimer *timer = new QTimer(this);
    // timer->setInterval(2000);
    // connect(timer, &QTimer::timeout, this, [&]() {
    //     qDebug() << "shots detected!";
    //     recordShot();
    // });
    // timer->start();
}

void ShotTimer::stopMonitoring() {
    qDebug() << "stopped";
    elapsedTimer.invalidate();
    updateTimer.stop();
    detector.stop();
    emit currentTimeChanged();
}

void ShotTimer::recordShot() {
    if (!elapsedTimer.isValid())
        return;
    qDebug() << "Shots recorded!";
    shottimes.append(elapsedTimer.elapsed());
    emit shotTimeChanged();
}

qint64 ShotTimer::currentTime() const {
    return elapsedTimer.isValid() ? elapsedTimer.elapsed() : 0;
}

qint64 ShotTimer::latestShotTime() const {
    if (shottimes.empty())
        return 0;
    return shottimes.last();
}

qint64 ShotTimer::lastShotTime() const {
    if (shottimes.empty())
        return 0;
    return *(shottimes.end() - 2);
}

Detector *ShotTimer::shotdetector()
{
    return &detector;
}

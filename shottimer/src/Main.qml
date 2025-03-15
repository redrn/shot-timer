import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import shottimer

Window {
    id: window
    width: 720
    height: 1280
    visible: true
    title: qsTr("Shot Timer")

    // Main timer backend
    ShotTimer {
        id: timer
    }

    // Main layout
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 50
        spacing: 5

        // Time displays
        ColumnLayout {
            // anchors.top: parent.top
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignTop
            spacing: 10

            RowLayout {
                // anchors.fill: parent
                // anchors.horizontalCenter: latestDisplay.horizontalCenter
                // anchors.bottom: latestDisplay.top
                // anchors.bottomMargin: 10
                Layout.fillWidth: true

                // Previous shot time display
                Label {
                    Layout.alignment: Qt.AlignLeft
                    Layout.fillWidth: true
                    text: msecToSec(timer.lastShotTime)
                }
                // Timer display
                Label {
                    text: msecToSec(timer.currentTime)
                    horizontalAlignment: Text.AlignRight
                }
            }

            // Latest shot time display
            Label {
                id: latestDisplay
                Layout.alignment: Qt.AlignCenter
                Layout.fillWidth: true

                text: msecToSec(timer.latestShotTime)
                fontSizeMode: Text.Fit
            }
        }


        ColumnLayout {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignBottom
            spacing: 10

            // Threshold slider
            ColumnLayout {
                Layout.fillWidth: true
                RowLayout {
                    Layout.fillWidth: true
                    Label {
                        Layout.fillWidth: true
                        text: qsTr("Detection Threshold")
                    }
                    Label {
                        text: thresholdSlider.value
                    }
                }
                Slider {
                    id: thresholdSlider
                    Layout.fillWidth: true
                    from: 1
                    stepSize: 1
                    to: 100
                    value: timer.detector.threshold
                    onMoved: {
                        timer.detector.threshold = value
                    }

                    // Binding {
                    //     target: timer.detector
                    //     property: "threshold"
                    //     value: value
                    // }
                }
            }


            // Main starter button
            RoundButton {
                // anchors.bottom: parent.bottom
                Layout.alignment: Qt.AlignBottom
                Layout.fillWidth: true
                // implicitWidth: 40
                implicitHeight: 100
                radius: 8
                background: Rectangle {
                    color: "red"
                    radius: 8
                }

                text: "Start"
                highlighted: true

                property bool stopped: true

                onClicked: {
                    toggleState()
                }

                function toggleState() {
                    stopped = !stopped
                    text = stopped ? "Start" : "Stop"
                    if (stopped) {
                        timer.stopMonitoring()
                    } else {
                        timer.startMonitoring()
                    }
                }
            }
        }

    }

    // Format to seconds two decimal point
    function msecToSec(msec) {
        return (msec / 1000).toFixed(2)
    }
}

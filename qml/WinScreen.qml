import QtQuick 2.0
import 'qrc:/qml/Constants.js' as Constants

Item {
    id: winScreen

    signal continueGameRequested
    signal restartGameRequested

    Rectangle {
        id: background

        anchors.fill: parent
        radius: Math.min(width, height) * Constants.gameboardRadiusRatio
        color: '#edc22e'
        opacity: 0.0
    }

    Text {
        id: text

        x: winScreen.width / 2 - text.paintedWidth / 2
        font.family: Constants.fontFamily
        font.pixelSize: 60
        font.weight: Font.Bold
        color: '#776e65'
        opacity: 0.0
        text: qsTr('You win!')
    }

    Row {
        id: buttons

        anchors.top: text.bottom
        anchors.topMargin: 10
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 5
        opacity: 0.0

        Button {
            id: keepGoingButton

            text: qsTr('Keep going')
            onClicked: {
                if (0.0 < opacity) {
                    continueGameRequested()
                }
            }
        }

        Button {
            id: tryAgainButton

            text: qsTr('Try again')
            onClicked: {
                if (0 < opacity) {
                    restartGameRequested()
                }
            }
        }
    }

    SequentialAnimation {
        id: animation

        PauseAnimation {
            duration: 500
        }

        NumberAnimation {
            target: background
            properties: 'opacity'
            from: 0.0
            to: 0.8
            duration: 1000
        }

        PauseAnimation {
            duration: 500
        }

        ParallelAnimation {
            PropertyAnimation {
                target: text
                property: 'y'
                from: winScreen.height * 0.8
                to: winScreen.height * 0.32
                duration: 1500
                easing.type: Easing.OutBack
            }

            NumberAnimation {
                target: text
                properties: 'opacity'
                from: 0.0
                to: 1.0
                duration: 500
            }
        }

        PauseAnimation {
            duration: 500
        }

        NumberAnimation {
            target: buttons
            properties: 'opacity'
            from: 0.0
            to: 1.0
            duration: 1000
        }
    }

    Component.onCompleted: animation.start()
}

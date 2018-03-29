import QtQuick 2.0
import 'qrc:/qml/Constants.js' as Constants

Item {
    id: defeatScreen

    signal restartGameRequested

    Rectangle {
        id: background

        anchors.fill: parent
        radius: Math.min(width, height) * Constants.gameboardRadiusRatio
        color: '#eee4da'
        opacity: 0.0
    }

    Text {
        id: text

        x: defeatScreen.width / 2 - text.paintedWidth / 2
        font.family: Constants.fontFamily
        font.pixelSize: 60
        font.weight: Font.Bold
        color: '#776e65'
        opacity: 0.0
        text: qsTr('Game over!')
    }

    Button {
        id: tryAgainButton

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: text.bottom
        anchors.topMargin: 10
        opacity: 0.0
        text: qsTr('Try again')
        onClicked: {
            if (0.0 < opacity) {
                restartGameRequested()
            }
        }
    }

    SequentialAnimation {
        id: animation

        PauseAnimation {
            duration: 1000
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
                from: defeatScreen.height * 0.8
                to: defeatScreen.height * 0.32
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
            target: tryAgainButton
            properties: 'opacity'
            from: 0.0
            to: 1.0
            duration: 1000
        }
    }

    Component.onCompleted: animation.start()
}

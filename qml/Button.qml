import QtQuick 2.0
import 'qrc:/qml/Constants.js' as Constants

Rectangle {
    id: button

    property string text

    signal clicked

    width: Math.max(130, buttonText.paintedWidth + 20)
    height: 40
    radius: 3
    color: '#8f7a66'

    SequentialAnimation {
        id: bounceAnimation

        loops: 1
        PropertyAnimation {
            target: scaleTransform
            properties: 'scale'
            from: 1.0
            to: 1.1
            duration: 50
        }

        PropertyAnimation {
            target: scaleTransform
            properties: 'scale'
            from: 1.1
            to: 1.0
            duration: 50
        }
    }

    transform: [
        Scale {
            id: scaleTransform

            origin.x: button.width / 2
            origin.y: button.height / 2
            property real scale: 1
            xScale: scale
            yScale: scale
        }
    ]

    MouseArea {
        anchors.fill: parent
        onClicked: {
            bounceAnimation.start()
            button.clicked()
        }
    }

    Text {
        id: buttonText

        anchors.centerIn: parent
        anchors.verticalCenterOffset: font.pixelSize * Constants.buttonTextVerticalOffsetRatio
        font.family: Constants.fontFamily
        font.pixelSize: 18
        font.weight: Font.Bold
        color: '#ffffff'
        text: button.text
    }
}


import QtQuick 2.0
import 'qrc:/qml/Constants.js' as Constants

Rectangle {
    id: scoreLabel

    property string text: 'Score'
    property int value: 0
    property int previousValue: 0
    property bool valueAnimation: false

    height: 55
    width: 60
    radius: 3
    color: '#bbada0'

    Behavior on width {
        NumberAnimation { duration: 50; easing.type: Easing.InOutQuad }
    }

    Text {
        id: scoreTitle

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 5
        font.family: Constants.fontFamily
        font.pixelSize: 13
        font.weight: Font.Bold
        color: '#eee4da'
        text: parent.text
    }

    Text {
        id: scoreText

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: scoreTitle.bottom
        anchors.topMargin: -5
        font.family: Constants.fontFamily
        font.pixelSize: Constants.scoreTextPixelSize
        font.weight: Font.Bold
        color: '#ffffff'
        text: parent.value

        onTextChanged: {
            if (paintedWidth > parent.width - 20) {
                parent.width = paintedWidth + 20
            }
        }
    }

    Text {
        id: animatedText

        anchors.right: scoreText.right
        anchors.verticalCenter: scoreText.verticalCenter
        font.family: Constants.fontFamily
        font.pixelSize: Constants.scoreTextPixelSize
        font.weight: Font.Bold
        color: '#776e65'
        opacity: 0.0
    }

    ParallelAnimation {
        id: animation

        PropertyAnimation {
            target: animatedText
            property: 'anchors.verticalCenterOffset'
            from: 0
            to: -scoreLabel.height
            duration: 800
            easing.type: Easing.InOutQuad
        }

        NumberAnimation {
            target: animatedText;
            properties: 'opacity'
            from: 1.0
            to: 0.0
            duration: 800
            easing.type: Easing.InOutQuad
        }
    }

    onValueChanged: {
        var diff = value - previousValue
        previousValue = value
        if (valueAnimation && 0 < diff) {
            animatedText.text = '+' + diff
            animation.restart()
        }
    }
}

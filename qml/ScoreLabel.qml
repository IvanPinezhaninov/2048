/***************************************************************************
**
** Copyright (C) 2018 Ivan Pinezhaninov <ivan.pinezhaninov@gmail.com>
**
** This file is part of the 2048 Game.
**
** The 2048 Game is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** The 2048 Game is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with the 2048 Game.  If not, see <http://www.gnu.org/licenses/>.
**
***************************************************************************/


import QtQuick 2.0
import 'qrc:/qml/Constants.js' as Constants

Rectangle {
    id: scoreLabel

    property string text: 'Score'
    property int value: 0
    property int previousValue: 0
    property bool valueAnimationEnabled: false
    property bool widthAnimationEnabled: true
    readonly property var locale: Qt.locale()

    height: 55
    width: 60
    radius: 3
    color: '#bbada0'

    Behavior on width {
        enabled: widthAnimationEnabled
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
        text: Number(parent.value).toLocaleString(locale, 'f', 0)

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
        if (valueAnimationEnabled && 0 < diff) {
            animatedText.text = '+' + Number(diff).toLocaleString(locale, 'f', 0)
            animation.restart()
        }
    }
}

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


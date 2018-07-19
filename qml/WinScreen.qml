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

Item {
    id: winScreen

    signal continueGameRequested
    signal startNewGameRequested

    Rectangle {
        id: background

        anchors.fill: parent
        radius: Math.round(Math.min(width, height) * Constants.gameboardRadiusRatio)
        color: '#edc22e'
        opacity: 0.0
    }

    Text {
        id: text

        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
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
                    startNewGameRequested()
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
                property: 'anchors.verticalCenterOffset'
                from: Math.round(parent.height * 0.5 + text.font.pixelSize * 0.5)
                to: Math.round(text.font.pixelSize * -0.65)
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

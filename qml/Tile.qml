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
    id: tile

    property int value: 0
    property int moveAnimationDuration: 100

    readonly property real largeFontRatio: 0.52
    readonly property real mediumFontRatio: 0.42
    readonly property real smallFontRatio: 0.32
    readonly property real xSmallFontRatio: 0.28
    readonly property real xxSmallFontRatio: 0.24
    readonly property real xxxSmallFontRatio: 0.22
    readonly property real xxxxSmallFontRatio: 0.18

    signal moveFinished

    color: '#eee4da'
    radius: Math.min(width, height) * Constants.tileRadiusRatio

    Text {
        id: tileText

        anchors.centerIn: parent
        anchors.verticalCenterOffset: font.pixelSize * -0.04
        font.family: Constants.fontFamily
        font.weight: Font.Bold
        font.pixelSize: Math.min(parent.width, parent.height) * largeFontRatio
        color: '#776e65'
        text: tile.value
    }

    Behavior on x {
        enabled: tile.value !== 0
        NumberAnimation {
            duration: moveAnimationDuration
            easing.type: Easing.InOutQuad
            onRunningChanged: onMoveAnimationRunningChanged(running)
        }
    }

    Behavior on y {
        enabled: tile.value !== 0
        NumberAnimation {
            duration: moveAnimationDuration
            easing.type: Easing.InOutQuad
            onRunningChanged: onMoveAnimationRunningChanged(running)
        }
    }

    PropertyAnimation {
        id: scaleAnimation

        target: scaleTransform
        property: 'scale'
        from: 0.0
        to: 1.0
        duration: 200
    }

    SequentialAnimation {
        id: bounceAnimation

        PropertyAnimation {
            target: scaleTransform
            property: 'scale'
            from: 1.0
            to: 1.2
            duration: 100
        }

        PropertyAnimation {
            target: scaleTransform
            property: 'scale'
            from: 1.2
            to: 1.0
            duration: 100
        }
    }

    transform: [
        Scale {
            id: scaleTransform

            property real scale: 1
            origin.x: tile.width / 2
            origin.y: tile.height / 2
            xScale: scale
            yScale: scale
        }
    ]

    states: [
        State {
            name: 'orphaned'
            when: tile.value === 0
            PropertyChanges { target: tile; visible: false }
        },

        State {
            name: '2'
            when: tile.value === 2
        },

        State {
            name: '4'
            when: tile.value === 4
            PropertyChanges { target: tile; color: '#ede0c8' }
            PropertyChanges { target: tileText;  color: '#776e65' }
        },

        State {
            name: '8'
            when: tile.value === 8
            PropertyChanges { target: tile; color: '#f2b179' }
            PropertyChanges { target: tileText;  color: '#f9f6f2' }
        },

        State {
            name: '16'
            when: tile.value === 16
            PropertyChanges { target: tile; color: '#f59563' }
            PropertyChanges { target: tileText;  color: '#f9f6f2' }
        },

        State {
            name: '32'
            when: tile.value === 32
            PropertyChanges { target: tile; color: '#f67c5f' }
            PropertyChanges { target: tileText;  color: '#f9f6f2' }
        },

        State {
            name: '64'
            when: tile.value === 64
            PropertyChanges { target: tile; color: '#f65e3b' }
            PropertyChanges { target: tileText;  color: '#f9f6f2' }
        },

        State {
            name: '128'
            when: tile.value === 128
            PropertyChanges { target: tile; color: '#edcf72' }
            PropertyChanges { target: tileText;  font.pixelSize: tile.width * mediumFontRatio }
            PropertyChanges { target: tileText;  color: '#f9f6f2' }
        },

        State {
            name: '256'
            when: tile.value === 256
            PropertyChanges { target: tile; color: '#edcc61' }
            PropertyChanges { target: tileText;  font.pixelSize: tile.width * mediumFontRatio }
            PropertyChanges { target: tileText;  color: '#f9f6f2' }
        },

        State {
            name: '512'
            when: tile.value === 512
            PropertyChanges { target: tile; color: '#edc850' }
            PropertyChanges { target: tileText;  font.pixelSize: tile.width * mediumFontRatio }
            PropertyChanges { target: tileText;  color: '#f9f6f2' }
        },

        State {
            name: '1024'
            when: tile.value === 1024
            PropertyChanges { target: tile; color: '#edc53f' }
            PropertyChanges { target: tileText;  font.pixelSize: tile.width * smallFontRatio }
            PropertyChanges { target: tileText;  color: '#f9f6f2' }
        },

        State {
            name: '2048'
            when: tile.value === 2048
            PropertyChanges { target: tile; color: '#edc22e' }
            PropertyChanges { target: tileText;  font.pixelSize: tile.width * smallFontRatio }
            PropertyChanges { target: tileText;  color: '#f9f6f2' }
        },

        State {
            name: '4096+'
            when: tile.value >= 4096 && tile.value < 131072
            PropertyChanges { target: tile; color: '#3c3a32' }
            PropertyChanges { target: tileText;  font.pixelSize: tile.width * xSmallFontRatio }
            PropertyChanges { target: tileText;  color: '#f9f6f2' }
        },

        State {
            name: '131072+'
            when: tile.value >= 131072 && tile.value < 1048576
            extend: '4096+'
            PropertyChanges { target: tileText;  font.pixelSize: tile.width * xxSmallFontRatio }
        },

        State {
            name: '1048576+'
            when: tile.value >= 1048576 && tile.value < 16777216
            extend: '4096+'
            PropertyChanges { target: tileText;  font.pixelSize: tile.width * xxxSmallFontRatio }
        },

        State {
            name: '16777216+'
            when: tile.value >= 16777216
            extend: '4096+'
            PropertyChanges { target: tileText;  font.pixelSize: tile.width * xxxxSmallFontRatio }
        }
    ]

    transitions: [
        Transition {
            from: 'orphaned'
            to: '*'
            PropertyAction { target: scaleAnimation; property: 'running'; value: true }
            PropertyAction { target: bounceAnimation; property: 'running'; value: false }
        }
    ]

    onValueChanged: bounceAnimation.running = true

    function onMoveAnimationRunningChanged(running)
    {
        if (!running) {
            moveFinished()
        }
    }
}

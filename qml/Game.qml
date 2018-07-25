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
    id: game

    property alias score: scoreLabel.value
    property alias bestScore: bestScoreLabel.value

    signal continueGameRequested
    signal startNewGameRequested

    objectName: 'Game'
    state: 'init'

    states: [
        State {
            name: 'init'
            PropertyChanges { target: scoreLabel; valueAnimationEnabled: false; widthAnimationEnabled: false }
            PropertyChanges { target: bestScoreLabel; widthAnimationEnabled: false }
        },

        State {
            name: 'play'
        },

        State {
            name: 'defeat'
            PropertyChanges { target: loader; source: 'DefeatScreen.qml' }
        },

        State {
            name: 'win'
            PropertyChanges { target: loader; source: 'WinScreen.qml' }
        },

        State {
            name: 'continue'
        }
    ]

    FontLoader { source: 'qrc:/fonts/ClearSans-Regular.ttf' }
    FontLoader { source: 'qrc:/fonts/ClearSans-Bold.ttf' }

    Text {
        id: titleText

        anchors.left: gameboard.left
        anchors.verticalCenter: bestScoreLabel.verticalCenter
        anchors.verticalCenterOffset: Math.round(font.pixelSize * -0.06)
        font.family: Constants.fontFamily
        font.pixelSize: 78
        font.weight: Font.Bold
        color: '#776e65'
        text: '2048'
    }

    ScoreLabel {
        id: scoreLabel

        anchors.right: bestScoreLabel.left
        anchors.rightMargin: 5
        anchors.verticalCenter: bestScoreLabel.verticalCenter
        width: 90
        text: qsTr('SCORE')
        valueAnimationEnabled: true
    }

    ScoreLabel {
        id: bestScoreLabel

        anchors.right: gameboard.right
        anchors.top: parent.top
        width: 90
        text: qsTr('BEST')
    }

    Text {
        id: mottoText

        anchors.left: gameboard.left
        anchors.verticalCenter: newGameButton.verticalCenter
        anchors.verticalCenterOffset: Math.round(font.pixelSize * Constants.textVerticalOffsetRatio)
        visible: 0.0 !== opacity
        font.family: Constants.fontFamily
        font.pixelSize: 18
        color: '#776e65'
        text: qsTr('Join the numbers and get to the <b>2048 tile!</b>')
        Behavior on opacity { NumberAnimation { duration: 200 } }
    }

    Button {
        id: undoButton

        objectName: 'UndoButton'
        anchors.left: gameboard.left
        anchors.verticalCenter: newGameButton.verticalCenter
        visible: 0.0 !== opacity
        opacity: 0.0
        width: 80
        text: qsTr('Undo')
        onClicked: console.log('Coming soon!')
        Behavior on opacity { NumberAnimation { duration: 200 } }
    }

    Button {
        id: newGameButton

        anchors.right: gameboard.right
        anchors.top: scoreLabel.bottom
        anchors.topMargin: 20
        text: qsTr('New Game')
        onClicked: startNewGameRequested()
    }

    Item {
        id: gameboardContainer

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: newGameButton.bottom
        anchors.bottom: parent.bottom
        anchors.topMargin: 20
    }

    Gameboard {
        id: gameboard

        anchors.top: gameboardContainer.top
        anchors.horizontalCenter: gameboardContainer.horizontalCenter
        width: Math.min(gameboardContainer.width, gameboardContainer.height)
        height: width
        focus: true
    }

    Text {
        id: rulesText

        anchors.left: gameboard.left
        anchors.right: gameboard.right
        anchors.top: gameboard.bottom
        anchors.topMargin: 20

        font.family: Constants.fontFamily
        font.pixelSize: 18
        color: '#776e65'
        wrapMode: Text.WordWrap

        text: qsTr('<b>HOW TO PLAY:</b> Use your <b>arrow keys</b> to move the tiles. '
                   + 'When two tiles with the same number touch, they <b>merge into one!</b>')
    }

    Loader {
        id: loader

        anchors.fill: gameboard
        asynchronous: true

        onLoaded: {
            switch (game.state) {
            case 'win':
                loader.item.continueGameRequested.connect(continueGameRequested)
                loader.item.startNewGameRequested.connect(startNewGameRequested)
                break
            case 'defeat':
                loader.item.startNewGameRequested.connect(startNewGameRequested)
                break
            }
        }
    }
}

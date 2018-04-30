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
    state: 'play'

    states: [
        State {
            name: 'play'
        },

        State {
            name: 'defeat'
            PropertyChanges { target: loader; source: 'DefeatScreen.qml' }
            StateChangeScript {
                script: {
                    loader.item.startNewGameRequested.connect(startNewGameRequested)
                }
            }
        },

        State {
            name: 'win'
            PropertyChanges { target: loader; source: 'WinScreen.qml' }
            StateChangeScript {
                script: {
                    loader.item.continueGameRequested.connect(continueGameRequested)
                    loader.item.startNewGameRequested.connect(startNewGameRequested)
                }
            }
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
        anchors.verticalCenterOffset: font.pixelSize * -0.06
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
        valueAnimation: true
        text: qsTr('SCORE')
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
        anchors.verticalCenterOffset: font.pixelSize * Constants.buttonTextVerticalOffsetRatio
        font.family: Constants.fontFamily
        font.pixelSize: 18
        color: '#776e65'
        text: qsTr('Join the numbers and get to the <b>2048 tile!</b>')
    }

    Button {
        id: newGameButton

        anchors.top: titleText.bottom
        anchors.right: gameboard.right
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

        anchors.centerIn: gameboardContainer
        width: Math.min(gameboardContainer.width, gameboardContainer.height)
        height: width
        focus: true
    }

    Loader {
        id: loader
        anchors.fill: gameboard
    }
}

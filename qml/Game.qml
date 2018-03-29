import QtQuick 2.0
import 'qrc:/qml/Constants.js' as Constants

Item {
    id: game

    property alias score: scoreLabel.value
    property alias bestScore: bestScoreLabel.value

    signal continueGameRequested
    signal restartGameRequested

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
                    loader.item.restartGameRequested.connect(restartGameRequested)
                }
            }
        },

        State {
            name: 'win'
            PropertyChanges { target: loader; source: 'WinScreen.qml' }
            StateChangeScript {
                script: {
                    loader.item.continueGameRequested.connect(continueGameRequested)
                    loader.item.restartGameRequested.connect(restartGameRequested)
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
        onClicked: restartGameRequested()
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

    function addScore(value) {
        score += value
        if (bestScore < score) {
            bestScore = score
        }
    }
}

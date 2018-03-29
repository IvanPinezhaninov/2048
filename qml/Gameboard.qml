import QtQuick 2.0
import 'qrc:/qml/Constants.js' as Constants

Rectangle {
    id: gameboard

    property int rows: 4
    property int columns: 4
    readonly property alias cellsCount: cellsRepeater.count

    objectName: 'Gameboard'
    radius: Math.min(width, height) * Constants.gameboardRadiusRatio
    color: '#bbada0'

    Component {
        id: cell

        Rectangle {
            width: (gameboard.width - (cellsGrid.spacing * (gameboard.columns + 1))) / gameboard.columns
            height: (gameboard.height - (cellsGrid.spacing * (gameboard.rows + 1))) / gameboard.rows
            radius: Math.min(width, height) * Constants.tileRadiusRatio
            color: '#ccc0b3'

            Text {
                id: cellIndex

                anchors.centerIn: parent
                anchors.verticalCenterOffset: parent.height * Constants.textVerticalOffsetRatio
                font.family: Constants.fontFamily
                font.weight: Font.Bold
                font.pixelSize: Math.min(parent.width, parent.height) * 0.32
                color: '#bbada0'
                text: index
            }
        }
    }

    Grid {
        id: cellsGrid

        objectName: 'CellsGrid'
        anchors.fill: parent
        anchors.margins: spacing
        columns: parent.columns
        rows: parent.rows
        spacing: Math.min(parent.width, parent.height) / Math.min(rows, columns) * 0.12

        Repeater {
            id: cellsRepeater

            model: parent.columns * parent.rows
            delegate: cell
        }
    }

    function getCell(index) {
        return cellsRepeater.itemAt(index)
    }
}

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
    id: gameboard

    property int rows: 0
    property int columns: 0
    property bool debug: false

    objectName: 'Gameboard'
    radius: Math.round(Math.min(width, height) * Constants.gameboardRadiusRatio)
    color: '#bbada0'

    Component {
        id: cell

        Rectangle {
            width: (gameboard.width - (cellsGrid.spacing * (gameboard.columns + 1))) / gameboard.columns
            height: (gameboard.height - (cellsGrid.spacing * (gameboard.rows + 1))) / gameboard.rows
            radius: Math.round(Math.min(width, height) * Constants.tileRadiusRatio)
            color: '#ccc0b3'

            Text {
                id: cellIndex

                anchors.centerIn: parent
                anchors.verticalCenterOffset: Math.round(font.pixelSize * Constants.textVerticalOffsetRatio)
                font.family: Constants.fontFamily
                font.weight: Font.Bold
                font.pixelSize: Math.round(Math.min(parent.width, parent.height) * 0.32)
                color: '#bbada0'
                text: index
                visible: gameboard.debug
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
        spacing: Math.round(Math.min(parent.width, parent.height) / Math.min(rows, columns) * 0.12)

        Repeater {
            id: cellsRepeater

            objectName: 'CellsRepeater'
            model: parent.columns * parent.rows
            delegate: cell
        }
    }
}

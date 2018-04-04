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
import QtQuick.Window 2.0

Window {
    id: mainWindow

    objectName: 'mainWindow'
    width: minimumWidth
    height: minimumHeight
    minimumWidth: 520
    minimumHeight: 660

    Rectangle {
        id: background

        anchors.fill: parent
        color: '#eee4da'
        opacity: 0.5
    }

    Game {
        id: game

        anchors.fill: parent
        anchors.margins: 10
    }
}

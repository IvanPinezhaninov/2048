import QtQuick 2.9
import QtQuick.Window 2.2

Window {
    id: mainWindow

    objectName: 'mainWindow'
    width: minimumWidth
    height: minimumHeight
    minimumWidth: 520
    minimumHeight: 660

    visible: true

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

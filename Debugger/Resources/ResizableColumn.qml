import QtQuick 2.0

Item {
    property string name
    property int size: 0
    property int minimumSize: 0
    property string color: "#f0f0f0"

    anchors.top: parent.top
    anchors.bottom: parent.bottom

    width: border.x

    Rectangle {
        id: border

        x: size

        anchors.top: parent.top
        anchors.bottom: parent.bottom

        width: 1

        color: parent.color

        visible: parent.enabled

        MouseArea {
            drag.target: parent
            drag.axis: Drag.XAxis

            cursorShape: Qt.SizeHorCursor

            anchors.top: parent.top
            anchors.bottom: parent.bottom

            drag.minimumX: minimumSize

            width: 4
            x: -2
        }
    }
}

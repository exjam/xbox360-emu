import QtQuick 2.1
import QtQuick.Controls.Styles 1.0

TableViewStyle {
    ScrollBarStyle {
        id: sbs
    }

    handle: sbs.handle
    corner: sbs.corner
    decrementControl: sbs.decrementControl
    incrementControl: sbs.incrementControl
    scrollBarBackground: sbs.scrollBarBackground

    frame: null

    headerDelegate: Rectangle {
        color: "#e5e5e5"
        height: 37

        Text {
            text: styleData.value

            anchors.fill: parent
            anchors.leftMargin: 5

            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter

            color: "#333333"
            font.pixelSize: 12
            font.weight: Font.Bold
        }
    }

    rowDelegate: Rectangle {
        property bool hover : false

        height: 40
        color: styleData.selected ? "#ffffff" : (hover ? "#f2f2f2" : "#e5e5e5")

        Rectangle {
            height: 1
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top

            color: "#c4c4c4"
        }

        MouseArea {
            hoverEnabled: true
            anchors.fill: parent
            onEntered: parent.hover = true
            onExited: parent.hover = false
        }
    }

    itemDelegate: Item {
        height: Math.max(40, label.implicitHeight)

        property int implicitWidth: sizehint.paintedWidth + 4

        Text {
            id: label

            text: styleData.value != undefined ? styleData.value : ""

            width: parent.width

            anchors.left: parent.left
            anchors.right: parent.right
            anchors.leftMargin: 10

            anchors.verticalCenter: parent.verticalCenter
            anchors.verticalCenterOffset: 1

            color: "#707070"
            font.pixelSize: 14
            elide: styleData.elideMode
            horizontalAlignment: styleData.textAlignment
        }

        Text {
            id: sizehint
            font: label.font
            text: styleData.value ? styleData.value : ""
            visible: false
        }
    }
}

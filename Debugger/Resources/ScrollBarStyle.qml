import QtQuick 2.1
import QtQuick.Controls.Styles 1.0

ScrollViewStyle {
    corner: null
    decrementControl: null
    incrementControl: null

    scrollBarBackground: Item {
        implicitWidth: 11
        implicitHeight: 11
        clip: true

        Rectangle {
            opacity: styleData.hovered ? 1 : 0

            color: "#d9d9d9"
            border.color: "#c2c2c2"

            anchors.fill: parent
            anchors.rightMargin: styleData.horizontal ? -2 : -1
            anchors.leftMargin: styleData.horizontal ? -2 : 0
            anchors.topMargin: styleData.horizontal ? 0 : -2
            anchors.bottomMargin: styleData.horizontal ? -1 : -2

            Behavior on opacity { PropertyAnimation {} }
        }
    }

    handle: Item {
        implicitWidth: 11
        implicitHeight: 11
        clip: true

        Rectangle {
            id: handleBackground

            color: styleData.pressed ? "#6c6c6c" : (styleData.hovered ? "#818181" : "#b6b6b6")

            anchors.fill: parent
            anchors.leftMargin: styleData.horizontal ? 0 : 2
            anchors.topMargin: styleData.horizontal ? 2 : 0

            Rectangle {
                id: outerBorderLeft

                width: 1
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.bottom: parent.bottom

                color: styleData.pressed ? "#535353" : (styleData.hovered ? "#656565" : "#a4a4a4")
            }

            Rectangle {
                id: outerBorderTop

                height: 1
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right

                color: styleData.pressed ? "#535353" : (styleData.hovered ? "#656565" : "#a4a4a4")
            }

            Rectangle {
                id: innerBorderLeft

                width: 1
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.bottom: parent.bottom
                anchors.leftMargin: 1
                anchors.topMargin: 1

                color: styleData.pressed ? "#5e5e5e" : (styleData.hovered ? "#7b7b7b" : "#b6b6b6")
            }

            Rectangle {
                id: innerBorderTop

                height: 1
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.topMargin: 1
                anchors.leftMargin: 1

                color: styleData.pressed ? "#5e5e5e" : (styleData.hovered ? "#7b7b7b" : "#b6b6b6")
            }
        }
    }
}

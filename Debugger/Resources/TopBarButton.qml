import QtQuick 2.0

Rectangle {
    id: rect
    width: 10+17+10
    anchors.top: parent.top
    anchors.bottom: parent.bottom

    property string image
    property string text
    property bool hover
    property bool pressed
	property bool enabled: true
	
    gradient: Gradient {
        GradientStop {
            position: 0
            color: rect.pressed ? "#1a1a1a" : rect.hover ? "#707070" : "#505050"
        }

        GradientStop {
            position: 1
            color: rect.pressed ? "#404040" : rect.hover ? "#909090" : "#707070"
        }
    }

    Image {
        anchors.leftMargin: 10
        anchors.topMargin: 1
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        width: 17
        height: 17
        source: parent.image
    }

    Rectangle {
		anchors.fill: parent
		visible: !rect.enabled
        color: "#000"
		opacity: 0.4
	}

    Rectangle {
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        width: 1
        color: "#909090"
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onEntered: rect.hover = rect.enabled
        onExited: rect.hover = false
        onPressed: rect.pressed = rect.enabled
        onReleased: rect.pressed = false
    }
}

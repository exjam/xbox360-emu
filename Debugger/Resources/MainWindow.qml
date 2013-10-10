import QtQuick 2.1
import QtQuick.Window 2.1
import QtQuick.Layouts 1.0
import QtQuick.Controls 1.0

ApplicationWindow {
    id: appWindow
    title: "xdbg"

    width: 1024
    height: 768

	color: "#fff"

    menuBar: MenuBar {
        Menu {
            title: "File"

            MenuItem {
                text: "Open"
            }

            MenuItem {
                text: "Exit"
                onTriggered: Qt.quit()
            }
        }

        Menu {
            title: "Debug"

            MenuItem {
                text: "Continue"
                shortcut: "F5"
				iconSource: "Resources/continue.png"
            }

            MenuItem {
                text: "Pause"
                shortcut: "Ctrl+Alt+Break"
				iconSource: "pause.png"
            }

            MenuItem {
                text: "Stop"
                shortcut: "Shift+F5"
				iconSource: "stop.png"
            }

            MenuItem {
                text: "Step Into"
                shortcut: "F11"
				iconSource: "step_into.png"
            }

            MenuItem {
                text: "Step Over"
                shortcut: "F10"
				iconSource: "step_over.png"
            }

            MenuItem {
                text: "Step Out"
                shortcut: "Shift+F11"
				iconSource: "step_out.png"
            }

            MenuItem {
                text: "Toggle Breakpoint"
                shortcut: "F9"
				iconSource: "breakpoint.png"
            }
        }
    }

    TopBar {
        id: topBar
		anchors.top: parent.top
        anchors.right: parent.right
        anchors.left: parent.left
    }

    SplitView {
        CodeView {
            id: codeView
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: 700
            Layout.fillWidth: true
        }

        RegisterView {
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            Layout.minimumWidth: 215
        }

        anchors.top: topBar.bottom
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        orientation: Qt.Horizontal
    }
}

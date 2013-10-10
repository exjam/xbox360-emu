import QtQuick 2.0

Item {
    height: 30

    Rectangle {
        anchors.fill: parent

        gradient: Gradient {
            GradientStop {
                position: 0
                color: "#505050"
            }

            GradientStop {
                position: 1
                color: "#707070"
            }
        }
    }

    Row {
        anchors.fill: parent

        TopBarButton {
            image: "continue.png"
            text: "Continue"
			enabled: false
        }

        TopBarButton {
            image: "pause.png"
            text: "Pause"
        }

        TopBarButton {
            image: "stop.png"
            text: "Stop"
        }

        TopBarButton {
            image: "step_into.png"
            text: "Step In"
        }

        TopBarButton {
            image: "step_over.png"
            text: "Step Over"
        }

        TopBarButton {
            image: "step_out.png"
            text: "Step Out"
        }
    }

    Rectangle {
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: 1

        color: "#202020"
    }
}

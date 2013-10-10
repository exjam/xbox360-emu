import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Private 1.0
import QtQuick.Controls.Styles 1.0

Item {
    id: root

    property bool horizontal
    property bool frameVisible: false

    property Component style: Qt.createComponent(Settings.style + "/ScrollViewStyle.qml", root)

    property StyleItem __styleitem: StyleItem { elementType: "frame" }

    property int minValue
    property int maxValue
    property int value: 0
    property int pageStep

	property MouseArea mouseArea: internal

    property Component __scrollbar: StyleItem {
        readonly property bool isTransient: root.__styleitem.styleHint("transientScrollBars")
        anchors.fill:parent
        elementType: "scrollbar"
        hover: activeControl != "none"
        activeControl: "none"
        sunken: __styleData.upPressed | __styleData.downPressed | __styleData.handlePressed
        minimum: root.minValue
        maximum: root.maxValue
        value: root.value
        horizontal: root.horizontal
        enabled: true
        implicitWidth: horizontal ? 200 : pixelMetric("scrollbarExtent")
        implicitHeight: horizontal ? pixelMetric("scrollbarExtent") : 200
    }

    Loader {
        id: frameLoader
        sourceComponent: parent.__scrollbar
        anchors.fill: parent

        property QtObject __styleData: QtObject {
            readonly property alias horizontal: root.horizontal
            readonly property alias upPressed: internal.upPressed
            readonly property alias downPressed: internal.downPressed
            readonly property alias handlePressed: internal.handlePressed
        }
    }

    MouseArea {
        id: internal

        property bool upPressed
        property bool downPressed
        property bool pageUpPressed
        property bool pageDownPressed
        property bool handlePressed
        property bool autoincrement

        property var pressedX
        property var pressedY
        property int pressedValue

        property int buttonHeight: horizontal ? root.height + 1 : root.width + 1
        property int buttonWidth: horizontal ? root.height + 1 : root.width + 1

        property Item __panel: frameLoader.item

        anchors.fill: parent
        hoverEnabled: true

        onEntered: if (!pressed) __panel.activeControl = __panel.hitTest(mouseX, mouseY)
        onExited: if (!pressed) __panel.activeControl = "none"
        onMouseXChanged: if (!pressed) __panel.activeControl = __panel.hitTest(mouseX, mouseY)

        Timer {
            running: internal.upPressed || internal.downPressed || internal.pageUpPressed || internal.pageDownPressed
            interval: 350
            onTriggered: internal.autoincrement = true
        }

        Timer {
            running: internal.autoincrement
            interval: 60
            repeat: true
            onTriggered: {
                if (internal.upPressed && internal.containsMouse) {
                    internal.decrement();
                } else if (internal.downPressed && internal.containsMouse) {
                    internal.increment();
                } else if (internal.pageUpPressed) {
                    internal.decrementPage();
                } else if (internal.pageDownPressed) {
                    internal.incrementPage();
                }

                var handleRect = internal.__panel.subControlRect("handle")

                if (internal.pageDownPressed) {
                    if (root.horizontal) {
                        if (handleRect.x + handleRect.width >= internal.mouseX) {
                            internal.pageDownPressed = false;
                        }
                    } else {
                        if (handleRect.y + handleRect.height >= internal.mouseY) {
                            internal.pageDownPressed = false;
                        }
                    }
                }

                if (internal.pageUpPressed) {
                    if (root.horizontal) {
                        if (handleRect.x <= internal.mouseX) {
                            internal.pageUpPressed = false;
                        }
                    } else {
                        if (handleRect.y <= internal.mouseY) {
                            internal.pageUpPressed = false;
                        }
                    }
                }
            }
        }

        onPressed: {
            __panel.activeControl = __panel.hitTest(mouseX, mouseY)

            pressedY = mouseY;
            pressedX = mouseX;
            pressedValue = root.value

            if (__panel.activeControl === "handle") {
                handlePressed = true;
            } else if (__panel.activeControl === "up") {
                decrement();
                internal.upPressed = Qt.binding(function() {return containsMouse});
            } else if (__panel.activeControl === "down") {
                increment();
                internal.downPressed = Qt.binding(function() {return containsMouse});
            } else {
                if (__panel.activeControl === "upPage") {
                    decrementPage();
                    internal.pageUpPressed = true;
                } else if (__panel.activeControl === "downPage") {
                    incrementPage();
                    internal.pageDownPressed = true;
                }
            }
        }

        onPositionChanged: {
            if (internal.handlePressed) {
                var handleRect = __panel.subControlRect("handle")

                if (root.horizontal) {
                    var pixelValue = (root.maxValue - root.minValue) / (root.width - buttonWidth * 2 - handleRect.width);
                    root.value = Math.max(Math.min(pressedValue + (mouseX - pressedX) * pixelValue, root.maxValue), root.minValue);
					onChanged(root.value)
                } else {
                    var pixelValue = (root.maxValue - root.minValue) / (root.height - buttonHeight * 2 - handleRect.height);
                    root.value = Math.max(Math.min(pressedValue + (mouseY - pressedY) * pixelValue, root.maxValue), root.minValue);
					onChanged(root.value)
                }
            }
        }

        onReleased: {
            __panel.activeControl = __panel.hitTest(mouseX, mouseY);
            internal.autoincrement = false;
            internal.upPressed = false;
            internal.downPressed = false;
            internal.handlePressed = false;
            internal.pageUpPressed = false;
            internal.pageDownPressed = false;
        }

		onWheel: {
			add(wheel.angleDelta.y / -120)
		}

        function add(val)
        {
			if (val < 0) {
				root.value = Math.max(root.value + val, root.minValue)
			} else {
				root.value = Math.min(root.value + val, root.maxValue)
			}

			onChanged(root.value)
        }

        function decrement()
        {
            root.value = Math.max(root.value - 1, root.minValue)
			onChanged(root.value)
        }

        function increment()
        {
            root.value = Math.min(root.value + 1, root.maxValue)
			onChanged(root.value)
        }

        function decrementPage()
        {
            root.value = Math.max(root.value - root.pageStep, root.minValue)
			onChanged(root.value)
        }

        function incrementPage()
        {
            root.value = Math.min(root.value + root.pageStep, root.maxValue)
			onChanged(root.value)
        }
    }
}

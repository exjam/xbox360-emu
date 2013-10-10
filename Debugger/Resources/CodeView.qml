import QtQuick 2.1
import QtQuick.Controls 1.0

Item {
	CodeScrollBar
	{
		id: codeScrollbar

		anchors.top: parent.top
		anchors.bottom: parent.bottom
		anchors.right: parent.right
		width: 16
		horizontal: false

        minValue: 0
        maxValue: disassembleView.rowCount
        value: (0x82F23470 - 0x82000400) / 4
        pageStep: repeater.count - 1

		function onChanged(index) {
			console.log(index)
		}
	}
	
    MouseArea {
		anchors.top: parent.top
		anchors.bottom: parent.bottom
		anchors.left: parent.left
		anchors.right: codeScrollbar.left

		onWheel: {
			codeScrollbar.mouseArea.onWheel(wheel)
		}

        onPressed: {
			var address = (mouseY / 15) + codeView.index

            codeView.selectDown = address
            codeView.selectStart = address
            codeView.selectEnd = address

			console.log(address);
        }

        onPositionChanged: {
            var address = (mouseY / 15) + codeView.index

            if (address < codeView.selectDown) {
                codeView.selectStart = address
                codeView.selectEnd = codeView.selectDown
            } else {
                codeView.selectStart = codeView.selectDown
                codeView.selectEnd = address
            }
        }
    }

    Column
	{
        id: codeView

		anchors.top: parent.top
		anchors.bottom: parent.bottom
		anchors.left: parent.left
		anchors.right: codeScrollbar.left

		property int index: codeScrollbar.value

		property int selectDown: 0
		property int selectStart: 0
		property int selectEnd: 0

        onHeightChanged: {
            repeater.model = height / 15
            console.log(repeater.model)
        }

        Repeater {
            id: repeater

            model: 15

            Item {
				id: rowItem

                height: 15
                anchors.left: codeView.left
                anchors.right: codeView.right

				property bool isSelected: (index + codeView.index) >= codeView.selectStart && (index + codeView.index) <= codeView.selectEnd
				property bool isBreakpoint: false
				property bool isExecutionPoint: false
				
				Rectangle {
					anchors.fill: parent
					color: "#f0f0f0"
					visible: rowItem.isSelected
				}

				Row {
					anchors.fill: parent

					Item {
						width: column0.width
						anchors.top: parent.top
						anchors.bottom: parent.bottom

						Rectangle {
							anchors.fill: parent
							color: rowItem.isExecutionPoint ? "#a0a0a0" : "#f08080"
							visible: rowItem.isExecutionPoint || rowItem.isBreakpoint
						}

						Text {
							anchors.fill: parent
							anchors.leftMargin: 5

							text: disassembleView.address(index + codeView.index)
							clip: true
							font.pointSize: 10
							font.family: "Consolas"
							renderType: Text.NativeRendering
							color: rowItem.isExecutionPoint ? (rowItem.isBreakpoint ? "#ff0000" : "#f0f0f0") : (rowItem.isBreakpoint ? "#000" : "#a0a0a0")
						}
					}
					
					Item {
						width: column2.width
						anchors.top: parent.top
						anchors.bottom: parent.bottom

						Text {
							anchors.fill: parent
							anchors.leftMargin: 5
							
							text: disassembleView.code(index + codeView.index)
							textFormat: Text.StyledText
							clip: true
							font.pointSize: 10
							font.family: "Consolas"
							renderType: Text.NativeRendering
							color: "#000"
						}
					}

					Item {
						width: parent.width - x
						anchors.top: parent.top
						anchors.bottom: parent.bottom

						Text {
							anchors.fill: parent
							anchors.leftMargin: 5

							text: disassembleView.comment(index + codeView.index)
							clip: true
							font.pointSize: 10
							font.family: "Consolas"
							renderType: Text.NativeRendering
							color: "#a0a0a0"
						}
					}
				}
            }
        }
    }

    Row {
        anchors.fill: parent

        ResizableColumn {
            id: column0
            name: "Address"
            size: 65
            color: "#e0e0e0"
        }

        ResizableColumn {
            id: column2
            name: "Text"
            size: 500
            color: "#e0e0e0"
        }

        ResizableColumn {
            id: column3
            name: "Comment"
            size: 500
            enabled: false
            color: "#e0e0e0"
        }
    }
}

import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Private 1.0
import QtQuick.Controls.Styles 1.0

Item {
    ListModel {
        id: gprModel

        Component.onCompleted: {
            for (var i = 0; i < 32; i += 1)
                gprModel.append(
                            {
                                "name": "R" + i,
                                "value": 0x01020304,
                            })
        }
    }

    ListModel {
        id: fpuModel

        Component.onCompleted: {
            for (var i = 0; i < 32; i += 1)
                fpuModel.append(
                            {
                                "name": "F" + i,
                                "value": 0x01020304,
                            })
        }
    }

    ScrollView {
        style: ScrollBarStyle {}
        anchors.fill: parent
        anchors.leftMargin: 5
        anchors.topMargin: 5

        Item {
            height: childrenRect.height

            Row {
                ListView {
                    id: gprView
                    model: gprModel
                    width: 120
                    height: childrenRect.height

                    delegate: Item {
                        height: 10

                        Text {
                            text: name
                            font.pointSize: 9
                            font.family: "Consolas"
                            renderType: Text.NativeRendering
                            color: "#000"
                        }

                        Text {
                            x: 30
                            text: value
                            font.pointSize: 9
                            font.family: "Consolas"
                            renderType: Text.NativeRendering
                            color: "#555"
                        }
                    }
                }

                ListView {
                    id: fpuView

                    model: fpuModel
                    width: 120
                    height: childrenRect.height

                    delegate: Item {
                        height: 10

                        Text {
                            text: name
                            font.pointSize: 9
                            font.family: "Consolas"
                            renderType: Text.NativeRendering
                            color: "#000"
                        }

                        Text {
                            x: 30
                            text: value
                            font.pointSize: 9
                            font.family: "Consolas"
                            renderType: Text.NativeRendering
                            color: "#555"
                        }
                    }
                }
            }
        }
    }
}

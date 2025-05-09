import QtQuick 2.9
import QtQuick.Window 2.2
import Grim.DeviceMonitor 1.0
import QtQuick.Controls 2.0

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

    Column {
        anchors.fill: parent
        spacing: 10
        Repeater {
            model: DeviceMonitor.serialDevices
            
            delegate: Label {
                required property string modelData
                text: modelData
                width: parent.width
                height: 50
            }
        }
    }
}

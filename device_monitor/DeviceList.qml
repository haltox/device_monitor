import QtQuick 2.9
import QtQuick.Window 2.2
import Grim.DeviceMonitor 1.0
import QtQuick.Controls 2.0
import QtQuick.Layouts

Window {
    id: deviceList
    property int selectedItem : -1

    readonly property int lineHeight : 24;
    readonly property int numberOfLines : 5;

    visible: true
    width: 320
    height: lineHeight * numberOfLines

    minimumWidth : width;
    maximumWidth: minimumWidth;
    minimumHeight: height;
    maximumHeight: minimumHeight;

    title: qsTr("Ports COM")

    x: screen.width - width - 48
    y: 64

    Flickable {
        anchors.fill: parent
        clip: true
        ScrollBar.vertical: ScrollBar {  }
        contentHeight: col.height

        Column {
            id:col
            anchors.left: parent.left
            anchors.right: parent.right

            Repeater {
                model: DeviceMonitor.serialDevices
                delegate: Rectangle {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    //anchors.margins: 8
                    height: lineHeight + 16
                    
                    //radius: 8
                    //border.width: 1
                    //border.color: (index == deviceList.selectedItem) 
                    //    ? Universal.accent
                    //    : Universal.foreground
                    color: (index == deviceList.selectedItem) 
                        ? Qt.lighter(Universal.accent, 1.25)
                        : Universal.background

                    Label { 
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                        anchors.margins: 8
                        text: modelData
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            if( deviceList.selectedItem == index ) {
                                deviceList.selectedItem = -1;
                            }
                            else {
                                deviceList.selectedItem = index;
                            }
                        }
                    }
                }
            }
        }
    }
}

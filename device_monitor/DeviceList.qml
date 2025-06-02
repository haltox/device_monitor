import QtQuick 2.9
import QtQuick.Window 2.2
import Grim.DeviceMonitor 1.0
import Grim.Settings 1.0
import QtQuick.Controls 2.0
import QtQuick.Layouts

Window {
    id: deviceList
    property int selectedItem : -1

    readonly property int lineHeight : 24;
    readonly property int numberOfLines : 5;

    visible: true
    width: 480
    height: lineHeight * numberOfLines

    minimumWidth : width;
    maximumWidth: minimumWidth;
    minimumHeight: height;
    maximumHeight: minimumHeight;

    title: qsTr("Ports COM")

    x: screen.width - width - 48
    y: 64

    function getComPorStr(portLabel) {
        // Extrait le numéro COM d'une étiquette de port, ex: "COM3 (Arduino)"
        var match = /\((COM\d+)\)/i.exec(portLabel);
        if (match && match.length > 1) {
            return match[1];
        }
        return "";      
    }
    
    function copyToClipboard(portLabel) {
        Settings.CopyToClipboard(getComPorStr(portLabel));
    }

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
                    id: item
                    anchors.left: parent.left
                    anchors.right: parent.right
                    height: lineHeight + 16
                    
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
                            deviceList.selectedItem = index;
                            item.forceActiveFocus();
                        }
                        onDoubleClicked: {
                            deviceList.copyToClipboard(modelData);
                        }
                    }

                    Keys.onPressed: function(ev) {
                        if( ev.key === Qt.Key_C && ev.modifiers === Qt.ControlModifier ) {
                            deviceList.copyToClipboard(modelData);
                        }
                    }
                }
            }
        }
    }
}

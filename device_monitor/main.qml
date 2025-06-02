import QtQuick.Controls.Universal 

import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 2.0
import Qt.labs.platform;
import QtQml;

import Grim.DeviceMonitor 1.0
import Grim.Settings 1.0

SystemTrayIcon {
    id: system
    
    visible: true
    icon.source: "qrc:/portw.png"

    function findMostLikelyPort() {
        var list = DeviceMonitor.serialDevices;
        for( var l in list ) {
            if(  list[l].indexOf(Settings.relevantPortPattern) != -1) {
                return [list[l]];
            }
        }

        return [];
    }

    function portIsRelevant(port) {
        return (port.indexOf(Settings.relevantPortPattern) != -1);
    }

    property Connections c: Connections {
        target: DeviceMonitor;
        function onSerialDevicesChanged() { 
            if(Settings.showNotifications && Settings.showNotificationsPortsChanged){
                system.showMessage("Ports COM", 
                    "Les ports COMs détectés ont changé", 
                    SystemTrayIcon.Information, 
                    1000);
            }
        }

        function onSerialDeviceAdded(port) { 
            if(Settings.showNotifications && Settings.showNotificationsPortAdded){
                if( !Settings.showOnlyRelevantPorts || portIsRelevant(port) ) {
                    system.showMessage("Ports COM", 
                                    "Port connecté : " + port, 
                                    SystemTrayIcon.Information, 
                                    1000 );
                }
            }
        }

        function onSerialDeviceRemoved(port) { 
            if(Settings.showNotifications){
                if( !Settings.showOnlyRelevantPorts || portIsRelevant(port) ) {
                    system.showMessage("Ports COM", 
                                    "Port retiré : " + port, 
                                    SystemTrayIcon.Information, 
                                    1000 );
                }
            }
        }
    }

    property DeviceList deviceList: DeviceList {
        id: deviceList;
        visible: false;
        Component.onCompleted: {
            if( Settings.showDeviceListOnStartup ) {
                deviceList.show();
            }
        }
    }

    property SetupWindow setupWindow: SetupWindow {
        id: setupWindow
        visible: false;
        Component.onCompleted: {
            if( Settings.showSettingsOnStartup || Settings.firstStart ) {
                setupWindow.show();
            }
        }
    }

    menu: Menu {
        id: menu
        title: qsTr("Device Monitor")
        visible: true
        MenuItem {
            text: qsTr("Ports COM")
            onTriggered: deviceList.show();
        }
        MenuItem {
            text: qsTr("Paramètres")
            onTriggered: setupWindow.show();
        }
        MenuItem {
            text: qsTr("Quitter")
            onTriggered: Qt.quit()
        }
    }

    onActivated: (reason) => {
        if(reason == SystemTrayIcon.DoubleClick) {
            deviceList.show();
        }
    }
}
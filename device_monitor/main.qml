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

    /*
    property Timer timer : Timer {
        interval: 1000;
        repeat: false;
        running: true;
        onTriggered: {
            if(system.supportsMessages) {
                system.showMessage("test", "test", SystemTrayIcon.Information, 1000);
            }
            else {
                console.log("SystemTrayIcon does not support messages");
            }

        }
    }
    */

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
            if( Settings.showSettingsOnStartup ) {
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
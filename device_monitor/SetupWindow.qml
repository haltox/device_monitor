import QtQuick 2.9
import QtQuick.Window 2.2
import Grim.DeviceMonitor 1.0
import QtQuick.Controls 2.0
import Grim.Settings 1.0
import QtQuick.Layouts

Window {
    visible: true
    width: 640
    height: 250
    title: qsTr("Ports COM - Paramètres")

    Rectangle {
        id: container;
        anchors.fill: parent;
        anchors.margins: 10;
        border.color : "#E8E9ED";
        border.width : 1;
        radius: 4;

        Flickable {
            anchors.fill: parent
            clip: true
            ScrollBar.vertical: ScrollBar {  }
            contentHeight: col.height

            Column {
                id: col
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.margins: 10

                Label {
                    text: "Paramètres de l'application"
                    font.pixelSize: 20
                    font.bold: true
                }

                CheckBox {
                    text: "Afficher la liste de ports COM au démarrage"
                    checked : Settings.showDeviceListOnStartup
                    onCheckedChanged: {
                        Settings.showDeviceListOnStartup = checked
                    }
                }

                CheckBox {
                    text: "Afficher la fenêtre de paramètres au démarrage de l'application"
                    checked : Settings.showSettingsOnStartup
                    onCheckedChanged: {
                        Settings.showSettingsOnStartup = checked
                    }
                }

                CheckBox {
                    text: "Afficher les messages de notification"
                    checked : Settings.showNotifications
                    onCheckedChanged: {
                        Settings.showNotifications = checked
                    }
                }

                CheckBox {
                    text: "Afficher le port le plus pertinent lors des notifications"
                    checked : Settings.showMostLikelyPort
                    onCheckedChanged: {
                        Settings.showMostLikelyPort = checked
                    }
                }

                Row {
                    spacing: 10
                    Label {
                        text: "Schéma d'identification du port: "
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    TextField{
                        text: Settings.likelyPortPattern
                        width: 200
                        onTextChanged: {
                            Settings.likelyPortPattern = text
                        }
                    }
                }
            }
        }
    } // container


}

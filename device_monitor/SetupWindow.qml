import QtQuick 2.9
import QtQuick.Window 2.2
import Grim.DeviceMonitor 1.0
import QtQuick.Controls 2.0
import Grim.Settings 1.0
import QtQuick.Layouts

Window {
    visible: true
    width: 640
    height: 460
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
                spacing: 10

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
                    text: "Afficher les messages de notification lors d'un ajout de port"
                    checked : Settings.showNotificationsPortAdded
                    onCheckedChanged: {
                        Settings.showNotificationsPortAdded = checked
                    }
                }

                CheckBox {
                    text: "Afficher les messages de notification lors d'un retrait de port"
                    checked : Settings.showNotificationsPortRemoved
                    onCheckedChanged: {
                        Settings.showNotificationsPortRemoved = checked
                    }
                }

                CheckBox {
                    text: "Afficher les messages de notification lors de tout changement de ports"
                    checked : Settings.showNotificationsPortsChanged
                    onCheckedChanged: {
                        Settings.showNotificationsPortsChanged = checked
                    }
                }

                CheckBox {
                    text: "Afficher uniquement les ports pertinents lors des notifications"
                    checked : Settings.showOnlyRelevantPorts
                    onCheckedChanged: {
                        Settings.showOnlyRelevantPorts = checked
                    }
                }

                Row {
                    spacing: 10
                    Label {
                        text: "Schéma d'identification du port: "
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    TextField{
                        text: Settings.relevantPortPattern
                        width: 200
                        onTextChanged: {
                            Settings.relevantPortPattern = text
                        }
                    }
                }

                Row {
                    spacing: 10
                    Button {
                        width: 290
                        text: "Ouvrir répertoire de l'application"
                        onClicked: {
                            Settings.OpenAppDirInExplorer();
                        }
                    }

                    Button {
                        width: 290
                        text: "Réinitialiser les paramètres"
                        onClicked: {
                            Settings.ResetSettings();
                        }
                    }
                }
            }
        }
    } // container


}

/* Copyright 2018 WobLight
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.9
import GitAddonsManager.engine 1.0
import QtQuick.Controls 2.4
import QtQuick.Dialogs 1.0
import QtQuick.Layouts 1.3

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    id: window

    property int availableUpdates: 0
    property bool addonsReady: false
    Component.onCompleted: {
        availableUpdates = Qt.binding(function(){
            var count = 0
            for (var i = 0; i < Engine.addons.length; i++) {
                var addon = Engine.addons[i]
                if (addon.gitStatus & Addon.Behind)
                    count = count + 1
            }
            return count
        })
        addonsReady = Qt.binding(function(){
            for (var i = 0; i < Engine.addons.length; i++) {
                var addon = Engine.addons[i]
                if (addon.status == Addon.Status.Busy)
                    return false
            }
            return true
        })
    }

    function updateAll() {
        for (var i = 0; i < Engine.addons.length; i++) {
            var addon = Engine.addons[i]
            if (addon.gitStatus & Addon.Behind)
                addon.update()
        }
    }

    onAvailableUpdatesChanged: {
        if (availableUpdates > 0) {
            tray.showMessage("Addons updates available", "You have " + availableUpdates + " addons waiting for update.", 1, 5000)
        } else {
            tray.showMessage("Addons up to date", "All addons are updated", 1)
        }
    }

    onClosing: {
       if (Engine.minimizeToTray == Engine.MinimizeToTrayAsk) {
            close.accepted = false
            closeDialog.visible = true
        } else if (Engine.minimizeToTray == Engine.MinimizeToTrayNo)
            Qt.quit()
       else if (Qt.platform.os == "windows")
           window.visible = false
    }

    Action {
        id: refreshAction
        icon.name: "view-refresh"
        onTriggered: Engine.scanForAddons()
        shortcut: StandardKey.Refresh
        enabled: Control.status == Control.Ready && addonsReady
    }
    Action {
        id: addAction
        icon.name: "list-add"
        onTriggered: addDialog.visible = true
        shortcut: StandardKey.New
    }
    Action {
        id: updateAllAction
        icon.name: availableUpdates > 0 ? "update-high" : "update-none"
        onTriggered: updateAll()
        enabled: availableUpdates > 0
    }
    Action {
        id: openAddonsFolder
        icon.name: "go-parent-folder"
        onTriggered: Qt.openUrlExternally((Qt.platform.os == "windows" ? "file:///" : "file://")+Engine.addonsPath)
    }

    ColumnLayout{
        anchors.fill: parent
        TabBar {
            Layout.fillWidth: true
            id: bar
            TabButton {
                text: qsTr("Addons")
            }
            TabButton {
                text: qsTr("Options")
            }
            TabButton {
                text: qsTr("About")
            }
        }

        StackLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: bar.currentIndex
            AddonsPanel {
                Layout.fillHeight: true
                Layout.fillWidth: true

            }

            Options {
                width: parent.width
            }

            ColumnLayout {
                Label {
                    text: "Author: woblight <woblight@gmail.com>"
                }
                Label {
                    text: "Source code: <a href=https://gitlab.com/woblight/GitAddonsManager>GitLab</a>"
                    onLinkActivated: Qt.openUrlExternally(link)
                    textFormat: Text.RichText
                }
                RowLayout {
                    Label {
                        text: "Version:"
                    }
                    TextArea {
                        readOnly: true
                        text: gitVersion
                    }
                }
            }
        }
    }
    FileDialog {
        id: fileDialog
        title: qsTr("Choose addons directory")
        folder: Engine.addonsPath != "" ? Engine.addonsPath : shortcuts.home
        onAccepted: Engine.addonsPath = fileUrl.toString().replace("file://","")
        selectFolder: true
        visible: Engine.addonsPath == ""
    }
    Dialog {
        parent: ApplicationWindow.overlay
        id: cloneHelpDialog
        modal: true
        visible: cloneHelpBtn.checked
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        title: qsTr("Clone URL Help")
        ColumnLayout {
            anchors.fill: parent
            Label {
                text: "Look for \"clone url\" on the repository webpage, it usually ends in .git"
            }
            Label { text: "Gitlab" }
            Image {
                source: "qrc:/gitlab_clone_url.jpg"
                fillMode: Image.PreserveAspectCrop
                Layout.fillHeight: true
                Layout.fillWidth: true
            }
            Label { text: "Github" }
            Image {
                source: "qrc:/github_clone_url.jpg"
                fillMode: Image.PreserveAspectCrop
                Layout.fillHeight: true
                Layout.fillWidth: true
            }
        }
    }
    Dialog {
        width: window.width
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        id: addDialog
        title: "Clone Repository"
        standardButtons: Dialog.Ok | Dialog.Cancel
        RowLayout {
            anchors.fill: parent
            TextField {
                Layout.fillWidth: true
                id: addField
                placeholderText: "Git clone url"
                onAccepted: addDialog.accept()
                focus: visible
            }
            ToolButton {
                id: cloneHelpBtn
                icon.name: "help-whatsthis"
                onClicked: cloneHelpDialog.visible = true
            }
        }
        focus: visible
        modal: true
        onAccepted: {
            Engine.clone(addField.text)
            addField.clear()
        }
        onRejected: addField.clear()
    }

    Dialog {
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        title: Engine.statusMessage
        id: cloningDialong
        ProgressBar {
            anchors.fill: parent
            id: cloningBar
            to: Engine.total
            value: Engine.progress
            indeterminate: to == 0
        }
        visible: Engine.status == Engine.Busy
        modal: true
        closePolicy: Popup.NoAutoClose
    }
    Dialog {
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        title: "An error occourred"
        id: errorDialong
        Label {
            text: Engine.statusMessage
        }
        visible: Engine.status == Engine.Error
        modal: true
        closePolicy: Popup.NoAutoClose
        standardButtons: Dialog.Ok
        onClosed: Engine.status = Engine.Ready
    }
    Tray {
        id: tray
    }

    Dialog {
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        visible: Engine.addonsPath != "" && !Engine.addonsPath.toLowerCase().match("interface/addons$")
        title: qsTr("Warning")
        Label {
            anchors.fill: parent
            text: qsTr("The path set for addons does not appear to be valid. Please make sure it is set to the Interface/AddOns folder inside your World of Warcraft folder.")
            wrapMode: Text.WordWrap
        }
        modal: true
    }

    Dialog {
        id: closeDialog
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        closePolicy: Popup.NoAutoClose
        modal: true
        title: qsTr("Choose closing behaviour")
        Label {
            text: qsTr("Your choise will be saved and be changed any time by the Options panel")
        }
        footer: DialogButtonBox {
            Button {
                text: "Close Application"
                onClicked: {
                    Engine.minimizeToTray = Engine.MinimizeToTrayNo
                    Qt.quit()
                }
                DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
            }
            Button {
                text: "Minimize in System Tray"
                onClicked: {
                    Engine.minimizeToTray = Engine.MinimizeToTrayYes
                    close()
                }
                DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
            }
        }
    }
    Dialog {
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        visible: Engine.firstBoot
        onClosed: Engine.firstBoot = false
        title: qsTr("Note on GitAddonsManager")
        Label {
            anchors.fill: parent
            text: qsTr("Greetings,<br/>"+
                       "This application will manage <b>only</b> addons <b>cloned</b> with git, not regularly downloaded ones. <i><b>If you don't know what that means</b>, probably none of your current addons will be listed and only addons added through this aplication will be managed.</i><br/>"+
                       "With this application is <b>only possible</b> to clone (download) addons from <b>git repositories</b> (e.g. Gitlab and Github).<br/>Finally, this is an alpha version things might not go as intended.")
            textFormat: Text.RichText
            wrapMode: Text.WordWrap
        }
        modal: true
        standardButtons: Dialog.Close
    }
}

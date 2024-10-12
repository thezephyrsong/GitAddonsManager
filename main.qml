/* Copyright 2022 WobLight
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

import QtQuick
import GitAddonsManager.engine
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    id: window

    property int availableUpdates: 0
    property bool addonsReady: false
    Component.onCompleted: {
        availableUpdates = Qt.binding(function(){
            if (!(Engine.status == Engine.Ready && addonsReady))
                return -1;
            var count = 0
            for (var i = 0; i < Engine.addons.length; i++) {
                var addon = Engine.addons[i]
                if (addon.gitStatus & Addon.Behind)
                    count = count + 1
            }
            return count
        })
        addonsReady = Qt.binding(checkAddonsReady)
    }

    function checkAddonsReady(){
        for (var i = 0; i < Engine.addons.length; i++) {
            var addon = Engine.addons[i]
            if (addon.status == Addon.Status.Busy)
                return false
        }
        return true
    }
    function updateAll() {
        for (var i = 0; i < Engine.addons.length; i++) {
            var addon = Engine.addons[i]
            if (addon.gitStatus & Addon.Behind)
                addon.update()
        }
    }

    onAvailableUpdatesChanged: {
        if (availableUpdates == -1)
            return;
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
        enabled: Engine.status == Engine.Ready && addonsReady
    }
    Action {
        id: addAction
        icon.name: "list-add"
        onTriggered: addDialog.visible = true
        shortcut: StandardKey.New
    }
    Action {
        id: exportList
        icon.name: "export-symbolic"
        onTriggered: {
            exportListDialog.targetPath = null
            exportListDialog.visible = true
        }

        enabled: Engine.status == Engine.Ready && addonsReady
        shortcut: StandardKey.SaveAs
    }
    Action {
        id: updateAllAction
        onTriggered: updateAll()
        enabled: availableUpdates > 0
    }

    Page {
        anchors.fill: parent
        footer: ToolBar {
            height: messageLabel.implicitHeight
            RowLayout
            {
                anchors.fill: parent
                Label {
                    id: messageLabel
                    text: Engine.statusMessage
                }
                Label {
                    id: logButton
                    text: qsTr("[View log]")
                    MouseArea {
                        onClicked: logWindow.visible = !logWindow.visible
                        anchors.fill: parent
                    }
                    Layout.fillHeight: true
                    Layout.alignment: Qt.AlignRight
                }
            }
        }
        header: TabBar {
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

        contentItem: StackLayout {
            currentIndex: bar.currentIndex
            AddonsPanel {
            }

            Options {
            }

            ColumnLayout {
                Label {
                    text: "Author: woblight <woblight@gmail.com>"
                }
                Label {
                    text: "Source code: <a href=\"https://gitlab.com/woblight/GitAddonsManager\">GitLab</a>"
                    onLinkActivated: Qt.openUrlExternally(link)
                    textFormat: Text.RichText
                }
                Label {
                    text: "Report an issue: <a href=\"https://gitlab.com/woblight/GitAddonsManager/-/issues\">GitLab</a>"
                    onLinkActivated: Qt.openUrlExternally(link)
                    textFormat: Text.RichText
                }
                Label {
                    text: "Website: <a href=\"https://woblight.gitlab.io/overview/gitaddonsmanager/\">woblight.gitlab.io</a>"
                    onLinkActivated: Qt.openUrlExternally(link)
                    textFormat: Text.RichText
                }
                Label {
                    text: "Discord: <a href=\"https://discord.gg/YZQcPMQ\">YZQcPMQ</a>"
                    onLinkActivated: Qt.openUrlExternally(link)
                    textFormat: Text.RichText
                }
                RowLayout {
                    Label {
                        text: "Version:"
                    }
                    TextArea {
                        readOnly: true
                        text: Engine.version
                        selectByMouse: true
                        onFocusChanged: if (focus) selectAll()
                    }
                    Button {
                        id: updateButton
                        visible: Engine.selfUpdates
                        contentItem: RowLayout {
                            BusyIndicator {
                                implicitHeight: parent.height
                                implicitWidth: implicitHeight
                                running: !updateButton.enabled
                                visible: running
                            }
                            Label {
                                text:
                                    Engine.updateStatus === Engine.NoUpdate ?
                                        qsTr("Check for update") :
                                        Engine.updateStatus === Engine.CheckingForUpdate ?
                                            qsTr("Checking for update...") :
                                            Engine.updateStatus === Engine.UpdateAvailable ?
                                                qsTr("Download update") :
                                                Engine.updateStatus === Engine.DownloadingUpdate ?
                                                    qsTr("Downloading update...") :
                                                    Engine.updateStatus === Engine.UpdateReady ?
                                                        qsTr("Unpacking update") : qsTr("error")
                            }
                        }
                        onClicked: Engine.updateStatus === Engine.NoUpdate ?
                                       Engine.checkForUpdates() :
                                       Engine.updateStatus === Engine.UpdateAvailable ?
                                           Engine.downloadUpdate() :
                                           Engine.updateStatus === Engine.UpdateReady ?
                                               Engine.executeUpdate() : {}
                        enabled: Engine.updateStatus === Engine.NoUpdate || Engine.updateStatus === Engine.UpdateAvailable || Engine.updateStatus === Engine.UpdateReady
                    }
                }
            }
        }
    }
    FolderDialog {
        id: fileDialog
        property int selector: -1
        title: qsTr("Choose addons directory")
        onAccepted: Engine.setAddonsPath(selector, fileDialog.selectedFolder.toString().replace("file://",""))
        visible: Engine.addonsPaths.length == 0
    }
    Dialog {
        parent: ApplicationWindow.overlay
        id: cloneHelpDialog
        modal: true
        visible: cloneHelpBtn.checked
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        standardButtons: Dialog.Close
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
        ColumnLayout {
            anchors.fill: parent
            RowLayout {
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
            ComboBox {
                id: pathChooser
                visible: Engine.addonsPaths.length > 1
                model: Engine.addonsPaths
                currentIndex: 0
                Layout.fillWidth: true
            }
        }
        focus: visible
        modal: true
        onAccepted: {
            Engine.clone(addField.text, pathChooser.currentIndex)
            addField.clear()
        }
        onRejected: addField.clear()
    }

    Dialog {
        x: (parent.width - implicitWidth) / 2
        y: (parent.height - implicitHeight) / 2
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
        id: errorDialog
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
        property var invalidPaths: {
            var l = []
            for (var i = 0; i < Engine.addonsPaths.length; i++)
                if (Engine.addonsPaths[i] != "" && !(Engine.addonsPaths[i].toLowerCase().match("interface/addons$") || Engine.addonsPaths[i].toLowerCase().match("/run/user/\\d+/doc/\\w+/addons$")))
                    l.push(Engine.addonsPaths[i])
            return l
        }
        id: invalidDialog
        visible: invalidPaths.length > 0
        title: qsTr("Warning")
        standardButtons: Dialog.Close
        Label {
            anchors.fill: parent
            text: {
                var m = qsTr("Following paths don't appears to be valid. Please make sure it is set to the Interface/AddOns folder inside your World of Warcraft folder.", "", invalidDialog.invalidPaths.length)
                for (var i = 0; i < invalidDialog.invalidPaths.length; i++)
                    m += "<br/><b>" + invalidDialog.invalidPaths[i]+"</b>"
                return m
            }
            textFormat: Text.RichText
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
    Dialog {
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        visible: Engine.updateStatus === Engine.UpdateAvailable || Engine.updateStatus === Engine.DownloadingUpdate || Engine.updateStatus === Engine.UpdateReady
        id: updateDialog
        ColumnLayout {
            anchors.fill: parent
            Label {
                Layout.fillWidth: true
                id: updateMessage
                text: Engine.updateStatus === Engine.UpdateAvailable ?
                          qsTr("<h1>A newer version of GitAddonsManager is available</h1>") :
                          Engine.updateStatus === Engine.DownloadingUpdate ?
                              qsTr("Downloading update...") : qsTr("Download finished.<br/>To complete the update click \"Close &amp; Update\", then extract GitAddonsManager.zip and replace old files.")
                textFormat: Text.RichText
                wrapMode: Text.WordWrap
            }
            ProgressBar {
                Layout.fillWidth: true
                id: updateProgress
                to: Engine.total
                value: Engine.progress
                indeterminate: to == -1
                visible: Engine.updateStatus === Engine.DownloadingUpdate
            }
        }

        modal: true
        footer: DialogButtonBox {
            Button {
                visible: Engine.updateStatus === Engine.UpdateAvailable
                text: qsTr("Download")
                onClicked: Engine.downloadUpdate()
            }
            Button {
                visible: Engine.updateStatus === Engine.UpdateReady
                text: qsTr("Close && Update")
                onClicked: Engine.executeUpdate()
            }
        }
        closePolicy: Engine.updateStatus !== Engine.DownloadingUpdate ? Dialog.CloseOnPressOutside | Dialog.CloseOnEscape : Dialog.NoAutoClose
        standardButtons: Engine.updateStatus !== Engine.DownloadingUpdate ? Dialog.Close : Dialog.NoButton
    }
    FileDialog {
        id: exportListDialog
        property string targetPath
        onAccepted: Engine.exportAddonList(selectedFile, targetPath)
        fileMode: FileDialog.SaveFile
        defaultSuffix: "txt"
    }
    LogWindow {
        id: logWindow
    }
}

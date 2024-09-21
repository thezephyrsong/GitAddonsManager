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

import QtQuick
import QtQuick.Controls
import GitAddonsManager.engine
import QtQuick.Layouts
import QtCore

ScrollView {
    id: optPage
    ColumnLayout {
        width: Math.max(optPage.availableWidth, implicitWidth)
        Repeater {
            model: Engine.addonsPaths.length
            RowLayout {
                Layout.fillWidth: true
                TextField {
                    Layout.fillWidth: true
                    id: path
                    text: Engine.addonsPaths[index]
                    onAccepted: Engine.addonsPath = text
                    placeholderText: qsTr("Path to AddOns folder")
                    hoverEnabled: true
                    ToolTip.visible: hovered
                    ToolTip.text: qsTr("Path to the folder containing the addons")
                    enabled: Engine.status == Engine.Ready && addonsReady
                }
                Button {
                    enabled: Engine.status == Engine.Ready && addonsReady
                    text: qsTr("Browse")
                    onClicked: {fileDialog.selector = index; fileDialog.currentFolder = "file://" + Engine.addonsPaths[index]; fileDialog.visible = true}
                    hoverEnabled: true
                    ToolTip.visible: hovered
                    ToolTip.text: qsTr("Choose the folder containing the addons")
                }
                Button {
                    enabled: Engine.status == Engine.Ready && addonsReady
                    icon.name: "delete"
                    onClicked: Engine.setAddonsPath(index)
                }
            }
        }
        Button {
            enabled: Engine.status == Engine.Ready && addonsReady
            text: qsTr("Add directory")
            Layout.alignment: Qt.AlignRight
            onClicked: {fileDialog.selector = -1; fileDialog.visible = true}
        }

        CheckBox {
            Layout.fillWidth: true
            checked: Engine.useRepoDirectory
            onCheckedChanged: Engine.useRepoDirectory = checked
            text: qsTr("Use a dedicated repository directory")
            hoverEnabled: true
            ToolTip.visible: hovered
            ToolTip.text: qsTr("Directories needed to update addons but not directly containing addons will be grouped in a dedicated directory.")
        }

        CheckBox {
            Layout.fillWidth: true
            tristate: Engine.minimizeToTray == Engine.MinimizeToTrayAsk
            onCheckedChanged: Engine.minimizeToTray = checked ? Engine.MinimizeToTrayYes : Engine.MinimizeToTrayNo
            checkState: Engine.minimizeToTray == Engine.MinimizeToTrayAsk ?
                            Qt.PartiallyChecked :
                            Engine.minimizeToTray == Engine.MinimizeToTrayYes ?
                                Qt.Checked : Qt.Unchecked
            text: qsTr("Minimize to System Tray")
                hoverEnabled: true
            ToolTip.visible: hovered
            ToolTip.text: qsTr("Minimize to System Tray when the main window is closed.")
        }
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("Style")

            Component.onCompleted: {
                var settings
                if (Engine.style == "Material")
                    settings = Qt.createComponent("MaterialSettings.qml")
                else if (Engine.style == "Universal")
                    settings = Qt.createComponent("UniversalSettings.qml")
                else
                    settings = Qt.createComponent("StyleSettingsBase.qml")
                settings.createObject(this.contentItem)
            }
        }
        Dialog {
            id: restartDialog
            parent: window.overlay
            x: (parent.width - width) / 2
            y: (parent.height - height) / 2
            standardButtons: Dialog.Yes | Dialog.No
            Label {
                anchors.fill: parent
                text: qsTr("The application must be restarted for this change to take effect.\nRestart now?")
                wrapMode: Text.WordWrap
            }
            onAccepted: Qt.exit(1)
        }
    }
}

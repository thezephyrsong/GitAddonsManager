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
import QtQuick.Controls 2.4
import GitAddonsManager.engine 1.0
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.2
import QtQuick.Controls.Universal 2.2
import Qt.labs.settings 1.0

Pane {
    ColumnLayout {
        width: parent.width
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
                    enabled: Control.status == Control.Ready && addonsReady
                }
                Button {
                    enabled: Control.status == Control.Ready && addonsReady
                    text: qsTr("Browse")
                    onClicked: {fileDialog.selector = index; fileDialog.folder = "file://" + Engine.addonsPaths[index]; fileDialog.visible = true}
                    hoverEnabled: true
                    ToolTip.visible: hovered
                    ToolTip.text: qsTr("Choose the folder containing the addons")
                }
            }
        }
        Button {
            text: qsTr("Add directory")
            Layout.alignment: Qt.AlignRight
            onClicked: {fileDialog.selector = -1; fileDialog.visible = true}
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
            Flow {
                anchors.fill: parent
                spacing: 4
                ComboBox {
                    model: Engine.availableStyles
                    currentIndex: Engine.availableStyles.indexOf(Engine.style)
                    onActivated: {
                        Engine.style = Engine.availableStyles[currentIndex]
                        restartDialog.visible = true
                    }
                    ToolTip.text: qsTr("Requires restart")
                    ToolTip.visible: hovered
                    hoverEnabled: true
                }
                ComboBox {
                    id: materialTheme
                    visible: Engine.style == "Material"
                    model: ["Light", "Dark"]
                    onCurrentIndexChanged: window.Material.theme = currentIndex
                }
                ComboBox {
                    id: universalTheme
                    visible: Engine.style == "Universal"
                    model: ["Light", "Dark", "System"]
                    onActivated: window.Universal.theme = currentIndex
                }
                ListModel {
                    id: materialColors
                    dynamicRoles: true
                    Component.onCompleted: {
                        for (var i = 0; i < 19; i++)
                            append({"color": Material.color(i)})}
                }
                ListModel {
                    id: materialDarkColors
                    dynamicRoles: true
                    Component.onCompleted: {
                        for (var i = 0; i < 19; i++)
                            append({"color": Material.color(i,Material.Shade200)})}
                }
                ListModel {
                    id: universalColors
                    dynamicRoles: true
                    Component.onCompleted: {
                        for (var i = 0; i < 20; i++)
                            append({"color": Universal.color(i)})}
                }

                ColorPicker {
                    id: materialPrimary
                    visible: Engine.style == "Material"
                    text: qsTr("Primary:")
                    model: materialTheme.currentIndex === Material.Dark ? materialDarkColors : materialColors
                    currentIndex: Material.Indigo
                    onColorChanged: window.Material.primary = color
                }
                ColorPicker {
                    id: materialAccent
                    visible: Engine.style == "Material"
                    text: qsTr("Accent:")
                    model: materialTheme.currentIndex === Material.Dark ? materialDarkColors : materialColors
                    currentIndex: Material.Pink
                    onColorChanged: window.Material.accent = color
                }
                ColorPicker {
                    id: universalAccent
                    visible: Engine.style == "Universal"
                    text: qsTr("Accent:")
                    model: universalColors
                    currentIndex: Universal.Cobalt
                    onColorChanged: window.Universal.accent = color
                }

                Settings {
                    category: "Material"
                    property alias primary: materialPrimary.currentIndex
                    property alias accent: materialAccent.currentIndex
                    property alias theme: materialTheme.currentIndex
                }
                Settings {
                    category: "Universal"
                    property alias accent: universalAccent.currentIndex
                    property alias theme: universalTheme.currentIndex
                }
            }
        }
        Dialog {
            id: restartDialog
            parent: overlay
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

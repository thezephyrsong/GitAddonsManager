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
import GitAddonsManager.engine
import QtQuick.Controls
import QtQuick.Layouts

Page {
    title: "Addons"
    header: ToolBar {
        Layout.fillWidth: true
        RowLayout {
            anchors.fill: parent
            ToolButton {
                hoverEnabled: true
                action: addAction
                ToolTip.visible: hovered
                ToolTip.text: "Download a new addon from a git repository"
                //Material.foreground: Material.background
            }
            ToolButton {
                hoverEnabled: true
                action: updateAllAction
                icon.name: availableUpdates > 0 ? "update-high" : "update-none"
                ToolTip.visible: hovered
                ToolTip.text: "Upgrade all addons"
                //Material.foreground: Material.background
            }

            Item {
                Layout.fillWidth: true
            }

            ToolButton {
                hoverEnabled: true
                Layout.alignment: Qt.AlignRight
                action: refreshAction
                ToolTip.visible: hovered
                ToolTip.text: "Refresh addons list and check for updates"
                //Material.foreground: Material.background
            }
        }
    }
    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 4
        spacing: 0
        ListView {
            ScrollBar.vertical: scrollBar
            clip: true
            Layout.fillHeight: true
            Layout.fillWidth: true
            model: Engine.addons
            id: listView

            section.property: "path"
            section.criteria: ViewSection.FullString
            section.delegate:
                Button {
                    text: section
                    icon.name: "go-parent-folder"
                    width: Math.min(listView.width, implicitWidth)
                    onClicked: Qt.openUrlExternally((Qt.platform.os == "windows" ? "file:///" : "file://")+section)
                    display: AbstractButton.TextBesideIcon
                }

           delegate: MouseArea {
               property Addon addon: Engine.addons[index]
                implicitWidth: listView.width
                implicitHeight: row.height
                hoverEnabled: true
                id: ma
                Rectangle {
                    anchors.fill: row
                    color: "#ff8000"
                    visible:  addon.status === Addon.Status.Error || addon.gitStatus === Addon.Error
                }

                RowLayout {
                    width: parent.width
                    id: row
                    Rectangle {
                        visible: ma.containsMouse || addonMenu.visible || branchSelector.popup.visible
                        Layout.fillHeight: true
                        width: 4
                        color: deleteButton.hovered ? "red" : "lightgreen"
                    }
                    Label {
                        id: addonName
                        text: addon.name
                    }
                    Item {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        ProgressBar {
                            anchors.fill: parent
                            anchors.margins: 4
                            id: pBar
                            from: 0
                            to: addon.total
                            value: addon.progress
                            visible: addon.status === Addon.Status.Busy
                            indeterminate: to == 0
                        }
                    }
                    ComboBox {
                        id: branchSelector
                        model: addon.branches
                        onModelChanged: currentIndex = find(addon.currentBranch)
                        Component.onCompleted: currentIndex = find(addon.currentBranch)
                        onActivated: addon.currentBranch = currentText
                    }
                        RowLayout {
                            spacing: 0
                            AddonUpdateButton {
                            }
                            ToolButton {
                                icon.name: "overflow-menu"
                                onClicked: if (!addonMenu.visible) addonMenu.visible = true
                                checked: addonMenu.visible
                                Menu {
                                    x: -width
                                    id: addonMenu
                                    visible: false
                                    MenuItem {
                                        id: readmeButton
                                        text: qsTr("readme")
                                        enabled: addon.readme !== ""
                                        icon.name: "help-about"
                                        onTriggered: {
                                            readmeDialog.addon = addon
                                            readmeDialog.visible = true
                                        }
                                        hoverEnabled: true
                                    }

                                    MenuItem {
                                        id: repairButton
                                        hoverEnabled: true
                                        state: addon.gitStatus === Addon.Error ? "reclone" : "reset"
                                        states: [
                                            State {
                                                name: "reset"
                                                PropertyChanges {
                                                    enabled: addon.status == Addon.Status.Ready
                                                    target: repairButton
                                                    text: qsTr("repair")
                                                    icon.name: "document-edit-decrypt"
                                                    ToolTip.visible: hovered
                                                    ToolTip.text: "Restore addon files"
                                                    onTriggered: addon.reset()
                                                }
                                            },
                                            State {
                                                name: "reclone"
                                                PropertyChanges {
                                                    enabled: true
                                                    target: repairButton
                                                    text: qsTr("repair repository")
                                                    icon.name: "view-refresh"
                                                    ToolTip.visible: hovered
                                                    ToolTip.text: "Attempts to re-download the repository"
                                                    onTriggered: addon.reclone()
                                                }
                                            }
                                        ]
                                    }

                                    MenuItem {
                                        enabled: addon.status == Addon.Status.Ready
                                        id: deleteButton
                                        text: qsTr("delete")
                                        icon.name: "delete"
                                        onTriggered: addon.uninstall()
                                        hoverEnabled: true
                                    }
                                }
                            }
                        }
                }
                Dialog {
                    parent: window.overlay
                    x: (parent.width - width) / 2
                    y: (parent.height - height) / 2
                    title: qsTr("Following files will be deleted:")
                    standardButtons: Dialog.Ok | Dialog.Cancel
                    onAccepted: addon.confirmFileRemove(true)
                    onRejected: addon.confirmFileRemove(false)
                    modal: true
                    visible: addon.filesToRemove != ""
                    closePolicy: Popup.NoAutoClose
                    ScrollView{
                        anchors.fill: parent
                        TextArea {
                            text: addon.filesToRemove
                            readOnly: true
                            anchors.fill: parent
                            wrapMode: Text.NoWrap
                        }
                    }
                }
            }
        }
        ScrollBar {id: scrollBar; Layout.fillHeight: true; visible: listView.contentHeight > listView.header}
    }
    Dialog {
        property Addon addon
        id: readmeDialog
        parent: window.overlay
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        title: addon?qsTr("%1 Readme").arg(addon.name):""
        standardButtons: Dialog.Close
        ScrollView{
            anchors.fill: parent
            TextArea {
                text: readmeDialog.addon?readmeDialog.addon.readme:""
                readOnly: true
                anchors.fill: parent
                wrapMode: Text.NoWrap
                font.family: "Hack"
            }
        }
    }
}

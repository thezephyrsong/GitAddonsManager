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
import QtQuick.Layouts 1.3

ColumnLayout {

    ToolBar {
        Layout.fillWidth: true
        RowLayout {
            anchors.fill: parent
            ToolButton {
                hoverEnabled: true
                action: addAction
                ToolTip.visible: hovered
                ToolTip.text: "Download a new addon from a git repository"
            }
            ToolButton {
                hoverEnabled: true
                action: updateAllAction
                ToolTip.visible: hovered
                ToolTip.text: "Upgrade all addons"
            }
            ToolButton {
                hoverEnabled: true
                action: openAddonsFolder
                ToolTip.visible: hovered
                ToolTip.text: "Open addons folder externally"
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
            }
        }
    }
    RowLayout {
        Layout.fillHeight: true
        Layout.fillWidth: true
        spacing: 0
        ListView {
            ScrollBar.vertical: scrollBar
            clip: true
            Layout.fillHeight: true
            Layout.fillWidth: true
            model: Engine.addons
            id: listView

           delegate: MouseArea {
                property Addon addon: Engine.addons[index]
                width: parent.width
                implicitHeight: row.height
                hoverEnabled: true
                id: ma
                RowLayout {
                    spacing: 0
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
                        Layout.fillWidth: true
                        background: ProgressBar {
                            id: pBar
                            from: 0
                            to: addon.total
                            value: addon.progress
                            visible: addon.status == Addon.Status.Busy
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
                    AddonUpdateButton {
                        width: parent.width
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
                                enabled: addon.status == Addon.Status.Ready
                                id: deleteButton
                                text: qsTr("delete")
                                icon.name: "delete"
                                onTriggered: addon.uninstall()
                            }
                        }
                    }
                }
            }
        }
        ScrollBar {id: scrollBar; Layout.fillHeight: true; visible: listView.contentHeight > listView.header}
    }
}

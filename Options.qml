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


ColumnLayout {
    RowLayout {
        Layout.fillWidth: true
        TextField {
            Layout.fillWidth: true
            id: path
            text: Engine.addonsPath
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
            onClicked: fileDialog.visible = true
            hoverEnabled: true
            ToolTip.visible: hovered
            ToolTip.text: qsTr("Choose the folder containing the addons")
        }
    }
    CheckBox {
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
    RowLayout {
        Label {
            text: qsTr("Style")
        }
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

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
    Frame {
        Layout.fillWidth: true
        contentItem: RowLayout {
            width: parent.width
            TextField {
                Layout.fillWidth: true
                id: path
                text: Engine.addonsPath
                onAccepted: Engine.addonsPath = text
                placeholderText: qsTr("Path to AddOns folder")
                hoverEnabled: true
                ToolTip.visible: hovered
                ToolTip.text: qsTr("Path to the folder containing the addons")
            }
            Button {
                text: qsTr("Browse")
                onClicked: fileDialog.visible = true
                hoverEnabled: true
                ToolTip.visible: hovered
                ToolTip.text: qsTr("Choose the folder containing the addons")
            }
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
}

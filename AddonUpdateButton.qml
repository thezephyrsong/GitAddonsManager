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

ToolButton {
    id: upBtn
    enabled: addon.status === Addon.Ready && (addon.gitStatus !== Addon.UpToDate && addon.gitStatus !== Addon.Error)
    state: addon.gitStatus & ~Addon.MergeStatusMask
    onClicked: addon.update()
    hoverEnabled: true
    states: [
        State {
            name: Addon.UpToDate
            PropertyChanges {
                target: upBtn
                icon.name: "update-none"
                ToolTip.visible: hovered
                ToolTip.text: "Up to date"
            }
        },
        State {
            name: Addon.Ahead
            PropertyChanges {
                target: upBtn
                icon.name: "update-low"
                ToolTip.visible: hovered
                ToolTip.text: "Perform downgrade"
            }
        },
        State {
            name: Addon.Behind
            PropertyChanges {
                target: upBtn
                icon.name: "update-low"
                ToolTip.visible: hovered
                ToolTip.text: "Perform upgrade"
            }
        },
        State {
            name: Addon.Diverged
            PropertyChanges {
                target: upBtn
                icon.name: "update-high"
                ToolTip.visible: hovered
                ToolTip.text: "Perform checkout"
            }
        },
        State {
            name: Addon.Error
            PropertyChanges {
                target: upBtn
                icon.name: "update-high"
                ToolTip.visible: hovered
                ToolTip.text: "Error"
            }
        }
    ]
}

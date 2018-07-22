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
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.4

RowLayout {
    id: comp
    property alias text: label.text
    property alias model: colorBox.model
    property int currentIndex
    property color color: model.count>0?model.get(currentIndex).color:"transparent"
    Label {
        id: label
    }
    ComboBox {
        id: colorBox
        delegate: ItemDelegate{
            width: colorBox.width
            height: colorBox.height
            background: Rectangle {
                color: comp.model.get(index).color
            }
        }
        indicator: Item {}
        contentItem: Item {}
        background: Button {
            id: bg
            onClicked: colorBox.popup.visible = true
            contentItem: Rectangle {
                radius: height/2
                color: comp.color
            }
        }
        currentIndex: comp.currentIndex
        onActivated: comp.currentIndex = currentIndex
    }
}

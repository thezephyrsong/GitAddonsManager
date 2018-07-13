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

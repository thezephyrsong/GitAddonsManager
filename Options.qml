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

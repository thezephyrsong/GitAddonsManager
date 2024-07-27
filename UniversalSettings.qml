import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Universal
import GitAddonsManager.engine
import QtCore

StyleSettingsBase {

    ComboBox {
        id: universalTheme
        visible: Engine.style == "Universal"
        model: ["Light", "Dark", "System"]
        onActivated: window.Universal.theme = currentIndex
    }
    ListModel {
        id: universalColors
        dynamicRoles: true
        Component.onCompleted: {
            for (var i = 0; i < 20; i++)
                append({"color": Universal.color(i)})}
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
        category: "Universal"
        property alias accent: universalAccent.currentIndex
        property alias theme: universalTheme.currentIndex
    }
}

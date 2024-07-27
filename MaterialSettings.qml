import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import GitAddonsManager.engine
import QtCore

StyleSettingsBase {

    ComboBox {
        id: materialTheme
        visible: Engine.style == "Material"
        model: ["Light", "Dark"]
        onCurrentIndexChanged: window.Material.theme = currentIndex
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
    ListModel {
        id: materialDarkColors
        dynamicRoles: true
        Component.onCompleted: {
            for (var i = 0; i < 19; i++)
                append({"color": Material.color(i,Material.Shade200)})}
    }
    ListModel {
        id: materialColors
        dynamicRoles: true
        Component.onCompleted: {
            for (var i = 0; i < 19; i++)
                append({"color": Material.color(i)})}
    }

    Settings {
        category: "Material"
        property alias primary: materialPrimary.currentIndex
        property alias accent: materialAccent.currentIndex
        property alias theme: materialTheme.currentIndex
    }
}

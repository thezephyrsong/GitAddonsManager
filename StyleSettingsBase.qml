import QtQuick
import QtQuick.Controls
import GitAddonsManager.engine

Flow {
    spacing: 4
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

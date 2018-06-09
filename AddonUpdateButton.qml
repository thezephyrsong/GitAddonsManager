import QtQuick 2.9
import GitAddonsManager.engine 1.0
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3

ToolButton {
    id: upBtn
    enabled: addon.status == Addon.Ready && (addon.gitStatus != Addon.UpToDate && addon.gitStatus != Addon.Error)
    state: addon.gitStatus & ~Addon.MergeStatusMask
    icon.color: enabled ? "transparent" : "lightgray"
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

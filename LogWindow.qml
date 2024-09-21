import QtQuick
import GitAddonsManager.engine
import QtQuick.Controls

ApplicationWindow {
    id: logWindow
    visible: false
    onVisibleChanged: area.cursorPosition = area.text.length
    ScrollView
    {
        id: view
        Flickable
        {
            id: flick
            TextArea.flickable: TextArea
            {
                id: area
                readOnly: true
                textFormat: TextEdit.RichText
                Connections
                {
                    target: Engine
                    function onLogMessage(a)
                    {
                        area.append(a)
                        flick.contentY = flick.contentHeight - flick.height
                        flick.contentX = 0
                    }
                }
            }
        }
    
        anchors.fill: parent
    }
}

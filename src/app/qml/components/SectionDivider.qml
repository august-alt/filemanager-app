import QtQuick 2.4
import Ubuntu.Components 1.3

Item {
    id: rootItem

    property alias text: sectionLabel.text
    property alias iconName: icon.name

    anchors { left: parent.left; right: parent.right }
    height: units.gu(4)

    Row {
        anchors {
            left: parent.left
            right: parent.right
            margins: units.gu(2)
            verticalCenter: parent.verticalCenter
        }
        spacing: units.gu(1)

        Icon {
            id: icon
            height: units.gu(2)
            width: name ? units.gu(2) : 0
            anchors.verticalCenter: parent.verticalCenter
        }

        Label {
            id: sectionLabel
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    Rectangle {
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        height: units.dp(1)
        color: "#cdcdcd"
    }
}

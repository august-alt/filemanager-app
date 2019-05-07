import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3

import "../components" as Components
import "../actions" as FMActions


PageHeader {
    id: rootItem

    property bool __actionsEnabled: (selectionManager.counter > 0) || (folderSelectorMode && folderModel.model.isWritable)
    property bool __actionsVisible: selectionMode

    property var folderModel
    property var selectionManager: folderModel.model.selectionObject
    property var fileOperationDialog
    title: FmUtils.basename(folderModel.path)

    contents: ListItemLayout {
        anchors.verticalCenter: parent.verticalCenter
        subtitle.text: rootItem.title
        title.text: i18n.tr("%1 item selected", "%1 items selected", folderModel.model.selectionObject.counter).arg(folderModel.model.selectionObject.counter)
    }

    extension: Components.PathHistoryRow {
        folderModel: rootItem.folderModel
    }

    leadingActionBar.actions: Action {
        text: i18n.tr("Cancel")
        iconName: "close"
        onTriggered: {
            console.log("FileSelector cancelled")
            selectionManager.clear()
            fileSelectorMode = false
            fileSelector.fileSelectorComponent = null
        }
    }

    trailingActionBar.numberOfSlots: 4
    trailingActionBar.anchors.rightMargin: 0
    trailingActionBar.delegate: Components.TextualButtonStyle {}
    trailingActionBar.actions: [
        FMActions.SelectUnselectAll {
            selectedAll: selectionManager.selectedAll
            onTriggered: {
                if (selectionManager.selectedAll) {
                    selectionManager.clear()
                } else {
                    selectionManager.selectAll()
                }
            }
        },

        Action {
            property bool smallText: true
            iconName: "edit-delete"
            enabled: __actionsEnabled
            visible: __actionsVisible && folderModel.model.isWritable
            onTriggered: {
                var selectedAbsPaths = selectionManager.selectedAbsFilePaths();

                var props = {
                    "paths" : selectedAbsPaths,
                    "folderModel": folderModel.model,
                    "fileOperationDialog": fileOperationDialog
                }

                PopupUtils.open(Qt.resolvedUrl("../dialogs/ConfirmMultipleDeleteDialog.qml"), mainView, props)
            }
        },

        Action {
            property bool smallText: true
            iconName: "edit-copy"
            enabled: __actionsEnabled
            visible: __actionsVisible
            onTriggered: {
                var selectedAbsPaths = selectionManager.selectedAbsFilePaths();
                folderModel.model.copyPaths(selectedAbsPaths)
                selectionManager.clear()
                fileSelectorMode = false
                fileSelector.fileSelectorComponent = null
            }
        },

        Action {
            property bool smallText: true
            iconName: "edit-cut"
            enabled: __actionsEnabled
            visible: __actionsVisible && folderModel.model.isWritable
            onTriggered: {
                var selectedAbsPaths = selectionManager.selectedAbsFilePaths();
                folderModel.model.cutPaths(selectedAbsPaths)
                selectionManager.clear()
                fileSelectorMode = false
                fileSelector.fileSelectorComponent = null
            }
        }
    ]


    // *** STYLE HINTS ***

    StyleHints { dividerColor: "transparent" }
}

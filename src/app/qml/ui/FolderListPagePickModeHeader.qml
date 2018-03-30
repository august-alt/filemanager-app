import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3

import "../components" as Components
import "../actions" as FMActions
import "../backend"

PageHeader {
    id: rootItem

    // temp
    property FolderListModel folderModel
    property var selectionManager: folderModel.model.selectionObject

    title: FmUtils.basename(folderModel.path)

    contents: ListItemLayout {
        anchors.verticalCenter: parent.verticalCenter
        title.text: rootItem.title
        subtitle.text: !importMode ? i18n.tr("%1 item", "%1 items", folderModel.count).arg(folderModel.count) : i18n.tr("Save here")
    }

    extension: Components.PathHistoryRow {
        folderModel: rootItem.folderModel
    }

    FMActions.GoBack {
        id: goBackAction
        onTriggered: lastPos = folderModel.goBack()
    }

    FMActions.PlacesBookmarks {
        id: placesBookmarkAction
        onTriggered: {
            var pp = pageStack.push(Qt.resolvedUrl("PlacesPage.qml"), { folderModel: rootItem.folderModel })
            pp.pathClicked.connect(function() {
                pp.pageStack.pop()
            })
        }
    }

    leadingActionBar.actions: showPanelAction.visible ? showPanelAction : placesBookmarkAction

    trailingActionBar {
        anchors.rightMargin: 0
        delegate: Components.TextualButtonStyle {}

        actions: [
            Action {
                text: i18n.tr("Cancel")
                iconName: "close"
                onTriggered: {
                    console.log("FileSelector cancelled")
                    cancelFileSelector()
                }
            },

            Action {
                text: i18n.tr("Select")
                iconName: "tick"
                enabled: selectionManager.counter > 0 || importMode
                onTriggered: {
                    var selectedAbsUrls = []
                    if (folderSelectorMode) {
                        selectedAbsUrls = [ folderModel.path ]
                    } else {
                        var selectedAbsPaths = selectionManager.selectedAbsFilePaths();
                        // For now support only selection in filesystem
                        selectedAbsUrls = selectedAbsPaths.map(function(item) {
                            return "file://" + item;
                        });
                    }
                    console.log("FileSelector OK clicked, selected items: " + selectedAbsUrls)
                    acceptFileSelector(selectedAbsUrls)
                }
            }
        ]
    }


    // *** STYLE HINTS ***

    StyleHints { dividerColor: "transparent" }
}


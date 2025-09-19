import QtQuick
import QtQuick.Controls
import StyleSheet

ScrollView {
    id: root
    width: parent.width
    height: parent.height
    contentHeight: contentCol.implicitHeight + 20

    anchors.leftMargin: 10
    anchors.rightMargin: 10
    anchors.topMargin: 10
    property var mrpa: mrpaData

    signal closeClicked

    function isObj(x) {
        return x && typeof x === "object" && !Array.isArray(x)
    }
    function isArr(x) {
        return Array.isArray(x)
    }
    function attrKeys(o) {
        return isObj(o) ? Object.keys(o).filter(k => k.startsWith("@")) : []
    }
    function secKeys(o) {
        return isObj(o) ? Object.keys(o).filter(k => !k.startsWith("@")) : []
    }

    function lastTitle(path) {
        if (!path || path.length === 0)
            return qsTr("Доверенность")
        return path[path.length - 1]
    }

    function buildSections(node, path, out) {
        if (!isObj(node) && !isArr(node))
            return

        if (isObj(node)) {
            const title = lastTitle(path)
            const attrs = attrKeys(node).map(k => ({
                                                       "key": k,
                                                       "value": node[k]
                                                   }))
            if (attrs.length > 0 || path.length === 0) {
                out.push({
                             "title": title,
                             "attrs": attrs
                         })
            }
            const children = secKeys(node)
            for (var i = 0; i < children.length; ++i) {
                const key = children[i]
                buildSections(node[key], path.concat([key]), out)
            }
            return
        }

        const last = lastTitle(path)
        for (var i = 0; i < node.length; ++i) {
            const indexed = (last || "(array)") + " [" + i + "]"
            const nextPath = path.slice(0, -1).concat([indexed])
            buildSections(node[i], nextPath, out)
        }
    }

    function computeSections() {
        const res = []
        if (mrpa)
            buildSections(mrpa, [], res)
        return res
    }

    property var sections: computeSections()
    onMrpaChanged: sections = computeSections()

    Item {
        width: parent.width
        height: 40
        ToolButton {
            flat: true
            display: AbstractButton.IconOnly
            icon.source: StyleSheet.close_icon
            icon.width: 20
            icon.height: 20
            icon.color: StyleSheet.font_color
            leftPadding: 0
            topPadding: 0
            rightPadding: 0
            bottomPadding: 0
            anchors.top: parent.top
            anchors.left: parent.left
            width: 20
            height: 20

            onClicked: {
                rightSideBar.showState = RightSideBar.ShowState.Mrpa
            }
        }

        RSBCloseButton {
            onClicked: root.closeClicked()
        }
    }

    Column {
        id: contentCol
        width: parent.width
        spacing: 10

        Repeater {
            model: sections
            delegate: Column {
                width: parent.width
                spacing: 6

                Text {
                    text: modelData.title
                    font.weight: Font.DemiBold
                    font.family: "Noto Sans"
                    color: StyleSheet.font_color_extra
                }

                Repeater {
                    model: modelData.attrs
                    delegate: TextPair {
                        keyText: modelData.key
                        value: String(modelData.value)
                    }
                }

                RightSBHorizontalDelimiter {
                    width: parent.width
                }
            }
        }
    }
}

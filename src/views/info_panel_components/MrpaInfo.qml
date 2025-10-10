import QtQuick
import QtQuick.Controls
import StyleSheet

Item {
    id: root
    width: parent.width
    height: parent.height

    property var mrpa: mrpaData
    signal closeClicked
    signal backClicked

    function isObj(x) {
        return x && typeof x === "object" && !Array.isArray(x)
    }
    function isArr(x) {
        return Array.isArray(x)
    }
    function attrKeys(o) {
        return isObj(o) ? Object.keys(o).filter(k => k.startsWith("@")
                                                || k === "text") : []
    }
    function secKeys(o) {
        return isObj(o) ? Object.keys(o).filter(k => !k.startsWith("@")
                                                && k !== "text") : []
    }
    function mrpaTr(str) {
        return surguchTranslator.surguchTranslate(str)
    }
    function lastTitle(path) {
        if (!path || path.length === 0)
            return qsTr("About MRPA")
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
        for (var j = 0; j < node.length; ++j) {
            const indexed = (last || "(array)") + " [" + j + "]"
            const nextPath = path.slice(0, -1).concat([indexed])
            buildSections(node[j], nextPath, out)
        }
    }
    function computeSections() {
        const res = []
        if (mrpa)
            buildSections(mrpa, [], res)
        return res
    }

    function isNumeric(value) {
        return /^\d+$/.test(value)
    }

    property var sections: computeSections()
    onMrpaChanged: sections = computeSections()

    Item {
        id: mrpaHeader
        width: parent.width
        height: 40
        anchors.top: parent.top
        z: 2

        ToolButton {
            flat: true
            display: AbstractButton.IconOnly
            icon.source: StyleSheet.back_icon
            icon.width: 20
            icon.height: 20
            icon.color: StyleSheet.font_color
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.margins: 5
            width: 20
            height: 20
            onClicked: {
                root.backClicked()
            }
        }

        RSBCloseButton {
            anchors.topMargin: 5
            anchors.rightMargin: 15
            onClicked: root.closeClicked()
        }
    }

    Flickable {
        id: flick
        anchors {
            top: mrpaHeader.bottom
            bottom: parent.bottom
            left: parent.left
            right: parent.right
            leftMargin: 10
            rightMargin: 10
            topMargin: 10
        }
        clip: true
        contentWidth: width
        contentHeight: contentCol.implicitHeight + 20
        z: 1

        Column {
            id: contentCol
            width: flick.width
            spacing: 10

            Repeater {
                model: sections
                delegate: Column {
                    width: parent.width - 15
                    spacing: 6

                    Text {
                        text: mrpaTr(modelData.title)
                        width: parent.width
                        horizontalAlignment: Text.AlignHCenter
                        wrapMode: Text.WordWrap
                        leftPadding: 12
                        rightPadding: 12
                        topPadding: 4
                        bottomPadding: 2
                        font.weight: Font.DemiBold
                        font.family: "Noto Sans"
                        font.pixelSize: 12
                        color: StyleSheet.font_color_extra
                        visible: String(modelData.title).trim(
                                     ) === "ПодпИзобр" ? false : true
                    }

                    Repeater {
                        model: String(modelData.title).trim(
                                   ) === "ПодпИзобр" ? "" : modelData.attrs

                        delegate: TextPair {
                            width: parent.width
                            keyText: {
                                let name = mrpaTr(String(
                                                      modelData.key).slice(1))
                                if (name === "ext")
                                    return ""
                                return name
                            }
                            value: String(modelData.value)
                            isMrpa: true
                        }
                    }

                    RightSBHorizontalDelimiter {
                        width: parent.width
                        visible: String(modelData.title).trim(
                                     ) === "ПодпИзобр" ? false : true
                    }
                }
            }
        }

        ScrollBar.vertical: ScrollBar {
            id: mrpaScroll
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.rightMargin: 4
            width: 8
            policy: ScrollBar.AlwaysOn

            contentItem: Rectangle {
                implicitWidth: 6
                radius: width / 2
                color: StyleSheet.slider_fill_color
                border.width: 2
                border.color: StyleSheet.slider_border_color
            }
            background: Rectangle {
                color: "transparent"
            }
        }
    }
}

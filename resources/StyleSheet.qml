pragma Singleton

import QtQuick

Item {
    id: style_sheet
    state: "light"

    property color font_color
    property color font_color_extra
    property string arrow_back_icon
    property string arrow_forward_icon
    property string arrow_down_icon
    property string arrow_up_icon
    property string close_icon
    property string bell_icon
    property string book_icon
    property string file_plus_icon
    property string file_simple_icon
    property string folder_plus_icon
    property string fullscreen_icon
    property string medal_icon
    property string medal_green_icon
    property string medal_pink_icon
    property string minus_circle_icon
    property string pen_tool_icon
    property string pencil_line_icon
    property string plus_circle_icon
    property string printer_icon
    property string search_icon
    property string wrench_icon
    property string save_icon
    property string trash_icon
    property color slider_fill_color
    property color slider_border_color
    property color slider_handle_color
    states: [
        State {
            name: "dark"
            PropertyChanges {
                target: style_sheet
                font_color: "black"
                font_color_extra: "white"
                arrow_back_icon: "qrc:/icons/dark/arrow_back_curve.svg"
                arrow_forward_icon: "qrc:/icons/dark/arrow_forward_curve.svg"
                arrow_down_icon: "qrc:/icons/dark/arrow-circle-down.svg"
                arrow_up_icon: "qrc:/icons/dark/arrow-circle-up.svg"
                close_icon: "qrc:/icons/dark/close_icon.svg"
                bell_icon: "qrc:/icons/dark/bell.svg"
                book_icon: "qrc:/icons/dark/book.svg"
                file_plus_icon: "qrc:/icons/dark/file_plus.svg"
                file_simple_icon: "qrc:/icons/dark/file_simple.svg"
                folder_plus_icon: "qrc:/icons/dark/folder_plus.svg"
                fullscreen_icon: "qrc:/icons/dark/fullscreen-custom.svg"
                medal_icon: "qrc:/icons/dark/medal-ribbon.svg"
                medal_green_icon: "qrc:/icons/dark/medal-ribbon-green.svg"
                medal_pink_icon: "qrc:/icons/dark/medal-ribbon-pink.svg"
                minus_circle_icon: "qrc:/icons/dark/minus-circle.svg"
                pen_tool_icon: "qrc:/icons/dark/pen_tool.svg"
                pencil_line_icon: "qrc:/icons/dark/pencil_line.svg"
                plus_circle_icon: "qrc:/icons/dark/plus-circle.svg"
                printer_icon: "qrc:/icons/dark/printer_sm.svg"
                search_icon: "qrc:/icons/dark/search-custom.svg"
                wrench_icon: "qrc:/icons/dark/wrench.svg"
                trash_icon: "qrc:/icons/dark/trash.svg"
                save_icon: "qrc:/icons/dark/save.svg"
                slider_fill_color: "#454A4E"
                slider_border_color: "#454A4E"
                slider_handle_color: "#31363B"
            }
        },
        State {
            name: "light"
            PropertyChanges {
                target: style_sheet
                font_color: "black"
                font_color_extra: "black"
                arrow_back_icon: "qrc:/icons/light/arrow_back_curve.svg"
                arrow_forward_icon: "qrc:/icons/light/arrow_forward_curve.svg"
                arrow_down_icon: "qrc:/icons/light/arrow-circle-down.svg"
                arrow_up_icon: "qrc:/icons/light/arrow-circle-up.svg"
                close_icon: "qrc:/icons/light/close_icon.svg"
                bell_icon: "qrc:/icons/light/bell.svg"
                book_icon: "qrc:/icons/light/book.svg"
                file_plus_icon: "qrc:/icons/light/file_plus.svg"
                file_simple_icon: "qrc:/icons/light/file_simple.svg"
                folder_plus_icon: "qrc:/icons/light/folder_plus.svg"
                fullscreen_icon: "qrc:/icons/light/fullscreen-custom.svg"
                medal_icon: "qrc:/icons/light/medal-ribbon.svg"
                medal_green_icon: "qrc:/icons/light/medal-ribbon-green.svg"
                medal_pink_icon: "qrc:/icons/light/medal-ribbon-pink.svg"
                minus_circle_icon: "qrc:/icons/light/minus-circle.svg"
                pen_tool_icon: "qrc:/icons/light/pen_tool.svg"
                pencil_line_icon: "qrc:/icons/light/pencil_line.svg"
                plus_circle_icon: "qrc:/icons/light/plus-circle.svg"
                printer_icon: "qrc:/icons/light/printer_sm.svg"
                search_icon: "qrc:/icons/light/search-custom.svg"
                wrench_icon: "qrc:/icons/light/wrench.svg"
                trash_icon: "qrc:/icons/light/trash.svg"
                save_icon: "qrc:/icons/light/save.svg"
                slider_fill_color: "#DADBDC"
                slider_border_color: "#DADBDC"
                slider_handle_color: "#EFF0F1"
            }
        }
    ]
}

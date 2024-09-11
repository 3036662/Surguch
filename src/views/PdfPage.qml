import QtQuick
import QtQuick.Controls

Rectangle{
    id: pdfpage
    property string text

    Text{
      text: pdfpage.text
    }
}

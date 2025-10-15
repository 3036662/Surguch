/* File: t_guiutils.cpp
Copyright (C) Basealt LLC,  2025
Author: Daniil-Viktor Ratkin, <ratkinda@basealt.ru>

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "t_guiutils.hpp"

#include <QFile>
#include <QFuture>
#include <QFutureWatcher>
#include <QImage>
#include <QJsonDocument>
#include <QJsonObject>
#include <QQuickItem>
#include <QSignalSpy>
#include <QStandardPaths>
#include <QTest>
#include <QVariant>
#include <fstream>

#include "core/csp_c_bridge/bridge_utils.hpp"
#include "cpp_views/rubber_preview_render.hpp"
#include "gui_core/gui_utils.hpp"
#include "gui_core/rubber_structs.hpp"
#include "models/rubber_stamp_model.hpp"
#include "pdf_csp_c.hpp"

TGuiUtils::TGuiUtils(QObject *parent) : QObject{parent} {}

void TGuiUtils::test_image() {
    const QString str1 =
        QString(
            R"( {"stamp_width":900,"stamp_height":300,"create_from_image":0,"img_path":"})") +
        TEST_FILES_DIR +
        R"({tag_5_logo.jpg","border_width":7,"border_radius":50,"text_color_red":50,"text_color_green":62,"text_color_blue":168,"border_color_red":50,"border_color_green":62,"border_color_blue":168,"bg_color_red":50,"bg_color_green":62,"bg_color_blue":168,"font_family":"Noto Sans","annotation_text":"","bg_transparent":0,"annotation_width":300})";
    QJsonParseError parse_error1;
    QJsonDocument json_doc1 =
        QJsonDocument::fromJson(str1.toUtf8(), &parse_error1);
    // qWarning() << "JSON parse error:" << parse_error.errorString();
    QVERIFY(parse_error1.error == QJsonParseError::NoError);
    const QString str2 =
        QString(
            R"( {"stamp_width":900,"stamp_height":300,"create_from_image":0,"img_path":"})") +
        TEST_FILES_DIR +
        R"({tag_5_logo.jpg","border_width":7,"border_radius":50,"text_color_red":50,"text_color_green":62,"text_color_blue":168,"border_color_red":50,"border_color_green":62,"border_color_blue":168,"bg_color_red":50,"bg_color_green":62,"bg_color_blue":168,"font_family":"Noto Sans","annotation_text":"Сургуч","bg_transparent":1,"annotation_width":300})";
    QJsonParseError parse_error2;
    QJsonDocument json_doc2 =
        QJsonDocument::fromJson(str2.toUtf8(), &parse_error2);
    // qWarning() << "JSON parse error:" << parse_error.errorString();
    QVERIFY(parse_error2.error == QJsonParseError::NoError);
    const QString str3 =
        QString(
            R"( {"stamp_width":900,"stamp_height":300,"create_from_image":1,"img_path":"})") +
        TEST_FILES_DIR +
        R"({tag_5_logo.jpg","border_width":7,"border_radius":50,"text_color_red":50,"text_color_green":62,"text_color_blue":168,"border_color_red":50,"border_color_green":62,"border_color_blue":168,"bg_color_red":50,"bg_color_green":62,"bg_color_blue":168,"font_family":"Noto Sans","annotation_text":"Сургуч","bg_transparent":0,"annotation_width":300})";
    QJsonParseError parse_error3;
    QJsonDocument json_doc3 =
        QJsonDocument::fromJson(str3.toUtf8(), &parse_error3);
    // qWarning() << "JSON parse error:" << parse_error.errorString();
    QVERIFY(parse_error3.error == QJsonParseError::NoError);

    const QString str4 = R"( {})";
    QJsonParseError parse_error4;
    QJsonDocument json_doc4 =
        QJsonDocument::fromJson(str4.toUtf8(), &parse_error4);
    // qWarning() << "JSON parse error:" << parse_error.errorString();
    QVERIFY(parse_error4.error == QJsonParseError::NoError);

    QJsonObject json_obj1 = json_doc1.object();
    QVariantMap varmap1 = json_obj1.toVariantMap();
    auto pre_params1 = core::gui::prepareParams(varmap1);
    auto params1 = core::gui::createParams(pre_params1);
    auto tag1 = core::gui::prepareImage(params1);
    QVERIFY(tag1 != nullptr);

    QJsonObject json_obj2 = json_doc2.object();
    QVariantMap varmap2 = json_obj2.toVariantMap();
    auto pre_params2 = core::gui::prepareParams(varmap2);
    auto params2 = core::gui::createParams(pre_params2);
    auto tag2 = core::gui::prepareImage(params2);
    QVERIFY(tag2 != nullptr);

    QJsonObject json_obj3 = json_doc3.object();
    QVariantMap varmap3 = json_obj3.toVariantMap();
    auto pre_params3 = core::gui::prepareParams(varmap3);
    auto params3 = core::gui::createParams(pre_params3);
    auto tag3 = core::gui::prepareImage(params3);
    QVERIFY(tag3 != nullptr);

    QJsonObject json_obj4 = json_doc4.object();
    QVariantMap varmap4 = json_obj4.toVariantMap();
    auto pre_params4 = core::gui::prepareParams(varmap4);
    auto params4 = core::gui::createParams(pre_params4);
    auto tag4 = core::gui::prepareImage(params4);
    QVERIFY(tag4 != nullptr);
}

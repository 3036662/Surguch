/* File: t_rubber.cpp
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

#include "t_rubber.hpp"

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

TRubber::TRubber(QObject *parent) : QObject{parent} {}

void TRubber::createRubber1() {
    qWarning() << "section 1";
    const QString json = R"( {})";
    QJsonParseError parse_error;
    QJsonDocument json_doc =
        QJsonDocument::fromJson(json.toUtf8(), &parse_error);
    // qWarning() << "JSON parse error:" << parse_error.errorString();
    QVERIFY(parse_error.error == QJsonParseError::NoError);

    QJsonObject json_obj = json_doc.object();
    QVariantMap varmap = json_obj.toVariantMap();
    RubberPreviewRender renderer;
    QSignalSpy spy(&renderer, &RubberPreviewRender::rubberBadResult);

    renderer.createImage(varmap);
    QTest::qWait(500);
    QCOMPARE(spy.count(), 1);
}

void TRubber::createRubber2() {
    qWarning() << "section 2";
    const QString json =
        QString(
            R"( {"stamp_width":900,"stamp_height":300,"create_from_image":1,"img_path":")") +
        TEST_FILES_DIR +
        R"(tag_5_logo.jpg","border_width":7,"border_radius":50,"text_color_red":50,"text_color_green":62,"text_color_blue":168,"border_color_red":50,"border_color_green":62,"border_color_blue":168,"bg_color_red":50,"bg_color_green":62,"bg_color_blue":168,"font_family":"Noto Sans","annotation_text":"Сургуч","bg_transparent":0,"annotation_width":300})";
    QJsonParseError parse_error;
    QJsonDocument json_doc =
        QJsonDocument::fromJson(json.toUtf8(), &parse_error);
    // qWarning() << "JSON parse error:" << parse_error.errorString();
    QVERIFY(parse_error.error == QJsonParseError::NoError);

    QJsonObject json_obj = json_doc.object();
    QVariantMap varmap = json_obj.toVariantMap();
    RubberPreviewRender renderer;
    QSignalSpy spy(&renderer, &RubberPreviewRender::rubberImageReady);

    renderer.createImage(varmap);
    QTest::qWait(500);
    QCOMPARE(spy.count(), 1);
}

void TRubber::createRubber3() {
    qWarning() << "section 3";
    const QString json =
        QString(
            R"( {"stamp_width":900,"stamp_height":300,"create_from_image":0,"img_path":"})") +
        TEST_FILES_DIR +
        R"({tag_5_logo.jpg","border_width":7,"border_radius":50,"text_color_red":50,"text_color_green":62,"text_color_blue":168,"border_color_red":50,"border_color_green":62,"border_color_blue":168,"bg_color_red":50,"bg_color_green":62,"bg_color_blue":168,"font_family":"Noto Sans","annotation_text":"Сургуч","bg_transparent":0,"annotation_width":300})";
    QJsonParseError parse_error;
    QJsonDocument json_doc =
        QJsonDocument::fromJson(json.toUtf8(), &parse_error);
    // qWarning() << "JSON parse error:" << parse_error.errorString();
    QVERIFY(parse_error.error == QJsonParseError::NoError);

    QJsonObject json_obj = json_doc.object();
    QVariantMap varmap = json_obj.toVariantMap();
    RubberPreviewRender renderer;
    QSignalSpy spy(&renderer, &RubberPreviewRender::rubberImageReady);

    renderer.createImage(varmap);
    QTest::qWait(500);
    QCOMPARE(spy.count(), 1);
}

/* File: main.cpp
Copyright (C) Basealt LLC,  2024-2025
Author: Oleg Proskurin, <proskurinov@basealt.ru>

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

#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QDirIterator>
#include <QGuiApplication>
#include <QIcon>
#include <QLocale>
#include <QPalette>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QTranslator>
#include <QtConcurrent>

#include "core/font_helper.hpp"
#include "core/gui_core/custom_event_filter.hpp"
#include "core/signature_creator.hpp"
#include "core/tag_creator.hpp"
#include "cpp_views/pdf_page_render.hpp"
#include "cpp_views/preview_render.hpp"
#include "cpp_views/rubber_preview_render.hpp"
#include "models/file_tree_model.hpp"
#include "models/pdf_doc_model.hpp"
#include "models/profiles_model.hpp"
#include "models/rubber_stamp_model.hpp"
#include "models/signatures_list_model.hpp"
#include "printer_launcher.hpp"
#include "surguch_launcher.hpp"
#include "surguch_translator.hpp"

int main(int argc, char* argv[]) {
    // translation
    QTranslator translator;
    const QString locale = QLocale::system().name();
    QApplication app(argc, argv);

    QCommandLineParser parser;
    QCommandLineOption fileOption("f");
    parser.addOption(fileOption);

    QGuiApplication::setWindowIcon(QIcon(":/app_icons/SealWax-1_32.png"));
    const QString translation_path = ":/translations/" + locale + ".qm";
    if (!translator.load(translation_path)) {
        qWarning("Load translations failed");
    } else {
        QApplication::installTranslator(&translator);
    }

    // register types
    qmlRegisterType<PdfPageRender>("alt.pdfcsp.pdfRender", 0, 1,
                                   "PdfPageRender");
    qmlRegisterType<PreviewRender>("alt.pdfcsp.previewRender", 0, 1,
                                   "PreviewRender");
    qmlRegisterType<RubberPreviewRender>("alt.pdfcsp.rubberPreviewRender", 0, 1,
                                         "RubberPreviewRender");
    qmlRegisterType<PdfDocModel>("alt.pdfcsp.pdfModel", 0, 1, "MuPdfModel");
    qmlRegisterType<SignaturesListModel>("alt.pdfcsp.signaturesListModel", 0, 1,
                                         "SignaturesListModel");
    qmlRegisterType<ProfilesModel>("alt.pdfcsp.profilesModel", 0, 1,
                                   "ProfilesModel");
    qmlRegisterType<RubberStampModel>("alt.pdfcsp.rubberStampModel", 0, 1,
                                      "RubberStampModel");
    qmlRegisterType<core::SignatureCreator>("alt.pdfcsp.signatureCreator", 0, 1,
                                            "SignatureCreator");
    qmlRegisterType<core::TagCreator>("alt.pdfcsp.tagCreator", 0, 1,
                                      "TagCreator");
    qmlRegisterType<core::PrinterLauncher>("alt.pdfcsp.printerLauncher", 0, 1,
                                           "PrinterLauncher");
    qmlRegisterType<core::SurguchLauncher>("alt.pdfcsp.surguchLauncher", 0, 1,
                                           "SurguchLauncher");
    qmlRegisterType<FileTreeModel>("alt.pdfcsp.fileTreeModel", 0, 1,
                                   "FileTreeModel");
    qmlRegisterSingletonType(QUrl("qrc:/StyleSheet.qml"), "StyleSheet", 0, 1,
                             "StyleSheet");

    qmlRegisterType<core::FontHelper>("alt.pdfcsp.fontHelper", 0, 1,
                                      "FontHelper");

    qmlRegisterType<core::gui::WheelEventFilter>("alt.pdfcsp.wheelFilter", 1, 0,
                                                 "WheelFilter");
    qmlRegisterType<core::SurguchTranslator>("alt.pdfcsp.surguchTranslator", 1,
                                             0, "SurguchTranslator");
    // the eventFilterInstaller singleton is utilized to fix the Qt6.4 bug
    // when the wheel event is not propagated to the popup nested within a
    // combobox
    auto eventFilterInstaller =
        std::make_unique<core::gui::EventFilterInstaller>();
    qmlRegisterSingletonInstance<core::gui::EventFilterInstaller>(
        "alt.pdfcsp.eventFilterInstaller", 1, 0, "EventFilterInstaller",
        eventFilterInstaller.get());

    // run the app
    QQmlApplicationEngine engine;
    parser.process(app);
    const QStringList args = parser.positionalArguments();
    // tree files to open on start
    engine.rootContext()->setContextProperty(
        "openFiles", parser.isSet(fileOption) ? args : QStringList());
    // pdf file to open on start
    engine.rootContext()->setContextProperty("openOnStart",
                                             (!args.empty() ? args.at(0) : ""));
    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreationFailed, &app,
        []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);

    // determine the KDE version
    QString kde_version = "";
    QString theme_style = "";
    if (qEnvironmentVariable("XDG_CURRENT_DESKTOP") == "KDE") {
        kde_version = qEnvironmentVariable("KDE_SESSION_VERSION");
    }
    engine.rootContext()->setContextProperty("kdeVersion", kde_version);
    const QPalette defaultPalette;
    const auto text = defaultPalette.color(QPalette::WindowText);
    const auto window = defaultPalette.color(QPalette::Window);
    if (text.lightness() > window.lightness() && kde_version != "5") {
        theme_style = "dark";
    } else {
        theme_style = "light";
    }
    engine.rootContext()->setContextProperty("themeStyle", theme_style);

#if QT_LOAD_FROM_MODULE == 1
    engine.loadFromModule("gui_pdf_csp", "Main");
#else
    const QUrl url("qrc:/gui_pdf_csp/Main.qml");
    engine.load(url);
#endif

    // QDirIterator it(":", QDirIterator::Subdirectories);
    // while (it.hasNext()) {
    //     qWarning() << it.next();
    // }

    return QApplication::exec();
}

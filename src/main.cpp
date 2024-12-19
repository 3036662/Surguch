/* File: main.cpp
Copyright (C) Basealt LLC,  2024
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

#include "core/signature_creator.hpp"
#include "cpp_views/pdf_page_render.hpp"
#include "models/pdf_doc_model.hpp"
#include "models/profiles_model.hpp"
#include "models/signatures_list_model.hpp"
#include "printer_launcher.hpp"
#include <QApplication>
#include <QDebug>
#include <QDirIterator>
#include <QGuiApplication>
#include <QIcon>
#include <QLocale>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QTranslator>

int main(int argc, char *argv[]) {

  // tranlsation
  QTranslator translator;
  const QString locale = QLocale::system().name();
  QApplication app(argc, argv);

  QGuiApplication::setWindowIcon(QIcon(":/app_icons/SealWax-1_32.png"));
  const QString translation_path = ":/translations/" + locale + ".qm";
  if (!translator.load(translation_path)) {
    qWarning("Load translations failed");
  } else {
    QApplication::installTranslator(&translator);
  }

  // register types
  qmlRegisterType<PdfPageRender>("alt.pdfcsp.pdfRender", 0, 1, "PdfPageRender");
  qmlRegisterType<PdfDocModel>("alt.pdfcsp.pdfModel", 0, 1, "MuPdfModel");
  qmlRegisterType<SignaturesListModel>("alt.pdfcsp.signaturesListModel", 0, 1,
                                       "SignaturesListModel");
  qmlRegisterUncreatableType<core::DocStatusEnum>(
      "alt.pdfcsp.signaturesListModel", 0, 1, "DocStatusEnum", "C++ enum");
  qmlRegisterType<ProfilesModel>("alt.pdfcsp.profilesModel", 0, 1,
                                 "ProfilesModel");
  qmlRegisterType<core::SignatureCreator>("alt.pdfcsp.signatureCreator", 0, 1,
                                          "SignatureCreator");
  qmlRegisterType<core::PrinterLauncher>("alt.pdfcsp.printerLauncher", 0, 1,
                                         "PrinterLauncher");
  // run the app
  QQmlApplicationEngine engine;
  const QStringList args = QApplication::arguments();
  // file to open on start
  engine.rootContext()->setContextProperty("openOnStart",
                                           (args.size() > 1 ? args.at(1) : ""));
  QObject::connect(
      &engine, &QQmlApplicationEngine::objectCreationFailed, &app,
      []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);

  // determine the KDE version
  QString kde_version = "";
  if (qEnvironmentVariable("XDG_CURRENT_DESKTOP") == "KDE") {
    kde_version = qEnvironmentVariable("KDE_SESSION_VERSION");
  }
  engine.rootContext()->setContextProperty("kdeVersion", kde_version);

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

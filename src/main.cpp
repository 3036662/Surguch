#include "core/signature_creator.hpp"
#include "cpp_views/pdf_page_render.hpp"
#include "models/pdf_page_model.hpp"
#include "models/profiles_model.hpp"
#include "models/signatures_list_model.hpp"
#include "printer_launcher.hpp"
#include <QApplication>
#include <QDebug>
#include <QDirIterator>
#include <QGuiApplication>
#include <QLocale>
#include <QQmlApplicationEngine>
#include <QTranslator>

#include <QQmlContext>

#include <qpainter.h>

int main(int argc, char *argv[]) {

  QTranslator translator;
  QString locale = QLocale::system().name();
  QApplication app(argc, argv);
  const QString translation_path = ":/translations/" + locale + ".qm";
  if (!translator.load(translation_path)) {
    qWarning("Load translations failed");
  } else {
    app.installTranslator(&translator);
  }

  qmlRegisterType<PdfPageRender>("alt.pdfcsp.pdfRender", 0, 1, "PdfPageRender");
  qmlRegisterType<PdfPageModel>("alt.pdfcsp.pdfModel", 0, 1, "MuPdfModel");
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

  QQmlApplicationEngine engine;
  QStringList args = app.arguments();
  // file to open on start
  engine.rootContext()->setContextProperty("openOnStart",
                                           (args.size() > 1 ? args.at(1) : ""));

  QObject::connect(
      &engine, &QQmlApplicationEngine::objectCreationFailed, &app,
      []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);

#if QT_LOAD_FROM_MODULE == 1
  engine.loadFromModule("gui_pdf_csp", "Main");
  engine.addImportPath("qrc:/modules");

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

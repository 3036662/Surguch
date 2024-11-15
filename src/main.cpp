#include "core/signature_creator.hpp"
#include "cpp_views/pdf_page_render.hpp"
#include "models/pdf_page_model.hpp"
#include "models/profiles_model.hpp"
#include "models/signatures_list_model.hpp"
#include <QApplication>
#include <QDebug>
#include <QDirIterator>
#include <QGuiApplication>
#include <QLocale>
#include <QQmlApplicationEngine>
#include <QTranslator>

int main(int argc, char *argv[]) {

  QTranslator translator;
  QString locale = QLocale::system().name();
  QApplication app(argc, argv);
  const QString translation_path = QString(TRANSLATION_DIR) + locale + ".qm";
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

  QQmlApplicationEngine engine;

  QObject::connect(
      &engine, &QQmlApplicationEngine::objectCreationFailed, &app,
      []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);
  engine.loadFromModule("gui_pdf_csp", "Main");

  // QDirIterator it(":", QDirIterator::Subdirectories);
  // while (it.hasNext()) {
  //     qWarning() << it.next();
  // }

  return QApplication::exec();
}

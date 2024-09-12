#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QDirIterator>
#include <QDebug>
#include <QApplication>
#include "models/pdf_page_model.hpp"
#include "cpp_views/pdf_page_render.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    qmlRegisterType<PdfPageRender>("alt.pdfcsp.pdfRender",0,1,"PdfPageRender");
    qmlRegisterType<PdfPageModel>("alt.pdfcsp.pdfModel",0,1,"MuPdfModel");

    QQmlApplicationEngine engine;
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
                     &app, []() { QCoreApplication::exit(-1); },
    Qt::QueuedConnection);
    engine.loadFromModule("gui_pdf_csp", "Main");
    // QDirIterator it(":", QDirIterator::Subdirectories);
    // while (it.hasNext()) {
    //     qWarning() << it.next();
    // }

    return app.exec();
}

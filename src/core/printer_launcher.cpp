#include "printer_launcher.hpp"

#include <QFile>
#include <QFileInfo>
#include <QMargins>
#include <QPrintDialog>
#include <QPrintEngine>
#include <QPrinter>
#include <QProcess>
#include <QUrl>

namespace core {

PrinterLauncher::PrinterLauncher(QObject *parent) : QObject{parent} {}

void PrinterLauncher::print(QString src_file, int page_count, bool landscape) {
  QPrinter printer;
  QPrintDialog print_dialog(&printer, nullptr);
  QSize size_hint = print_dialog.minimumSizeHint();
  if (size_hint.isValid()) {
    print_dialog.resize(size_hint);
  }
  print_dialog.setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  qWarning() << "PAGE_COUNT " << page_count;
  print_dialog.setMinMax(1, page_count);
  print_dialog.setFromTo(1, page_count);
  print_dialog.setPrintRange(QAbstractPrintDialog::AllPages);
  print_dialog.setOption(QAbstractPrintDialog::PrintToFile, false);
  if (landscape) {
    qWarning("LANDSCAPE");
    printer.setPageOrientation(QPageLayout::Landscape);
  }
  // print_dialog.setLayout()
  //  print_dialog.setOption(QAbstractPrintDialog::PrintCurrentPage);
  if (print_dialog.exec() == QDialog::Accepted) {
    qWarning() << "Print...";
    // QPrintPreviewDialog preview_dialog(&printer);
    // preview_dialog.exec();
    // QPainter painter(&printer);
    // painter.setPen(Qt::blue);
    // painter.setFont(QFont("Arial", 30));
    // QPointF pos(10,10);
    // painter.drawText(pos, "SURGUCH PRINT TEST");
    // painter.end();
    qWarning() << "color mode" << printer.colorMode();
    qWarning() << "copyCount" << printer.copyCount();
    qWarning() << "creator " << printer.creator();
    qWarning() << "doc name " << printer.docName();
    qWarning() << "duplex " << printer.duplex();
    qWarning() << "from page " << printer.fromPage();
    qWarning() << "to from page " << printer.toPage();
    qWarning() << "ranges " << printer.pageRanges().toString();
    qWarning() << "printRange " << printer.printRange();
    qWarning() << "fullPage " << printer.fullPage();
    qWarning() << "pageOrder " << printer.pageOrder();
    qWarning() << "pageRect " << printer.pageRect(QPrinter::Millimeter);
    qWarning() << "paperRect " << printer.paperRect(QPrinter::Millimeter);
    qWarning() << "printProgram " << printer.printProgram();
    qWarning() << "printerName " << printer.printerName();
    qWarning() << "resolution " << printer.resolution();
    qWarning() << "PrintToFile"
               << print_dialog.testOption(QAbstractPrintDialog::PrintToFile);
    qWarning() << "PrintSelection"
               << print_dialog.testOption(QAbstractPrintDialog::PrintSelection);
    qWarning() << "PrintPageRange"
               << print_dialog.testOption(QAbstractPrintDialog::PrintPageRange);
    qWarning() << "PrintShowPageSize"
               << print_dialog.testOption(
                      QAbstractPrintDialog::PrintShowPageSize);
    qWarning() << "PrintCollateCopies"
               << print_dialog.testOption(
                      QAbstractPrintDialog::PrintCollateCopies);
    qWarning() << "PrintCurrentPage"
               << print_dialog.testOption(
                      QAbstractPrintDialog::PrintCurrentPage);
    qWarning() << "PrintSelection"
               << print_dialog.testOption(QAbstractPrintDialog::PrintSelection);
    QStringList dialogOptions =
        printer.printEngine()
            ->property(QPrintEngine::PrintEnginePropertyKey(0xfe00))
            .toStringList();
    for (const auto &opt : dialogOptions) {
      qWarning() << opt;
    }
    auto options = createPrintCommand(printer, src_file);
    if (!options.empty()) {
      QProcess process;
      process.setProgram(cups_executable);
      process.setArguments(options);
      process.start(cups_executable, options);
      if (!process.waitForStarted()) {
        qWarning() << "Can not start the lp programm";
      }
      process.waitForFinished();
      qWarning() << process.program() << process.arguments();
      qWarning() << QString(process.readAllStandardOutput());
      qWarning() << QString(process.readAllStandardError());
    }
    // print_dialog.dumpObjectTree();
    // QAbstractPrintDialog* priv_dialog=static_cast<QAbstractPrintDialog*>
    // (&print_dialog); QPrintDialogPrivate*
    // priv_dialog_priv=reinterpret_cast<QPrintDialogPrivate*> (priv_dialog);
    // auto opts= print_dialog.options();
  }
}

/// @details: https://www.cups.org/doc/options.html
/// @details: man lp-cups
QStringList PrinterLauncher::createPrintCommand(const QPrinter &printer,
                                                const QString &file) {
  QStringList res;
  res.reserve(300);
  const QString local_file_path = QUrl(file).toString(QUrl::PreferLocalFile);
  res.append(local_file_path);
  // printer name
  {
    const QString printer_name = printer.printerName();
    if (!printer_name.isEmpty()) {
      res.append("-d");
      res.append(printer_name);
    }
  }
  // copies
  const int copies = printer.copyCount();
  res.append("-n");
  res.append(QString::number(copies));
  // job name
  {
    const QString job_name = printer.docName();
    if (!job_name.isEmpty()) {
      res.append("-t");
      res.append(job_name);
    } else {
      const QString doc_name = QFileInfo(file).fileName();
      res.append("-t");
      res.append(doc_name);
    }
  }
  /* page ranges
   * The page numbers used by page-ranges refer to the output pages and not the
   document's page numbers. Options like number-up can make the output page
   numbering not match the document page numbers.
   https://www.cups.org/doc/options.html
  */
  if (printer.printRange() == QPrinter::PageRange) {
    res.append("-P");
    res.append(printer.pageRanges().toString());
  }
  // collate
  res.append(kKeyO);
  res.append(printer.collateCopies() ? "collate=true" : "collate=false");
  // page layout
  {
    const QPageLayout layout = printer.pageLayout();
    const QPageSize page_size = layout.pageSize();
    // media
    if (page_size.isValid()) {
      res.append(kKeyO);
      res.append("media=" + page_size.name());
    }
    // orientation
    res.append(kKeyO);
    res.append(layout.orientation() == QPageLayout::Portrait ? "portrait"
                                                             : "landscape");
    // sides
    res.append(kKeyO);
    switch (printer.duplex()) {
    case QPrinter::DuplexNone:
      res.append("sides=one-sided");
      break;
    case QPrinter::DuplexAuto:
      res.append(layout.orientation() == QPageLayout::Portrait
                     ? "sides=two-sided-long-edge"
                     : "sides=two-sided-short-edge");
      break;
    case QPrinter::DuplexLongSide:
      res.append("sides=two-sided-long-edge");
      break;
    case QPrinter::DuplexShortSide:
      res.append("two-sided-short-edge");
      break;
    }
    // margins
    const QMarginsF margins = layout.margins(QPageLayout::Point);
    res.append(kKeyO);
    res.append("page-left=" + QString::number(margins.left()));
    res.append(kKeyO);
    res.append("page-right=" + QString::number(margins.right()));
    res.append(kKeyO);
    res.append("page-bottom=" + QString::number(margins.bottom()));
    res.append(kKeyO);
    res.append("page-top=" + QString::number(margins.top()));
    res.append(kKeyO);
    res.append("fit-to-page");
  }
  // order
  res.append(kKeyO);
  res.append(printer.pageOrder() == QPrinter::LastPageFirst
                 ? "outputorder=reverse"
                 : "outputorder=normal");
  // native options
  {
    // HACK qtbase/src/printsupport/kernel/qcups_p.h
    QStringList dialog_options =
        printer.printEngine()
            ->property(QPrintEngine::PrintEnginePropertyKey(0xfe00))
            .toStringList();
    for (auto i = 0; i < dialog_options.count(); i += 2) {
      res.append(kKeyO);
      QString tmp = dialog_options[i];
      if (i < dialog_options.count() - 1) {
        tmp += "=";
        tmp += dialog_options[i + 1];
      }
      res.append(tmp);
    }
  }



  return res;
}

} // namespace core

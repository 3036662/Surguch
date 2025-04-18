/* File: printer_launcher.cpp
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

#include "printer_launcher.hpp"

#include <QFile>
#include <QFileInfo>
#include <QMargins>
#include <QPrintDialog>
#include <QPrintEngine>
#include <QPrinter>
#include <QProcess>
#include <QThread>
#include <QUrl>

namespace core {

PrinterLauncher::PrinterLauncher(QObject *parent) : QObject{parent} {}

/*!
 * \brief Launch a native print dialog,print with cups
 * \param src_file - pdf file to print
 * \param page_count - total pages in file
 * \param landscape - true if orientation is landscape
 */
void PrinterLauncher::print(const QString &src_file, int page_count,
                            bool landscape) {
    QPrinter printer;
    QPrintDialog print_dialog(&printer, nullptr);
    const QSize size_hint = print_dialog.minimumSizeHint();
    if (size_hint.isValid()) {
        print_dialog.resize(size_hint);
    }
    print_dialog.setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    print_dialog.setMinMax(1, page_count);
    print_dialog.setFromTo(1, page_count);
    print_dialog.setPrintRange(QAbstractPrintDialog::AllPages);
    print_dialog.setOption(QAbstractPrintDialog::PrintToFile, false);
    if (landscape) {
        printer.setPageOrientation(QPageLayout::Landscape);
    }
    if (print_dialog.exec() == QDialog::Accepted) {
        auto options = createPrintCommand(printer, src_file);
        if (!options.empty()) {
            auto *process = new QProcess();
            // finished
            connect(process, &QProcess::finished,
                    [process] { process->deleteLater(); });
            //  error
            connect(process, &QProcess::errorOccurred,
                    [process](QProcess::ProcessError err) {
                        if (err == QProcess::FailedToStart) {
                            qWarning()
                                << "Cups executable failed to start:"
                                << QString(process->readAllStandardError());
                        }
                        process->close();
                        process->deleteLater();
                    });
            process->setProgram(cups_executable_);
            process->setArguments(options);
            process->start(cups_executable_, options);
        }
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
     * The page numbers used by page-ranges refer to the output pages and not
     the document's page numbers. Options like number-up can make the output
     page numbering not match the document page numbers.
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

}  // namespace core

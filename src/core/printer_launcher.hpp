/* File: printer_launcher.hpp
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

#ifndef PRINTER_LAUNCHER_HPP
#define PRINTER_LAUNCHER_HPP

#include <QObject>
#include <QPrinter>
#include <QString>

namespace core {

constexpr const char *const kKeyO = "-o";

class PrinterLauncher : public QObject {
    Q_OBJECT

   public:
    explicit PrinterLauncher(QObject *parent = nullptr);

    /*!
     * \brief Launch a native print dialog,print with cups
     * \param src_file - pdf file to print
     * \param page_count - total pages in file
     * \param landscape - true if orientation is landscape
     */
    Q_INVOKABLE void print(const QString &src_file, int page_count,
                           bool landscape);

   signals:

   private:
    /// @details: https://www.cups.org/doc/options.html
    /// @details: man lp-cups
    static QStringList createPrintCommand(const QPrinter &printer,
                                          const QString &file);

    const QString cups_executable_ = "lp-cups";
};

}  // namespace core

#endif  // PRINTER_LAUNCHER_HPP

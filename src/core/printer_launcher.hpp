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
  Q_INVOKABLE void print(QString src_file, int page_count, bool landscape);

signals:

private:

  /// @details: https://www.cups.org/doc/options.html
  /// @details: man lp-cups
  static QStringList createPrintCommand(const QPrinter &printer, const QString &file);

  const QString cups_executable_ = "lp-cups";
};

} // namespace core

#endif // PRINTER_LAUNCHER_HPP

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

  Q_INVOKABLE void print(QString src_file, int page_count, bool landscape);

signals:

private:
  QStringList createPrintCommand(const QPrinter &printer, const QString &file);

  const QString cups_executable = "lp-cups";
};

} // namespace core

#endif // PRINTER_LAUNCHER_HPP

#include "file_recover_worker.hpp"
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>

namespace core {

void FileRecoverWorker::recoverFileWithByteRange(const QString& path,
                                                 const RangesVector& branges) {
  const QFileInfo finfo(path);
  QFile src_file(path);
  const quint64 eof_pos = branges.at(1).first + branges.at(1).second;
  if (!src_file.exists() || branges.size() != 2 || eof_pos >= src_file.size()) {
    qWarning() << "[recoverFileWithByteRange] invalid parameters";
    emit recoverCompleted("");
    return;
  }
  // create temporary
  const QString tpm_dir =
      QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/";
  const QString prefix = "alt_csp";
  const QString postfix = ".recovered";
  QFile dest_file(tpm_dir + prefix + finfo.fileName() + postfix);
  if (dest_file.exists()) {
    dest_file.remove();
  }
  // create new destination file
  if (dest_file.open(QIODevice::WriteOnly)) {
    dest_file.close();
    dest_file.setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner);
  }
  if (!dest_file.open(QIODevice::WriteOnly) ||
      !src_file.open(QIODevice::ReadOnly)) {
    dest_file.close();
    src_file.close();
    qWarning() << "[recoverFileWithByteRange] recover failed";
    emit recoverCompleted("");
    return;
  }
  // copy
  src_file.seek(0);
  while (!src_file.atEnd() && src_file.pos() < eof_pos && !abort_recieved_) {
    char symbol = 0x00;
    if (!src_file.getChar(&symbol)) {
      break;
    }
    dest_file.putChar(symbol);
  }
  dest_file.close();
  src_file.close();
  const QFileInfo dest_finfo(dest_file);
  if (dest_finfo.size() != eof_pos) {
    qWarning() << "[recoverFileWithByteRange] recover failed";
    emit recoverCompleted("");
    return;
  }
  qWarning() << "recover file:" << dest_file.fileName();
  emit recoverCompleted(dest_file.fileName());
}

} // namespace core

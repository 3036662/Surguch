/* File: file_recover_worker.cpp
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

#include "file_recover_worker.hpp"
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>

namespace core {

/**
 * @brief Perform the recovery
 * @param path file to recover
 * @param branges the byteranges covered by signature
 */
void FileRecoverWorker::recoverFileWithByteRange(const QString &path,
                                                 const RangesVector &branges) {
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

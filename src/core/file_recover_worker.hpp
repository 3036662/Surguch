/* File: file_recover_worker.hpp
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

#ifndef FILE_RECOVER_WORKER_HPP
#define FILE_RECOVER_WORKER_HPP
#include <QObject>

#include "raw_signature.hpp"

namespace core {

/**
 * @brief Recover a file to it's signed state
 * @details Is supposed to run in a separate thread
 */
class FileRecoverWorker : public QObject {
    Q_OBJECT
   public:
    explicit FileRecoverWorker(QObject *parent = nullptr) : QObject(parent) {};
    void abort() { abort_recieved_ = true; };

   public slots:

    /**
     * @brief Perform the recovery
     * @param path file to recover
     * @param branges the byteranges covered by signature
     */
    void recoverFileWithByteRange(const QString &path,
                                  const RangesVector &branges);
   signals:

    /// @brief recoverFileWithByteRange completed
    void recoverCompleted(QString path);

   private:
    bool abort_recieved_ = false;
};

}  // namespace core

#endif  // FILE_RECOVER_WORKER_HPP

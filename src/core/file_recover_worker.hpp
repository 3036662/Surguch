#ifndef FILE_RECOVER_WORKER_HPP
#define FILE_RECOVER_WORKER_HPP
#include "raw_signature.hpp"

#include <QObject>

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

} // namespace core

#endif // FILE_RECOVER_WORKER_HPP

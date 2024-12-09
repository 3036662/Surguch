#ifndef FILE_RECOVER_WORKER_HPP
#define FILE_RECOVER_WORKER_HPP
#include "raw_signature.hpp"

#include <QObject>

namespace core {
class FileRecoverWorker : public QObject {
  Q_OBJECT
public:
  explicit FileRecoverWorker(QObject *parent = nullptr) : QObject(parent) {};

  void abort() { abort_recieved_ = true; };

public slots:
  void recoverFileWithByteRange(const QString& path, const RangesVector& branges);
signals:

  void recoverCompleted(QString path);

private:
  bool abort_recieved_ = false;
};

} // namespace core

#endif // FILE_RECOVER_WORKER_HPP

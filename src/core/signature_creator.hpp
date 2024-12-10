#ifndef SIGNATURE_CREATOR_HPP
#define SIGNATURE_CREATOR_HPP

#include "sign_worker.hpp"
#include <QObject>
#include <QVariant>

namespace core {

/**
 * @brief Create a signature and signature's stamp
 */
class SignatureCreator : public QObject {
  Q_OBJECT
public:
  explicit SignatureCreator(QObject *parent = nullptr);

  /// @brief Handle the result of the signature worker thread.
  void handleResult(const SignWorker::SignResult &res);

  /// @brief Recieve the estimated stamp size and send it to the frontend
  void handleStampResize(SignWorker::AimResizeFactor res);

  /**
   * @brief Create a signature (nonblocking)
   * @param QVariantMap, suitable for filling SignWorker::SignParams
   */
  Q_INVOKABLE bool createSignature(const QVariantMap &qvparams);

  /**
   * @brief estimate StampResizeFactor
   * @param qvparams from qml
   * @details In case text strings in certificates are very long, the stamp size
   * will be enlarged.
   */
  Q_INVOKABLE void estimateStampResizeFactor(const QVariantMap &qvparams);

signals:

  /// @brief Signature is created
  void signCompleted(QVariantMap res);

  /// @brief Send the estimated stamp size to the frontend
  void stampSizeEstimated(QVariantMap res);

private:
  /// @brief parse all parameters recieved from QML
  static SignWorker::SignParams createWorkerParams(const QVariantMap &qvparams);

  SignWorker *p_worker_resize_img_ = nullptr;
  SignWorker *p_worker_ = nullptr;
  QThread *p_sign_thread_ = nullptr;
  QThread *p_resize_img_thread_ = nullptr;
};

} // namespace core

#endif // SIGNATURE_CREATOR_HPP

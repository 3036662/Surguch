#ifndef SIGNATURE_CREATOR_HPP
#define SIGNATURE_CREATOR_HPP

#include "sign_worker.hpp"
#include <QObject>


namespace core{

class SignatureCreator : public QObject
{
    Q_OBJECT
public:
    explicit SignatureCreator(QObject *parent = nullptr);

    void handleResult(SignWorker::SignResult res);

    void handleStampResize(SignWorker::AimResizeFactor res);

    /**
     * @brief Create a signature (nonblocking)
     * @param QVariantMap, suitable for filling SignWorker::SignParams
    */
    Q_INVOKABLE bool createSignature(const QVariantMap& qvparams);

    Q_INVOKABLE void estimateStampResizeFactor(const QVariantMap& qvparams);

signals:
    void signCompleted(QVariantMap res);
    void stampSizeEstimated(QVariantMap res);

private:

    static SignWorker::SignParams createWorkerParams(const QVariantMap& qvparams);

    SignWorker* p_worker_resize_img_=nullptr;
    SignWorker* p_worker_=nullptr;
    QThread* p_sign_thread_=nullptr;
    QThread* p_resize_img_thread_=nullptr;

};

} // namespace core

#endif // SIGNATURE_CREATOR_HPP

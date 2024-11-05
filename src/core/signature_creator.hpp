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

    /**
     * @brief Create a signature (nonblocking)
     * @param QVariantMap, suitable for filling SignWorker::SignParams
    */
    Q_INVOKABLE bool createSignature(const QVariantMap& qvparams);
signals:
    void signCompleted(QVariantMap res);

private:
    SignWorker* p_worker_=nullptr;
    QThread* p_sign_thread_=nullptr;

};

} // namespace core

#endif // SIGNATURE_CREATOR_HPP

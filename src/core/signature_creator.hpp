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

    Q_INVOKABLE bool createSignature(const QVariantMap& qvparams);
signals:

};

} // namespace core

#endif // SIGNATURE_CREATOR_HPP

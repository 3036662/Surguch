#ifndef SIGNATURE_CREATOR_HPP
#define SIGNATURE_CREATOR_HPP

#include <QObject>

namespace core{

class SignatureCreator : public QObject
{
    Q_OBJECT
public:
    explicit SignatureCreator(QObject *parent = nullptr);

    Q_INVOKABLE bool createSignature(const QVariantMap& params);
signals:

};

} // namespace core

#endif // SIGNATURE_CREATOR_HPP

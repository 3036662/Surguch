#include "signature_creator.hpp"
#include <QDebug>
#include <QVariant>

namespace core{

SignatureCreator::SignatureCreator(QObject *parent)
    : QObject{parent}
{
}

bool SignatureCreator::createSignature(const QVariantMap& params){
    qWarning()<<"[SignatureCreator::CreateSignature]";
    qWarning()<<params["logo_path"].toString();
    return false;
}

} //namespace core

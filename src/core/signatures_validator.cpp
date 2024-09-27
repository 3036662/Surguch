#include "signatures_validator.hpp"
#include <QDebug>

namespace core{

SignaturesValidator::SignaturesValidator(QObject *parent)
    : QObject{parent}
{
 qWarning() << "Validator constructed";
}


SignaturesValidator::~SignaturesValidator(){
    qWarning() << "Validator's destructor";
}

void SignaturesValidator::validateSignatures(std::vector<core::RawSignature> raw_signatures_){
    qWarning() << "validating signatures "<<raw_signatures_.size();
    emit validationFinished();
}

} //namespace core

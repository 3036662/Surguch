#include "signatures_validator.hpp"
#include <QDebug>
#include <QThread>
#include "csp_response.hpp"

namespace core{

SignaturesValidator::SignaturesValidator(QObject *parent)
    : QObject{parent}
{
 qWarning() << "Validator constructed";
}


SignaturesValidator::~SignaturesValidator(){
    qWarning() << "Validator's destructor";
}

void SignaturesValidator::validateSignatures(std::vector<core::RawSignature> raw_signatures_,QString file_source){
    bool aborted=false;
    for (const auto& sig:raw_signatures_){
        if (abort_recieved_ || QThread::currentThread()->isInterruptionRequested()){
            qWarning()<<"Validation abort";
            aborted=true;
            break;
        }
        qWarning() << "validating signatures "<<raw_signatures_.size();
        //QThread::sleep(5);
        auto result=std::make_unique<CSPResponse>(sig,file_source.toStdString());

    }
    if (aborted){
        qWarning() << "quit without sending results";
        QThread::currentThread()->quit();
    }
    else{
        emit validationFinished();
    }
}

} //namespace core

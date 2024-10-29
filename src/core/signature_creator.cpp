#include "signature_creator.hpp"
#include "sign_worker.hpp"
#include <QDebug>
#include <QCoreApplication>
#include <QThread>
#include <QVariant>

namespace core{

SignatureCreator::SignatureCreator(QObject *parent)
    : QObject{parent}
{
}

bool SignatureCreator::createSignature(const QVariantMap& qvparams){
    qWarning()<<"[SignatureCreator::CreateSignature]";
    qWarning()<<qvparams["logo_path"].toString();
    SignWorker::SignParams params{};
    if (qvparams.contains("page_index")){
        params.page_index=qvparams.value("page_index").toInt();
    }
    if (qvparams.contains("page_width")){
        params.page_width=qvparams.value("page_width").toReal();
    }
    if (qvparams.contains("page_height")){
        params.page_width=qvparams.value("page_height").toReal();
    }
    if (qvparams.contains("stamp_x")){
        params.stamp_x=qvparams.value("stamp_x").toReal();
    }
    if (qvparams.contains("stamp_y")){
        params.stamp_y=qvparams.value("stamp_y").toReal();
    }
    if (qvparams.contains("stamp_width")){
        params.stamp_width=qvparams.value("stamp_width").toReal();
    }
    if (qvparams.contains("stamp_height")){
        params.stamp_height=qvparams.value("stamp_height").toReal();
    }
    if (qvparams.contains("logo_path")){
        params.logo_path=qvparams.value("logo_path").toString();
    }
    if (qvparams.contains("config_path")){
        params.config_path=qvparams.value("config_path").toString();
    }
    if (qvparams.contains("cert_serial")){
        params.cert_serial=qvparams.value("cert_serial").toString();
    }
    if (qvparams.contains("cert_subject")){
        params.cert_subject=qvparams.value("cert_subject").toString();
    }
    if (qvparams.contains("cert_time_validity")){
        params.cert_time_validity=qvparams.value("cert_time_validity").toString();
    }
    if (qvparams.contains("stamp_type")){
        params.stamp_type=qvparams.value("stamp_type").toString();
    }
    if (qvparams.contains("cades_type")){
        params.cades_type=qvparams.value("cades_type").toString();
    }
    if (qvparams.contains("file_to_sign_path")){
        params.file_to_sign_path=qvparams.value("file_to_sign_path").toString();
    }
    p_worker_=new SignWorker();
    p_sign_thread_=new QThread();
    p_worker_->moveToThread(p_sign_thread_);
    // start job
    QObject::connect(p_sign_thread_,&QThread::started,[params = std::move(params),this]() mutable{
        p_worker_->launchSign(std::move(params));
    });
    // app closed
    QObject::connect(QCoreApplication::instance(),&QCoreApplication::aboutToQuit,[this](){
        if (p_sign_thread_!=nullptr && p_sign_thread_->isRunning()){
            p_sign_thread_->requestInterruption();
            p_sign_thread_->wait();
        }
    });
    // job is completed
    QObject::connect(p_worker_,&SignWorker::signCompleted,[this](SignWorker::SignResult res){
        handleResult(std::move(res));
        p_sign_thread_->quit();
    });
    // thread is finished
    QObject::connect(p_sign_thread_,&QThread::finished,[this](){
        p_worker_->deleteLater();
        p_sign_thread_->deleteLater();
        p_worker_=nullptr;
        p_sign_thread_=nullptr;
    });
    p_sign_thread_->start();;
    return true;
}


void SignatureCreator::handleResult(SignWorker::SignResult res){
    qWarning()<<"Implement ME, result handling";
}

} //namespace core

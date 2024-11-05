#include "sign_worker.hpp"

#include "pdf_csp_c.hpp"
#include <QDebug>
#include <QStandardPaths>
#include <QThread>

namespace core{

SignWorker::SignWorker(QObject *parent)
    : QObject{parent}
{

}

void SignWorker::launchSign(SignParams sign_params){
    params_=std::move(sign_params);
    SignResult res=preparePdf();
    emit signCompleted(res);
}

SignWorker::SignResult SignWorker::preparePdf(){
    pdfcsp::pdf::CSignParams pod_params{};
    pod_params.page_index=params_.page_index;
    pod_params.page_width=params_.page_width;
    pod_params.page_height=params_.page_height;
    pod_params.stamp_x=params_.stamp_x;
    pod_params.stamp_y=params_.stamp_y;
    pod_params.stamp_width=params_.stamp_width;
    pod_params.stamp_height=params_.stamp_height;
    QByteArray qb_logo_path=params_.logo_path.toUtf8();
    pod_params.logo_path=qb_logo_path.data();
    QByteArray qb_config_path=params_.config_path.toUtf8();
    pod_params.config_path=qb_config_path.data();
    QByteArray qb_cert_serial=params_.cert_serial.toUtf8();
    pod_params.cert_serial=qb_cert_serial.data();
    QByteArray qb_cert_subject=params_.cert_subject.toUtf8();
    pod_params.cert_subject=qb_cert_subject.data();
    QByteArray qb_cert_time_validity=params_.cert_time_validity.toUtf8();
    pod_params.cert_time_validity=qb_cert_time_validity.data();
    QByteArray qb_stamp_type=params_.stamp_type.toUtf8();
    pod_params.stamp_type=qb_stamp_type.data();
    QByteArray qb_cades_type=params_.cades_type.toUtf8();
    pod_params.cades_type=qb_cades_type.data();
    QByteArray qb_file_to_sign_path=params_.file_to_sign_path.toUtf8();
    pod_params.file_to_sign_path=qb_file_to_sign_path.data();
    QString temp_dir = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    QByteArray qb_temp_dir=temp_dir.toUtf8();
    if (!temp_dir.isEmpty()){
        pod_params.temp_dir_path=qb_temp_dir.data();
    } else{
        qWarning("Can not determine the user's temporary location");
    }
    QByteArray qb_tsp_url=params_.tsp_url.toUtf8();
    pod_params.tsp_link=qb_tsp_url.data();
    auto* result=pdfcsp::pdf::PrepareDoc(pod_params);
    SignResult res{};
    if (result!=nullptr){
        res.status=result->status;
        if (result->tmp_file_path!=nullptr){
            res.tmp_result_file=result->tmp_file_path;
        }
        if (result->err_string!=nullptr){
            res.err_string=result->err_string;
        }
    }
    pdfcsp::pdf::FreePrepareDocResult(result);
    return res;
}

} //namespace core

#include "sign_worker.hpp"


#include <QDebug>
#include <QStandardPaths>
#include <QThread>

namespace core{

SignWorker::SignWorker(QObject *parent)
    : QObject{parent},locale_(newlocale(LC_ALL_MASK,"POSIX",static_cast<locale_t>(0)))
{

}

SignWorker::~SignWorker(){
    if(locale_!=nullptr){
        freelocale(locale_);
    }
}

void SignWorker::launchSign(SignParams sign_params){
    params_=std::move(sign_params);
    SignResult res=preparePdf();
    emit signCompleted(res);
}


 SignWorker::SharedParamWrapper SignWorker::createParams() const{
     auto params_wrapper=std::make_shared<CSignParamsWrapper>();
     pdfcsp::pdf::CSignParams& pod_params=params_wrapper->pod_params;
     pod_params.page_index=params_.page_index;
     pod_params.page_width=params_.page_width;
     pod_params.page_height=params_.page_height;
     pod_params.stamp_x=params_.stamp_x;
     pod_params.stamp_y=params_.stamp_y;
     pod_params.stamp_width=params_.stamp_width;
     pod_params.stamp_height=params_.stamp_height;
     params_wrapper->qb_logo_path=params_.logo_path.toUtf8();
     pod_params.logo_path=params_wrapper->qb_logo_path.data();
     params_wrapper->qb_config_path=params_.config_path.toUtf8();
     pod_params.config_path=params_wrapper->qb_config_path.data();
     params_wrapper->qb_cert_serial=params_.cert_serial.toUtf8();
     pod_params.cert_serial=params_wrapper->qb_cert_serial.data();
     params_wrapper->qb_cert_serial_prefix=params_.cert_serial_prefix.toUtf8();
     pod_params.cert_serial_prefix=params_wrapper->qb_cert_serial_prefix.data();
     params_wrapper->qb_cert_subject=params_.cert_subject.toUtf8();
     pod_params.cert_subject=params_wrapper->qb_cert_subject.data();
     params_wrapper->qb_cert_subject_prefix=params_.cert_subject_prefix.toUtf8();
     pod_params.cert_subject_prefix=params_wrapper->qb_cert_subject_prefix.data();
     params_wrapper->qb_cert_time_validity=params_.cert_time_validity.toUtf8();
     pod_params.cert_time_validity=params_wrapper->qb_cert_time_validity.data();
     params_wrapper->qb_stamp_type=params_.stamp_type.toUtf8();
     pod_params.stamp_type=params_wrapper->qb_stamp_type.data();
     params_wrapper->qb_cades_type=params_.cades_type.toUtf8();
     pod_params.cades_type=params_wrapper->qb_cades_type.data();
     params_wrapper->qb_file_to_sign_path=params_.file_to_sign_path.toUtf8();
     pod_params.file_to_sign_path=params_wrapper->qb_file_to_sign_path.data();
     params_wrapper->temp_dir = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
     params_wrapper->qb_temp_dir=params_wrapper->temp_dir.toUtf8();
     params_wrapper->qb_stamp_title=params_.stamp_title.toUtf8();
     pod_params.stamp_title=params_wrapper->qb_stamp_title.data();

     if (!params_wrapper->temp_dir.isEmpty()){
         pod_params.temp_dir_path=params_wrapper->qb_temp_dir.data();
     } else{
         qWarning("Can not determine the user's temporary location");
     }
     params_wrapper->qb_tsp_url=params_.tsp_url.toUtf8();
     pod_params.tsp_link=params_wrapper->qb_tsp_url.data();
     return params_wrapper;
}

void SignWorker::estimateStampSize(SignParams sign_params){
    if (locale_!=nullptr){
        uselocale(locale_);
    }
    params_=std::move(sign_params);
    auto params_wrapper=createParams();
    auto* p_resize_factor=pdfcsp::pdf::GetStampResultingSizeFactor(params_wrapper->pod_params);
    if (p_resize_factor!=nullptr){
        qWarning()<< "[SignWorker] Resize factor: x="<<p_resize_factor->x<<" y= "<<p_resize_factor->y<<"\n";
        AimResizeFactor res{};
        res.x=p_resize_factor->x;
        res.y=p_resize_factor->y;
        pdfcsp::pdf::FreeImgResizeFactorResult(p_resize_factor);
        emit resizeStampCompleted(res);
    }
}

SignWorker::SignResult SignWorker::preparePdf(){
    if (locale_!=nullptr){
        uselocale(locale_);
    }
    auto params_wrapper=createParams();
    auto* result=pdfcsp::pdf::PrepareDoc(params_wrapper->pod_params);
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

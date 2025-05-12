#include "t_stamp.hpp"

#include <QTest>
#include <QQuickItem>
#include <QVariant>
#include <QFuture>
#include <QFutureWatcher>
#include <QImage>
#include <QStandardPaths>

#include "cpp_views/preview_render.hpp"
#include "models/profiles_model.hpp"
#include "core/csp_c_bridge/bridge_utils.hpp"

#include "pdf_csp_c.hpp"

TStamp::TStamp(QObject *parent) : QObject{parent} {}

void TStamp::createPreview() {
    PreviewRender::SignParams params_;
    params_.bg_opacity = 1;
    params_.bg_transparent = 1;
    params_.border_color.R = 255;
    params_.border_color.G = 0;
    params_.border_color.B = 0;
    params_.border_radius = 50;
    params_.border_width = 50;
    params_.cades_type = "CADES_BES";
    params_.cert_serial = "7c001e316d0c3296185e9c6902000d001e316d";
    params_.cert_serial_prefix = "Сертификат: ";
    params_.cert_subject = "Test Certificate";
    params_.cert_subject_prefix = "Субъект: ";
    params_.cert_time_validity = "Действителен: 2025-04-21 08:33:16 UTC по 2025-06-21 08:43:16 UTC";
    params_.config_path = "/home/dv/.config/csppdf";
    params_.file_to_sign_path = "/home/dv/Документы/42_pades-xlt1-sertum_pro.pdf";
    params_.logo_path = "/home/dv/.config/csppdf/profile_3_logo.jpg";
    params_.page_height = 0;
    params_.page_index = 0;
    params_.page_width = 0;
    params_.stamp_height = 300;
    params_.stamp_title = "ДОКУМЕНТ ПОДПИСАН ЭЛЕКТРОННОЙ ПОДПИСЬЮ";
    params_.stamp_type = "test";
    params_.stamp_width = 150;
    params_.stamp_x = 0;
    params_.stamp_y = 0;
    params_.tsp_url = "";
    params_.text_color.R = 255;
    params_.text_color.G = 0;
    params_.text_color.B = 0;

    auto params_wrapper = std::make_shared<PreviewRender::CSignParamsWrapper>();
    pdfcsp::pdf::CSignParams &pod_params = params_wrapper->pod_params;
    pod_params.page_index = params_.page_index;
    pod_params.page_width = params_.page_width;
    pod_params.page_height = params_.page_height;
    pod_params.stamp_x = params_.stamp_x;
    pod_params.stamp_y = params_.stamp_y;
    pod_params.stamp_width = params_.stamp_width;
    pod_params.stamp_height = params_.stamp_height;
    params_wrapper->qb_logo_path = params_.logo_path.toUtf8();
    if (!params_wrapper->qb_logo_path.isEmpty()) {
        pod_params.logo_path = params_wrapper->qb_logo_path.data();
    }
    params_wrapper->qb_config_path = params_.config_path.toUtf8();
    pod_params.config_path = params_wrapper->qb_config_path.data();
    params_wrapper->qb_cert_serial = params_.cert_serial.toUtf8();
    pod_params.cert_serial = params_wrapper->qb_cert_serial.data();
    params_wrapper->qb_cert_serial_prefix = params_.cert_serial_prefix.toUtf8();
    pod_params.cert_serial_prefix =
        params_wrapper->qb_cert_serial_prefix.data();
    params_wrapper->qb_cert_subject = params_.cert_subject.toUtf8();
    pod_params.cert_subject = params_wrapper->qb_cert_subject.data();
    params_wrapper->qb_cert_subject_prefix =
        params_.cert_subject_prefix.toUtf8();
    pod_params.cert_subject_prefix =
        params_wrapper->qb_cert_subject_prefix.data();
    params_wrapper->qb_cert_time_validity = params_.cert_time_validity.toUtf8();
    pod_params.cert_time_validity =
        params_wrapper->qb_cert_time_validity.data();
    params_wrapper->qb_stamp_type = params_.stamp_type.toUtf8();
    pod_params.stamp_type = params_wrapper->qb_stamp_type.data();
    params_wrapper->qb_cades_type = params_.cades_type.toUtf8();
    pod_params.cades_type = params_wrapper->qb_cades_type.data();
    params_wrapper->qb_file_to_sign_path = params_.file_to_sign_path.toUtf8();
    pod_params.file_to_sign_path = params_wrapper->qb_file_to_sign_path.data();
    params_wrapper->temp_dir =
        QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    params_wrapper->qb_temp_dir = params_wrapper->temp_dir.toUtf8();
    params_wrapper->qb_stamp_title = params_.stamp_title.toUtf8();
    pod_params.stamp_title = params_wrapper->qb_stamp_title.data();

    if (!params_wrapper->temp_dir.isEmpty()) {
        pod_params.temp_dir_path = params_wrapper->qb_temp_dir.data();
    } else {
        qWarning("Can not determine the user's temporary location");
    }
    params_wrapper->qb_tsp_url = params_.tsp_url.toUtf8();
    pod_params.tsp_link = params_wrapper->qb_tsp_url.data();
    pod_params.text_color.red = params_.text_color.R;
    pod_params.text_color.green = params_.text_color.G;
    pod_params.text_color.blue = params_.text_color.B;
    pod_params.border_color.red = params_.border_color.R;
    pod_params.border_color.green = params_.border_color.G;
    pod_params.border_color.blue = params_.border_color.B;
    pod_params.border_width = params_.border_width;
    pod_params.border_radius = params_.border_radius;
    pod_params.bg_transparent = params_.bg_transparent;
    pod_params.bg_opacity = params_.bg_opacity;

    auto result = pdfcsp::pdf::BakeSignatureStampImage(pod_params);
    QVERIFY(result != nullptr);
}

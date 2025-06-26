#include "gui_utils.hpp"

#include <QDebug>
#include <QFontDatabase>
#include <QImage>
#include <QStandardPaths>

namespace core::gui {

inline std::vector<unsigned char> *glueImageWithMask(
    const unsigned char *const img, size_t img_size,
    const unsigned char *img_mask, size_t mask_size) {
    if (img_size == 0 || img == nullptr) {
        return {};
    }
    auto result = std::make_unique<std::vector<unsigned char>>();
    result->reserve(img_size + mask_size);
    for (size_t i = 0; i < img_size; ++i) {
        result->push_back(img[i]);
        if (i >= 2 && (i - 2) % 3 == 0) {
            const size_t mask_index = (i - 2) / 3;
            if (img_mask != nullptr && mask_index < mask_size) {
                // result.push_back(img_mask[mask_index] > 0 ? 0xff : 0x00);
                result->push_back(img_mask[mask_index]);
            } else {
                result->push_back(0xff);
            }
        }
    }
    return result.release();
}

std::unique_ptr<BakeResult> prepareStampImage(
    const SharedSignParamWrapper &params) {
    auto result = std::make_unique<BakeResult>(BakeResult{
        std::unique_ptr<pdfcsp::pdf::BakeSignatureStampResult,
                        void (*)(pdfcsp::pdf::BakeSignatureStampResult *)>(
            pdfcsp::pdf::BakeSignatureStampImage(params->pod_params),
            pdfcsp::pdf::FreeBakedSigStampImage),
        std::unique_ptr<QImage>()});
    // qWarning() << "result pointer:" << result.get();
    if (result && result->data_ && result->data_->img != nullptr &&
        result->data_->img_size > 0) {
        auto *p_vec = glueImageWithMask(
            result->data_->img, result->data_->img_size,
            result->data_->img_mask, result->data_->img_mask_size);
        result->image_ = std::make_unique<QImage>(  // result->data_->img,
            p_vec->data(), result->data_->resolution_x,
            result->data_->resolution_y, result->data_->resolution_x * 4,
            QImage::Format_RGBA8888,
            [](void *ptr) {
                delete static_cast<std::vector<unsigned char> *>(ptr);
            },
            p_vec);
        // qWarning() << "resolution_x = " << result->data_->resolution_x;
        // qWarning() << "resolution_y = " << result->data_->resolution_y;
    }
    return result;
}

/// @brief prepare preview params for later use
SignParams preparePreviewParams(const QVariantMap &qvparams) {
    SignParams params_;
    if (qvparams.contains("bg_transparent")) {
        params_.bg_transparent = qvparams.value("bg_transparent").toBool();
    }
    if (qvparams.contains("bg_opacity")) {
        params_.bg_opacity = qvparams.value("bg_opacity").toUInt();
    }
    if (qvparams.contains("page_index")) {
        params_.page_index = qvparams.value("page_index").toInt();
    }
    if (qvparams.contains("border_width")) {
        params_.border_width = qvparams.value("border_width").toUInt();
    }
    if (qvparams.contains("border_radius")) {
        params_.border_radius = qvparams.value("border_radius").toUInt();
    }
    if (qvparams.contains("page_width")) {
        params_.page_width = qvparams.value("page_width").toReal();
    }
    if (qvparams.contains("page_height")) {
        params_.page_height = qvparams.value("page_height").toReal();
    }
    if (qvparams.contains("stamp_x")) {
        params_.stamp_x = qvparams.value("stamp_x").toReal();
    }
    if (qvparams.contains("stamp_y")) {
        params_.stamp_y = qvparams.value("stamp_y").toReal();
    }
    if (qvparams.contains("stamp_width")) {
        // params_.stamp_width = qvparams.value("stamp_width").toReal();
        // we only need height/width ration so hardcode params
        params_.stamp_width = 900;
    }
    if (qvparams.contains("stamp_height")) {
        // params_.stamp_height = qvparams.value("stamp_height").toReal();
        params_.stamp_height = 300;
    }
    if (qvparams.contains("logo_path")) {
        params_.logo_path = qvparams.value("logo_path").toUrl().toLocalFile();
        if (params_.logo_path.isEmpty()) {
            params_.logo_path = qvparams.value("logo_path").toString();
        }
    }
    if (qvparams.contains("config_path")) {
        params_.config_path = qvparams.value("config_path").toString();
    }
    if (qvparams.contains("cert_serial")) {
        params_.cert_serial = qvparams.value("cert_serial").toString();
    }
    if (qvparams.contains("cert_subject")) {
        params_.cert_subject = qvparams.value("cert_subject").toString();
    }
    if (qvparams.contains("cert_time_validity")) {
        params_.cert_time_validity =
            qvparams.value("cert_time_validity").toString();
    }
    if (qvparams.contains("stamp_type")) {
        params_.stamp_type = qvparams.value("stamp_type").toString();
    }
    if (qvparams.contains("cades_type")) {
        params_.cades_type = qvparams.value("cades_type").toString();
    }
    if (qvparams.contains("file_to_sign_path")) {
        params_.file_to_sign_path =
            qvparams.value("file_to_sign_path").toString();
    }
    if (qvparams.contains("tsp_url")) {
        params_.tsp_url = qvparams.value("tsp_url").toString();
    }
    if (qvparams.contains("cert_serial_prefix")) {
        params_.cert_serial_prefix =
            qvparams.value("cert_serial_prefix").toString();
    }
    if (qvparams.contains("cert_subject_prefix")) {
        params_.cert_subject_prefix =
            qvparams.value("cert_subject_prefix").toString();
    }
    if (qvparams.contains("stamp_title")) {
        params_.stamp_title = qvparams.value("stamp_title").toString();
    }
    if (qvparams.contains("border_color_red")) {
        params_.border_color.R = qvparams.value("border_color_red").toUInt();
    }
    if (qvparams.contains("border_color_green")) {
        params_.border_color.G = qvparams.value("border_color_green").toUInt();
    }
    if (qvparams.contains("border_color_blue")) {
        params_.border_color.B = qvparams.value("border_color_blue").toUInt();
    }
    if (qvparams.contains("text_color_red")) {
        params_.text_color.R = qvparams.value("text_color_red").toUInt();
    }
    if (qvparams.contains("text_color_green")) {
        params_.text_color.G = qvparams.value("text_color_green").toUInt();
    }
    if (qvparams.contains("text_color_blue")) {
        params_.text_color.B = qvparams.value("text_color_blue").toUInt();
    }
    return params_;
}

/// @brief Gather all parameters (pdfcsp::pdf::CSignParam)
SharedSignParamWrapper createParams(SignParams params_) {
    auto params_wrapper = std::make_shared<CSignParamsWrapper>();
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
    return params_wrapper;
}

std::unique_ptr<BakeRubberResult> prepareImage(
    const SharedRubberParamWrapper &params) {
    auto result = std::make_unique<BakeRubberResult>(BakeRubberResult{
        std::unique_ptr<pdfcsp::pdf::BakeRubberStamResult,
                        void (*)(pdfcsp::pdf::BakeRubberStamResult *)>(
            pdfcsp::pdf::BakeRubberStamp(params->pod_params),
            pdfcsp::pdf::FreeRubberStampResult),
        std::unique_ptr<QImage>()});
    // qWarning() << "result pointer:" << result.get();
    if (result && result->data_ && result->data_->img != nullptr &&
        result->data_->img_size > 0) {
        auto *p_vec = glueImageWithMask(
            result->data_->img, result->data_->img_size,
            result->data_->img_mask, result->data_->img_mask_size);
        result->image_ = std::make_unique<QImage>(  // result->data_->img,
            p_vec->data(), result->data_->resolution_x,
            result->data_->resolution_y, result->data_->resolution_x * 4,
            QImage::Format_RGBA8888,
            [](void *ptr) {
                delete static_cast<std::vector<unsigned char> *>(ptr);
            },
            p_vec);
        // qWarning() << "resolution_x = " << result->data_->resolution_x;
        // qWarning() << "resolution_y = " << result->data_->resolution_y;
    }
    return result;
}

RubberParams prepareParams(const QVariantMap &qvparams) {
    RubberParams params;
    double zoom = 1;
    if (qvparams.contains("zoom_on_rubber_render")) {
        zoom = qvparams.value("zoom_on_rubber_render").toDouble();
    }
    if (qvparams.contains("page_index")) {
        params.page_index = qvparams.value("page_index").toUInt();
    }
    if (qvparams.contains("stamp_x")) {
        params.position_x = qvparams.value("stamp_x").toDouble();
    }
    if (qvparams.contains("stamp_y")) {
        params.position_y = qvparams.value("stamp_y").toDouble();
    }
    if (qvparams.contains("page_width")) {
        params.page_width = qvparams.value("page_width").toDouble();
    }
    if (qvparams.contains("page_height")) {
        params.page_height = qvparams.value("page_height").toDouble();
    }
    if (qvparams.contains("link")) {
        params.link = qvparams.value("link").toString().toStdString();
    }
    if (qvparams.contains("stamp_width")) {
        params.stamp_width = qvparams.value("stamp_width").toUInt();
        if (zoom > 0 && zoom < 1) {
            params.stamp_width = params.stamp_width / zoom;
        }
        // params.stamp_width = 400;
    }
    if (qvparams.contains("stamp_height")) {
        params.stamp_height = qvparams.value("stamp_height").toUInt();
        // params.stamp_height = 400;
    }
    if (qvparams.contains("annotation_width")) {
        params.real_stamp_width = qvparams.value("annotation_width").toUInt();
        params.annotation_width = qvparams["annotation_width"].toUInt();
        if (zoom > 0 && zoom < 1) {
            params.annotation_width = params.annotation_width / zoom;
        }
    }
    if (qvparams.contains("border_width")) {
        params.border_width = qvparams.value("border_width").toUInt();
    }
    if (qvparams.contains("border_radius")) {
        params.border_radius = qvparams.value("border_radius").toUInt();
    }
    if (qvparams.contains("bg_transparent")) {
        params.bg_transparent = qvparams.value("bg_transparent").toBool();
    }
    if (qvparams.contains("create_from_image")) {
        params.create_from_image = qvparams.value("create_from_image").toBool();
    }
    if (qvparams.contains("stamp_preserve_ratio")) {
        // params.stamp_preserve_ratio =
        // qvparams.value("stamp_preserve_ratio").toBool();
        params.stamp_preserve_ratio = true;
    }
    if (qvparams.contains("bg_opacity")) {
        params.bg_opacity = qvparams.value("bg_opacity").toUInt();
    }
    if (qvparams.contains("font_size")) {
        params.font_size = qvparams.value("font_size").toUInt();
    }
    if (qvparams.contains("font_weight")) {
        // params.font_weight = qvparams.value("font_weight").toUInt();
        params.font_weight = 100;
    }
    if (qvparams.contains("img_path")) {
        params.img_path = qvparams.value("img_path").toUrl().toLocalFile();
        if (params.img_path.isEmpty()) {
            params.img_path = qvparams.value("img_path").toString();
        }
    }
    if (qvparams.contains("stamp_text")) {
        params.annotation_text = qvparams.value("stamp_text").toString();
    }
    if (qvparams.contains("font_family")) {
        params.font_family = qvparams.value("font_family").toString();
        int default_weight = 400;
        int tmp_weight = 0;
        const QStringList styles = QFontDatabase::styles(params.font_family);
        qsizetype index_regular =
            styles.indexOf("regular", Qt::CaseInsensitive);
        if (index_regular < 0 && styles.size() > 0) {
            index_regular = 0;
        }
        tmp_weight = default_weight = QFontDatabase::weight(
            params.font_family, styles.value(index_regular));
        if (tmp_weight > 0 && tmp_weight <= 1000) {
            default_weight = tmp_weight;
        }
        params.font_weight = default_weight;
        //qWarning() << "weight:" << params.font_weight;
    }
    if (qvparams.contains("border_color_red")) {
        params.border_color.R = qvparams.value("border_color_red").toUInt();
    }
    if (qvparams.contains("border_color_green")) {
        params.border_color.G = qvparams.value("border_color_green").toUInt();
    }
    if (qvparams.contains("border_color_blue")) {
        params.border_color.B = qvparams.value("border_color_blue").toUInt();
    }
    if (qvparams.contains("text_color_red")) {
        params.text_color.R = qvparams.value("text_color_red").toUInt();
    }
    if (qvparams.contains("text_color_green")) {
        params.text_color.G = qvparams.value("text_color_green").toUInt();
    }
    if (qvparams.contains("text_color_blue")) {
        params.text_color.B = qvparams.value("text_color_blue").toUInt();
    }
    if (qvparams.contains("bg_color_red")) {
        params.bg_color.R = qvparams.value("bg_color_red").toUInt();
    }
    if (qvparams.contains("bg_color_green")) {
        params.bg_color.G = qvparams.value("bg_color_green").toUInt();
    }
    if (qvparams.contains("bg_color_blue")) {
        params.bg_color.B = qvparams.value("bg_color_blue").toUInt();
    }
    return params;
}

SharedRubberParamWrapper createParams(const RubberParams &params) {
    auto paramswrapper = std::make_shared<CRubberParamsWrapper>();
    pdfcsp::pdf::RubberStampParams &pod_params = paramswrapper->pod_params;
    paramswrapper->qb_img_path = params.img_path.toUtf8();
    if (!paramswrapper->qb_img_path.isEmpty()) {
        pod_params.src_img_path = paramswrapper->qb_img_path.data();
    }
    pod_params.target_x = params.stamp_width;
    pod_params.target_y = params.stamp_height;
    pod_params.stamp_preserve_ratio = params.stamp_preserve_ratio;
    pod_params.create_from_image = params.create_from_image;
    paramswrapper->qb_annotation_text = params.annotation_text.toUtf8();
    if (!paramswrapper->qb_annotation_text.isEmpty()) {
        pod_params.annotation_text = paramswrapper->qb_annotation_text.data();
    }
    pod_params.bg_color.red = params.bg_color.R;
    pod_params.bg_color.green = params.bg_color.G;
    pod_params.bg_color.blue = params.bg_color.B;
    pod_params.font_color.red = params.text_color.R;
    pod_params.font_color.green = params.text_color.G;
    pod_params.font_color.blue = params.text_color.B;
    pod_params.border_color.red = params.border_color.R;
    pod_params.border_color.green = params.border_color.G;
    pod_params.border_color.blue = params.border_color.B;
    paramswrapper->qb_font_family = params.font_family.toUtf8();
    if (!paramswrapper->qb_font_family.isEmpty()) {
        pod_params.font_family = paramswrapper->qb_font_family.data();
    }
    pod_params.border_radius =
        std::ceil(static_cast<double>(params.border_radius) / 900.0 *
                  static_cast<double>(params.annotation_width));
    pod_params.border_width =
        std::ceil(static_cast<double>(params.border_width) / 900.0 *
                  static_cast<double>(params.annotation_width));
    pod_params.font_size = 1;  // params.font_size;
    pod_params.font_weight = params.font_weight;
    pod_params.bg_transparent = params.bg_transparent;
    pod_params.bg_opacity = params.bg_opacity;
    pod_params.annotation_width = params.annotation_width;
    return paramswrapper;
}

std::vector<pdfcsp::pdf::CAnnotParams> createAnnotParams(
    const std::vector<std::shared_ptr<RubberStamp>> &params) {
    std::vector<pdfcsp::pdf::CAnnotParams> cparams;
    std::for_each(
        params.begin(), params.end(),
        [&cparams](const std::shared_ptr<RubberStamp> &p) {
            cparams.emplace_back(pdfcsp::pdf::CAnnotParams{
                p->page_index, p->qml_width, p->qml_height, p->position_x,
                p->position_y, p->stamp_width, p->stamp_height,
                p->res->data_->img, p->res->data_->img_size,
                p->res->data_->img_mask, p->res->data_->img_mask_size,
                p->res->data_->resolution_x, p->res->data_->resolution_y,
                p->link.empty() ? nullptr : p->link.data()});
        });
    return cparams;
}

}  //  namespace core::gui
#include <QDebug>
#include <QImage>

#include "gui_utils.hpp"

namespace core::gui {

inline std::vector<unsigned char>* glueImageWithMask(
    const unsigned char* const img , size_t img_size, const unsigned char* img_mask,
    size_t mask_size) {
    if (img_size == 0 || img == nullptr) {
        return {};
    }
    auto result=std::make_unique< std::vector<unsigned char>>();
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

std::unique_ptr<BakeRubberResult> prepareImage(
    const SharedParamWrapper &params) {
    auto result = std::make_unique<BakeRubberResult>(BakeRubberResult{
        std::unique_ptr<pdfcsp::pdf::BakeRubberStamResult,
                        void (*)(pdfcsp::pdf::BakeRubberStamResult *)>(
            pdfcsp::pdf::BakeRubberStamp(params->pod_params),
            pdfcsp::pdf::FreeRubberStampResult),
        std::unique_ptr<QImage>()});
    // qWarning() << "result pointer:" << result.get();
    if (result && result->data_ && result->data_->img != nullptr &&
        result->data_->img_size > 0) {
        auto* p_vec=glueImageWithMask(result->data_->img, result->data_->img_size,
                result->data_->img_mask, result->data_->img_mask_size);
        result->image_ = std::make_unique<QImage>(//result->data_->img,
            p_vec->data(),
                result->data_->resolution_x,
            result->data_->resolution_y, result->data_->resolution_x * 4,
            QImage::Format_RGBA8888,[](void *ptr){delete static_cast<std::vector<unsigned char>*>(ptr);},p_vec);
        //qWarning() << "resolution_x = " << result->data_->resolution_x;
        //qWarning() << "resolution_y = " << result->data_->resolution_y;
        }
    return result;
}

RubberParams prepareParams(const QVariantMap &qvparams)  {
    RubberParams params;
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
        params.link = qvparams.value("link").toString();
    }
    if (qvparams.contains("stamp_width")) {
        params.stamp_width = qvparams.value("stamp_width").toUInt();
        //params.stamp_width = 400;
    }
    if (qvparams.contains("stamp_height")) {
        params.stamp_height = qvparams.value("stamp_height").toUInt();
        //params.stamp_height = 400;
    }
    if (qvparams.contains("annotation_width")) {
        params.annotation_width = qvparams["annotation_width"].toUInt();
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
        //params.stamp_preserve_ratio = qvparams.value("stamp_preserve_ratio").toBool();
        params.stamp_preserve_ratio = true;
    }
    if (qvparams.contains("bg_opacity")) {
        params.bg_opacity = qvparams.value("bg_opacity").toUInt();
    }
    if (qvparams.contains("font_size")) {
        params.font_size = qvparams.value("font_size").toUInt();
    }
    if (qvparams.contains("font_weight")) {
        //params.font_weight = qvparams.value("font_weight").toUInt();
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

 SharedParamWrapper createParams(const RubberParams& params)  {
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
    pod_params.border_radius = params.border_radius;
    pod_params.border_width = static_cast<double>(params.border_width) / 900.0 * static_cast<double>(params.annotation_width);
    pod_params.font_size = 1; //params.font_size;
    pod_params.font_weight = 400; //params.font_weight;
    pod_params.bg_transparent = params.bg_transparent;
    pod_params.bg_opacity = params.bg_opacity;
    pod_params.annotation_width = params.annotation_width;
    return paramswrapper;
}

std::vector<pdfcsp::pdf::CAnnotParams> createAnnotParams(const std::vector<std::shared_ptr<RubberStamp>>& params) {
    std::vector<pdfcsp::pdf::CAnnotParams> cparams;
    std::for_each(params.begin(), params.end(), [&cparams](const std::shared_ptr<RubberStamp> &p) {
        cparams.emplace_back(pdfcsp::pdf::CAnnotParams{
        .page_index = p->page_index,
        .page_width = p->qml_width,
        .page_height = p->qml_height,
        .stamp_x = p->position_x,
        .stamp_y = p->position_y,
        .stamp_width = p->stamp_width,
        .stamp_height = p->stamp_height,
        .img = p->res->data_->img,
        .img_size = p->res->data_->img_size,
        .img_mask = p->res->data_->img_mask,
        .img_mask_size = p->res->data_->img_mask_size,
        .resolution_x = p->res->data_->resolution_x,
        .resolution_y = p->res->data_->resolution_y,
        .link = p->link.toUtf8()});
    });
    return cparams;
}

}  //  namespace core::gui
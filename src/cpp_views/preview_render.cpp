#include "preview_render.hpp"

#include <QFuture>
#include <QPainter>
#include <QSGGeometryNode>
#include <QSGNode>
#include <QSGSimpleTextureNode>
#include <QScreen>
#include <QThread>
#include <QtConcurrent>
#include <QtMath>
#include <memory>

PreviewRender::PreviewRender() {
    setFlag(QQuickItem::ItemHasContents, true);
    setClip(true);
    const qreal pix_rat = QWindow().devicePixelRatio();
    if (pix_rat > 2) {
        dev_pix_ratio_ = static_cast<float>(pix_rat);
    }
}

QSGNode *PreviewRender::updatePaintNode(
    QSGNode *node,
    [[maybe_unused]] QQuickItem::UpdatePaintNodeData *updatePaintNodeData) {
    QSGSimpleTextureNode *rectNode = nullptr;
    if (node != nullptr) {
        rectNode = dynamic_cast<QSGSimpleTextureNode *>(node);
        if (!isVisible()) {
            // qWarning()<<"return same node, not visible";
            return node;
        }
    }
    if (rectNode == nullptr) {
        if (!size().isValid()) {
            return node;
        }
        // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
        rectNode = new QSGSimpleTextureNode();
        rectNode->setFiltering(QSGTexture::Linear);
        rectNode->setOwnsTexture(true);
    }

    if (result_ == nullptr || result_->image_ == nullptr) {
        auto img =
            std::make_unique<QImage>(size().toSize(), QImage::Format_RGB888);
        img->fill(Qt::white);  // Fill the image with white color
        QSGTexture *texture = window()->createTextureFromImage(*img);
        if (texture != nullptr) {
            rectNode->setTexture(texture);
            rectNode->setRect(QRectF(0, 0, width(), height()));
        }
        return rectNode;
    }
    QSGTexture *texture = window()->createTextureFromImage(*result_->image_);
    if (texture != nullptr) {
        rectNode->setTexture(texture);
        rectNode->setRect(QRectF(0, 0, width(), height()));
    }
    return rectNode;
}

void PreviewRender::createImage(const QVariantMap &qvparams) {
    preparePreviewParams(qvparams);
    auto params_wrapper = createParams();
    image_watcher_ = std::make_unique<ImageFutureWatcher>();
    QObject::connect(image_watcher_.get(), &ImageFutureWatcher::finished,
                     [this]() {
                         qWarning() << "finished";
                         saveImage();
                     });
    image_future_ = std::make_unique<ImageFuture>(
        QtConcurrent::run(prepareImage, params_wrapper));
    image_watcher_->setFuture(*image_future_);
}

void PreviewRender::saveImage() {
    if (image_future_ && image_future_->isValid()) {
        result_ = image_future_->takeResult();
    }
    if (result_ && result_->image_ && result_->image_->width() != 0) {
        // qWarning() << "width " << width();
        // qWarning() << "result->resolution_y " << result_->image_->height();
        // qWarning() << "result->resolution_x " << result_->image_->width();
        setHeight(static_cast<double>(result_->image_->height()) /
                  result_->image_->width() * width());
        // qWarning() << static_cast<double>(result_->image_->height()) /
        //                   result_->image_->width() * width();
    }
    emit imageReady();
}

std::unique_ptr<BakeResult> prepareImage(
    const PreviewRender::SharedParamWrapper &params) {
    auto result = std::make_unique<BakeResult>(BakeResult{
        std::unique_ptr<pdfcsp::pdf::BakeSignatureStampResult,
                        void (*)(pdfcsp::pdf::BakeSignatureStampResult *)>(
            pdfcsp::pdf::BakeSignatureStampImage(params->pod_params),
            pdfcsp::pdf::FreeBakedSigStampImage),
        std::unique_ptr<QImage>()});
    qWarning() << "result pointer:" << result.get();
    if (result && result->data_ && result->data_->img != nullptr &&
        result->data_->img_size > 0) {
        result->image_ = std::make_unique<QImage>(
            result->data_->img, result->data_->resolution_x,
            result->data_->resolution_y, result->data_->resolution_x * 3,
            QImage::Format_RGB888);
    }
    return result;
}

/// @brief prepare preview params for later use
void PreviewRender::preparePreviewParams(const QVariantMap &qvparams) {
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
        params_.logo_path = qvparams.value("logo_path").toString();
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
}

/// @brief Gather all parameters (pdfcsp::pdf::CSignParam)
[[nodiscard]] PreviewRender::SharedParamWrapper PreviewRender::createParams()
    const {
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

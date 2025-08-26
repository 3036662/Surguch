#include "rubber_preview_render.hpp"

#include <QFuture>
#include <QPainter>
#include <QRawFont>
#include <QSGGeometryNode>
#include <QSGNode>
#include <QSGSimpleTextureNode>
#include <QScreen>
#include <QThread>
#include <QtConcurrent>
#include <QtMath>
#include <memory>

RubberPreviewRender::RubberPreviewRender() {
    setFlag(QQuickItem::ItemHasContents, true);
    setClip(true);
    const qreal pix_rat = QWindow().devicePixelRatio();
    if (pix_rat > 2) {
        dev_pix_ratio_ = static_cast<float>(pix_rat);
    }
}

QSGNode *RubberPreviewRender::updatePaintNode(
    QSGNode *node,
    [[maybe_unused]] QQuickItem::UpdatePaintNodeData *updatePaintNodeData) {
    QSGSimpleTextureNode *rectNode = nullptr;
    if (node != nullptr) {
        rectNode = dynamic_cast<QSGSimpleTextureNode *>(node);
        if (!isVisible()) {
            return node;
        }
    }
    if (width() == 0 || height() == 0) {
        return node;
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

    // Fill the image with white color
    if (result_ == nullptr || result_->image_ == nullptr) {
        qWarning()
            << "RubberPreviewRender: updateNode() call with an empty result";

        // Create an empty image if it does not exist.
        if (!blank_image_ || blank_image_->width() != requested_width_ ||
            blank_image_->height() != requested_height_) {
            blank_image_ = std::make_unique<QImage>(
                requested_width_, requested_height_, QImage::Format_RGB888);
            blank_image_->fill(Qt::white);
        }
        QSGTexture *texture = window()->createTextureFromImage(*blank_image_);
        setWidth(requested_width_);
        setHeight(requested_height_);
        if (texture != nullptr) {
            rectNode->setTexture(texture);
            rectNode->setRect(
                QRectF(0, 0, requested_width_, requested_height_));
        }

        return rectNode;
    }

    QSGTexture *texture = nullptr;

    // horizontal img or rectangle
    if (result_->data_->resolution_x >= result_->data_->resolution_y) {
        const auto yx_ratio_result =
            static_cast<double>(result_->data_->resolution_y) /
            result_->data_->resolution_x;
        const auto target_height = yx_ratio_result * requested_width_;
        double scale_fact = 1.0;
        if (target_height > requested_height_) {
            scale_fact = requested_height_ / target_height;
        }
        QSGTexture *texture = window()->createTextureFromImage(
            (*result_->image_)
                .scaled(requested_width_ * scale_fact,
                        target_height * scale_fact, Qt::KeepAspectRatio));
        setWidth(requested_width_ * scale_fact);
        setHeight(target_height * scale_fact);
        if (texture != nullptr) {
            rectNode->setTexture(texture);
            rectNode->setRect(QRectF(0, 0, width(), height()));
        }
        return rectNode;
    }

    // vertical img
    const auto xy_ratio_result =
        static_cast<double>(result_->data_->resolution_x) /
        result_->data_->resolution_y;
    const auto target_width = xy_ratio_result * requested_height_;
    double scale_fact = 1.0;
    if (target_width > requested_width_) {
        scale_fact = requested_width_ / target_width;
    }
    texture = window()->createTextureFromImage(
        (*result_->image_)
            .scaled(target_width * scale_fact, requested_height_ * scale_fact,
                    Qt::KeepAspectRatio));

    setHeight(requested_height_ * scale_fact);
    setWidth(target_width * scale_fact);
    if (texture != nullptr) {
        rectNode->setTexture(texture);
        rectNode->setRect(QRectF(0, 0, width(), height()));
    }
    return rectNode;
}

void RubberPreviewRender::createImage(const QVariantMap &qvparams) {
    preparePreviewParams(qvparams);
    auto params_wrapper = createParams();
    image_watcher_ = std::make_unique<ImageFutureWatcher>();
    QObject::connect(image_watcher_.get(), &ImageFutureWatcher::finished,
                     [this]() { saveImage(); });
    image_future_ = std::make_unique<ImageFuture>(
        QtConcurrent::run(core::gui::prepareImage, params_wrapper));
    image_watcher_->setFuture(*image_future_);
}

void RubberPreviewRender::saveImage() {
    if (image_future_ && image_future_->isValid()) {
        result_ = image_future_->takeResult();
    }
    if (result_ && result_->image_ && result_->image_->width() != 0) {
        emit rubberImageReady();
        return;
    }
    qWarning() << "RubberPreviewRender: render failed";
    emit rubberBadResult();
}

/// @brief prepare rubber preview params for later use
void RubberPreviewRender::preparePreviewParams(const QVariantMap &qvparams) {
    if (qvparams.contains("stamp_width")) {
        // params_.stamp_width = qvparams.value("stamp_width").toUInt();
        params_.stamp_width = 900;
    }
    if (qvparams.contains("stamp_height")) {
        // params_.stamp_height = qvparams.value("height").toUInt();
        params_.stamp_height = requested_height_;
    }
    if (qvparams.contains("border_width")) {
        params_.border_width = qvparams.value("border_width").toUInt();
    }
    if (qvparams.contains("border_radius")) {
        params_.border_radius = qvparams.value("border_radius").toUInt();
    }
    if (qvparams.contains("bg_transparent")) {
        params_.bg_transparent = qvparams.value("bg_transparent").toBool();
        // params_.bg_transparent = false;
    }
    if (qvparams.contains("create_from_image")) {
        params_.create_from_image =
            qvparams.value("create_from_image").toBool();
    }
    if (qvparams.contains("stamp_preserve_ratio")) {
        // params_.stamp_preserve_ratio =
        // qvparams.value("stamp_preserve_ratio").toBool();
        params_.stamp_preserve_ratio = true;
    }
    if (qvparams.contains("bg_opacity")) {
        params_.bg_opacity = qvparams.value("bg_opacity").toUInt();
    }
    if (qvparams.contains("font_size")) {
        params_.font_size = qvparams.value("font_size").toUInt();
    }
    if (qvparams.contains("img_path")) {
        params_.img_path = qvparams.value("img_path").toUrl().toLocalFile();
        if (params_.img_path.isEmpty()) {
            params_.img_path = qvparams.value("img_path").toString();
        }
    }
    if (qvparams.contains("annotation_text")) {
        params_.annotation_text = qvparams.value("annotation_text").toString();
    }
    if (qvparams.contains("font_family")) {
        params_.font_family = qvparams.value("font_family").toString();
        int default_weight = requested_width_;
        int tmp_weight = 0;
        const QStringList styles = QFontDatabase::styles(params_.font_family);
        qsizetype index_regular =
            styles.indexOf("regular", Qt::CaseInsensitive);
        if (index_regular < 0 && !styles.empty()) {
            index_regular = 0;
        }
        tmp_weight = default_weight = QFontDatabase::weight(
            params_.font_family, styles.value(index_regular));
        if (tmp_weight > 0 && tmp_weight <= 1000) {
            default_weight = tmp_weight;
        }
        params_.font_weight = default_weight;
        // qWarning() << "weight:" << params_.font_weight;
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
    if (qvparams.contains("bg_color_red")) {
        params_.bg_color.R = qvparams.value("bg_color_red").toUInt();
    }
    if (qvparams.contains("bg_color_green")) {
        params_.bg_color.G = qvparams.value("bg_color_green").toUInt();
    }
    if (qvparams.contains("bg_color_blue")) {
        params_.bg_color.B = qvparams.value("bg_color_blue").toUInt();
    }
}

/// @brief Gather all parameters (pdfcsp::pdf::RubberStampParams)
core::gui::SharedRubberParamWrapper RubberPreviewRender::createParams() const {
    auto params_wrapper = std::make_shared<core::gui::CRubberParamsWrapper>();
    pdfcsp::pdf::RubberStampParams &pod_params = params_wrapper->pod_params;
    params_wrapper->qb_img_path = params_.img_path.toUtf8();
    if (!params_wrapper->qb_img_path.isEmpty()) {
        pod_params.src_img_path = params_wrapper->qb_img_path.data();
    }
    pod_params.target_x = static_cast<uint64_t>(params_.stamp_width);
    pod_params.target_y = static_cast<uint64_t>(params_.stamp_height);
    pod_params.stamp_preserve_ratio = params_.stamp_preserve_ratio;
    pod_params.create_from_image = params_.create_from_image;
    params_wrapper->qb_annotation_text = params_.annotation_text.toUtf8();
    if (!params_wrapper->qb_annotation_text.isEmpty()) {
        pod_params.annotation_text = params_wrapper->qb_annotation_text.data();
    }
    pod_params.bg_color.red = params_.bg_color.R;
    pod_params.bg_color.green = params_.bg_color.G;
    pod_params.bg_color.blue = params_.bg_color.B;
    pod_params.font_color.red = params_.text_color.R;
    pod_params.font_color.green = params_.text_color.G;
    pod_params.font_color.blue = params_.text_color.B;
    pod_params.border_color.red = params_.border_color.R;
    pod_params.border_color.green = params_.border_color.G;
    pod_params.border_color.blue = params_.border_color.B;
    params_wrapper->qb_font_family = params_.font_family.toUtf8();
    if (!params_wrapper->qb_font_family.isEmpty()) {
        pod_params.font_family = params_wrapper->qb_font_family.data();
    }
    pod_params.border_radius = params_.border_radius;
    pod_params.border_width = params_.border_width;
    pod_params.font_size = 1;  // params_.font_size;
    pod_params.font_weight = params_.font_weight;
    pod_params.bg_transparent = params_.bg_transparent;
    pod_params.bg_opacity = params_.bg_opacity;
    pod_params.annotation_width = requested_width_;
    return params_wrapper;
}

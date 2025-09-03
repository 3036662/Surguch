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

    if (result_ == nullptr || result_->image_ == nullptr) {
        // Create an empty image if it does not exist.
        if (!blank_image_ || blank_image_->width() != width() ||
            blank_image_->height() != height()) {
            blank_image_ = std::make_unique<QImage>(width(), height(),
                                                    QImage::Format_RGB888);
            blank_image_->fill(Qt::white);
        }
        QSGTexture *texture = nullptr;
        if (blank_image_) {
            texture = window()->createTextureFromImage(*blank_image_);
        }
        if (texture != nullptr) {
            rectNode->setTexture(texture);
            rectNode->setRect(QRectF(0, 0, width(), height()));
        }
        return rectNode;
    }
    double scale_fact = 1.0;

    const auto yx_ratio_result =
        static_cast<double>(result_->data_->resolution_y) /
        result_->data_->resolution_x;
    auto target_height_qml = yx_ratio_result * max_width_;

    if (target_height_qml > max_height_) {
        scale_fact = max_height_ / target_height_qml;
    }
    target_height_qml*=scale_fact;
    const auto target_width_qml = target_height_qml/yx_ratio_result;

    const auto img_tmp = result_->image_->scaled(
        static_cast<int>(result_->image_->width() * scale_fact),
        static_cast<int>(result_->image_->height() * scale_fact),
        Qt::KeepAspectRatio);
    QSGTexture *texture = window()->createTextureFromImage(img_tmp);
    setWidth(target_width_qml);
    setHeight(target_height_qml);
    if (texture != nullptr) {
        rectNode->setTexture(texture);
        rectNode->setRect(QRectF(0, 0, width(), height()));
    }

    return rectNode;
}

void PreviewRender::createImage(const QVariantMap &qvparams) {
    params_ = core::gui::preparePreviewParams(qvparams);
    auto params_wrapper = core::gui::createParams(params_);
    image_watcher_ = std::make_unique<ImageFutureWatcher>();
    QObject::connect(image_watcher_.get(), &ImageFutureWatcher::finished,
                     [this]() {
                         // qWarning() << "finished";
                         saveImage();
                     });
    image_future_ = std::make_unique<ImageFuture>(
        QtConcurrent::run(core::gui::prepareStampImage, params_wrapper));
    image_watcher_->setFuture(*image_future_);
}

void PreviewRender::saveImage() {
    if (image_future_ && image_future_->isValid()) {
        result_ = image_future_->takeResult();
    }
    if (result_ && result_->image_ && result_->image_->width() != 0) {
        const auto yx_ratio = static_cast<double>(result_->image_->height()) /
                              result_->image_->width();
        setHeight(yx_ratio * width());
        emit imageReady();
        return;
    }
    stampPreviewBadResult();
}

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

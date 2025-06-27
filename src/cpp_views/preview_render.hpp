#ifndef PREVIEWRENDERER_HPP
#define PREVIEWRENDERER_HPP

#include <QFuture>
#include <QFutureWatcher>
#include <QImage>
#include <QQuickItem>
#include <QVariant>
#include <memory>

#include "gui_core/gui_utils.hpp"
#include "gui_core/rubber_structs.hpp"

/**
 * @brief QML Item for rendering preview images
 */
class PreviewRender : public QQuickItem {
    Q_OBJECT

   public:
    PreviewRender();
    ~PreviewRender() override = default;
    PreviewRender(const PreviewRender &) = delete;
    PreviewRender(PreviewRender &&) = delete;
    PreviewRender &operator=(const PreviewRender &) = delete;
    PreviewRender &operator=(PreviewRender &&) = delete;

    using ImageFuture = QFuture<std::unique_ptr<core::gui::BakeResult>>;
    using ImageFutureWatcher =
        QFutureWatcher<std::unique_ptr<core::gui::BakeResult>>;

    struct RGBColor {
        quint8 R = 0;
        quint8 G = 0;
        quint8 B = 0;
    };

    /// @brief Create an image with user settings.
    Q_INVOKABLE void createImage(const QVariantMap &qvparams);

    /// @brief perfom the render
    QSGNode *updatePaintNode(
        QSGNode *oldNode,
        QQuickItem::UpdatePaintNodeData *updatePaintNodeData) override;

   signals:

    /// @brief the image is prepared and ready for render
    void imageReady();

    /// @brief signal when received nullptr for preview image
    void errorOnImageGenerate(const QString &err_string);

   private:
    /// @brief Save the constructed image to the class and signal to render it.
    void saveImage();

    core::gui::SignParams params_;
    float dev_pix_ratio_ = 2;
    std::unique_ptr<core::gui::BakeResult> result_;
    std::unique_ptr<ImageFuture> image_future_;
    std::unique_ptr<ImageFutureWatcher> image_watcher_;
};

#endif  // PREVIEWRENDERER_HPP

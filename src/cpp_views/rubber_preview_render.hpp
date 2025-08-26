#ifndef RUBBER_RENDER_HPP
#define RUBBER_RENDER_HPP

#include <QFuture>
#include <QFutureWatcher>
#include <QImage>
#include <QQuickItem>
#include <QVariant>
#include <memory>

#include "gui_core/gui_utils.hpp"
#include "gui_core/rubber_structs.hpp"

/**
 * @brief QML Item for rendering rubber stamp preview images
 */
class RubberPreviewRender : public QQuickItem {
    Q_OBJECT

   public:
    RubberPreviewRender();
    ~RubberPreviewRender() override = default;
    RubberPreviewRender(const RubberPreviewRender &) = delete;
    RubberPreviewRender(RubberPreviewRender &&) = delete;
    RubberPreviewRender &operator=(const RubberPreviewRender &) = delete;
    RubberPreviewRender &operator=(RubberPreviewRender &&) = delete;

    using ImageFuture = QFuture<std::unique_ptr<core::gui::BakeRubberResult>>;
    using ImageFutureWatcher =
        QFutureWatcher<std::unique_ptr<core::gui::BakeRubberResult>>;

    /// @brief Create preview with user settings
    Q_INVOKABLE void createImage(const QVariantMap &qvparams);

    /// @brief preform the render
    QSGNode *updatePaintNode(
        QSGNode *oldNode,
        QQuickItem::UpdatePaintNodeData *updatePaintNodeData) override;

    /// @brief size hint to be set from QML
    Q_PROPERTY(float requestedWidth MEMBER requested_width_);
    /// @brief size hint to be set from QML
    Q_PROPERTY(float requestedHeight MEMBER requested_height_);

   signals:

    /// @brief the image is prepared and ready for render
    void rubberImageReady();

    /// @brief render failed
    void rubberBadResult();

    /// @brief signal when received nullptr for preview image
    void errorOnImageGenerate(const QString &err_string);

   private:
    /// @brief prepare preview params for later use
    void preparePreviewParams(const QVariantMap &qvparams);

    /// @brief Save the constructed image to the class and signal to render it.
    void saveImage();

    /// @brief Gather all parameters (pdfcsp::pdf::CSignParam)
    [[nodiscard]] core::gui::SharedRubberParamWrapper createParams() const;

    core::gui::RubberParams params_;
    float dev_pix_ratio_ = 2;
    std::unique_ptr<core::gui::BakeRubberResult> result_;
    std::unique_ptr<ImageFuture> image_future_;
    std::unique_ptr<ImageFutureWatcher> image_watcher_;
    std::unique_ptr<QImage> blank_image_;

    float requested_width_ = 340;
    float requested_height_ = 280;
};

/// @brief concurrent function to make QImage
std::unique_ptr<core::gui::BakeRubberResult> prepareImage(
    const core::gui::SharedRubberParamWrapper &params);

#endif  // RUBBER_RENDER_HPP

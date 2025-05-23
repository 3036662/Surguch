#ifndef RUBBER_RENDER_HPP
#define RUBBER_RENDER_HPP

#include <QFuture>
#include <QFutureWatcher>
#include <QImage>
#include <QQuickItem>
#include <QVariant>
#include <memory>

#include "pdf_csp_c.hpp"

/// @brief strucute for holding image data and contructed image for renderring
struct BakeRubberResult {
    std::unique_ptr<pdfcsp::pdf::BakeRubberStamResult,
                    void (*)(pdfcsp::pdf::BakeRubberStamResult *)>
        data_;
    std::unique_ptr<QImage> image_;
};

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

    using ImageFuture = QFuture<std::unique_ptr<BakeRubberResult>>;
    using ImageFutureWatcher = QFutureWatcher<std::unique_ptr<BakeRubberResult>>;

    struct RGBColor {
        quint8 R = 0;
        quint8 G = 0;
        quint8 B = 0;
    };

    // utility structure for storing parameters for library
    struct CRubberParamsWrapper {
        QByteArray qb_img_path;
        QByteArray qb_annotation_text;
        QByteArray qb_font_family;
        pdfcsp::pdf::RubberStampParams pod_params;
    };

    using SharedParamWrapper = std::shared_ptr<CRubberParamsWrapper>;

    struct RubberParams {
        bool bg_transparent = true;
        bool create_from_image = false;
        bool stamp_preserve_ratio = false;
        quint8 bg_opacity = 0;
        quint64 border_width = 10;
        quint32 border_radius = 10;
        quint64 stamp_width = 0;
        quint64 stamp_height = 0;
        quint64 font_size = 0;
        quint64 font_weight = 0;
        QString img_path;
        QString annotation_text;
        QString font_family;
        RGBColor text_color;
        RGBColor border_color;
        RGBColor bg_color;
    };

    /// @brief Create preview with user settings
    Q_INVOKABLE void createImage(const QVariantMap &qvparams);

    /// @brief preform the render
    QSGNode *updatePaintNode(
        QSGNode *oldNode,
        QQuickItem::UpdatePaintNodeData *updatePaintNodeData) override;

    signals:

     /// @brief the image is prepared and ready for render
     void imageReady();

    /// @brief signal when received nullptr for preview image
    void errorOnImageGenerate(const QString &err_string);

private:
    /// @brief prepare preview params for later use
    void preparePreviewParams(const QVariantMap &qvparams);

    /// @brief Save the constructed image to the class and signal to render it.
    void saveImage();

    /// @brief Gather all parameters (pdfcsp::pdf::CSignParam)
    [[nodiscard]] SharedParamWrapper createParams() const;

    RubberParams params_;
    float dev_pix_ratio_ = 2;
    std::unique_ptr<BakeRubberResult> result_;
    std::unique_ptr<ImageFuture> image_future_;
    std::unique_ptr<ImageFutureWatcher> image_watcher_;
};

/// @brief concurrent function to make QImage
std::unique_ptr<BakeRubberResult> prepareImage(
    const RubberPreviewRender::SharedParamWrapper &params);

#endif //RUBBER_RENDER_HPP

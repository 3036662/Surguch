#ifndef PREVIEWRENDERER_HPP
#define PREVIEWRENDERER_HPP

#include <QQuickItem>
#include <QVariant>
#include <QFuture>
#include <QFutureWatcher>
#include <QImage>

#include <memory>

#include "pdf_csp_c.hpp"

/// @brief strucute for holding image data and contructed image for renderring
struct BakeResult {
    std::unique_ptr<pdfcsp::pdf::BakeSignatureStampResult, void (*)(pdfcsp::pdf::BakeSignatureStampResult *)> data_;
    std::unique_ptr<QImage> image_;
};

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

    using ImageFuture = QFuture<std::unique_ptr<BakeResult>>;
    using ImageFutureWatcher = QFutureWatcher<std::unique_ptr<BakeResult>>;

    struct RGBColor {
        quint8 R = 0;
        quint8 G = 0;
        quint8 B = 0;
    };

    // utility structure for storing parameters for library
    struct CSignParamsWrapper {
        QByteArray qb_logo_path;
        QByteArray qb_config_path;
        QByteArray qb_cert_serial;
        QByteArray qb_cert_serial_prefix;
        QByteArray qb_cert_subject;
        QByteArray qb_cert_subject_prefix;
        QByteArray qb_cert_time_validity;
        QByteArray qb_stamp_type;
        QByteArray qb_cades_type;
        QByteArray qb_file_to_sign_path;
        QString temp_dir;
        QByteArray qb_temp_dir;
        QByteArray qb_stamp_title;
        QByteArray qb_tsp_url;
        pdfcsp::pdf::CSignParams pod_params;
    };

    using SharedParamWrapper = std::shared_ptr<CSignParamsWrapper>;

    struct SignParams {
        bool bg_transparent = true;
        quint8 bg_opacity = 0;
        int page_index = 0;
        quint32 border_width = 10;
        quint32 border_radius = 10;
        qreal page_width = 0;
        qreal page_height = 0;
        qreal stamp_x = 0;
        qreal stamp_y = 0;
        qreal stamp_width = 0;
        qreal stamp_height = 0;
        QString logo_path;
        QString config_path;
        QString cert_serial;
        QString cert_subject;
        QString cert_time_validity;
        QString stamp_type;
        QString cades_type;
        QString file_to_sign_path;
        QString tsp_url;
        QString cert_serial_prefix;
        QString cert_subject_prefix;
        QString stamp_title;
        RGBColor text_color;
        RGBColor border_color;
    };

    /// @brief create image with user settings
    Q_INVOKABLE void createImage(const QVariantMap &qvparams);

    /// @brief perfom the render
    QSGNode *updatePaintNode(
        QSGNode *oldNode,
        QQuickItem::UpdatePaintNodeData *updatePaintNodeData) override;

   signals:

    /// @brief singal when image prepared and ready for render
    void imageReady();

    /// @brief signal when recieved nullptr for preview image
    void errorOnImageGenerate(const QString &err_string);

   private:
    /// @brief prepare preview params for later use
    void preparePreviewParams(const QVariantMap &qvparams);

    /// @brief save constructed image to class and signal to render it
    void saveImage();

    /// @brief Gather all parameters (pdfcsp::pdf::CSignParam)
    [[nodiscard]] SharedParamWrapper createParams() const;


    SignParams params_;
    float dev_pix_ratio_ = 2;
    std::unique_ptr<BakeResult> result_;
    std::unique_ptr<ImageFuture> image_future_;
    std::unique_ptr<ImageFutureWatcher> image_watcher_;
};


/// @brief concurrent function to make QImage
std::unique_ptr<BakeResult> prepareImage(PreviewRender::SharedParamWrapper params);

#endif  // PREVIEWRENDERER_HPP

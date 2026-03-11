/* File: preview_render.hpp
Copyright (C) Basealt LLC,  2025
Author: Daniil-Viktor Ratkin, <ratkinda@basealt.ru>

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program. If not, see <https://www.gnu.org/licenses/>.
*/

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
#include "pdf_csp_c.hpp"

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

    /// @brief size hint to be set from QML
    Q_PROPERTY(float maxWidth MEMBER max_width_);
    /// @brief size hint to be set from QML
    Q_PROPERTY(float maxHeight MEMBER max_height_);

   signals:

    /// @brief the image is prepared and ready for render
    void imageReady();

    /// @brief render failed
    void stampPreviewBadResult();

    /// @brief signal when received nullptr for preview image
    void errorOnImageGenerate(const QString &err_string);

   protected:
    /// @brief perform the render
    QSGNode *updatePaintNode(QSGNode *node,
                             UpdatePaintNodeData *updatePaintNodeData) override;

   private:
    /// @brief Save the constructed image to the class and signal to render it.
    void saveImage();

    core::gui::SignParams params_;
    float dev_pix_ratio_ = 2;
    std::unique_ptr<core::gui::BakeResult> result_;
    std::unique_ptr<ImageFuture> image_future_;
    std::unique_ptr<ImageFutureWatcher> image_watcher_;
    std::unique_ptr<QImage> blank_image_;

    float max_width_ = 460;   // set from QML
    float max_height_ = 175;  // set from QML
};

#endif  // PREVIEWRENDERER_HPP

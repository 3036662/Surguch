/* File: pdf_page_render.cpp
Copyright (C) Basealt LLC,  2024-2025
Author: Oleg Proskurin, <proskurinov@basealt.ru>

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

#include "pdf_page_render.hpp"

#include <QPainter>
#include <QSGNode>
#include <QSGSimpleTextureNode>
#include <QScreen>
#include <QThread>
#include <QtMath>
#include <memory>
#include <shared_mutex>

#include "gui_core/gui_utils.hpp"

PdfPageRender::PdfPageRender() {
    setFlag(ItemHasContents, true);
    setClip(true);
    if (const qreal pix_rat = QWindow().devicePixelRatio(); pix_rat > 2) {
        dev_pix_ratio_ = static_cast<float>(pix_rat);
    }
}

/**
 * @brief Handle the geometry change
 * @details Makes a decision if we need to rerender the whole page.
 */
void PdfPageRender::geometryChange(const QRectF &newGeometry,
                                   const QRectF &oldGeometry) {
    bool needs_new_rendering = false;
    if (oldGeometry != newGeometry && pwidth_ > 1 && zoom_dpi_last_ > 0) {
        const qreal new_zoom_dpi_ = width() / pwidth_;
        const double zoom_change =
            qFabs(new_zoom_dpi_ - zoom_dpi_last_) / zoom_dpi_last_;
        if (zoom_change > 0.1 && isVisible()) {
            needs_new_rendering = true;
            // qWarning() << "zoom change" << zoom_change * 100 << "%";
        }
    }
    if (needs_new_rendering && isVisible()) {
        image_.reset();
    }
    update();
    QQuickItem::geometryChange(newGeometry, oldGeometry);
}

/// @brief perform the render
// NOLINTBEGIN(cppcoreguidelines-owning-memory)
QSGNode *PdfPageRender::updatePaintNode(
    QSGNode *node, [[maybe_unused]] UpdatePaintNodeData *updatePaintNodeData) {
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
        rectNode = new QSGSimpleTextureNode();
        rectNode->setFiltering(QSGTexture::Linear);
        rectNode->setOwnsTexture(true);
    }
    // qWarning() << "PdfPageRender: render page" << page_number_;
    if (!image_) {
        try {
            core::MuPageRender mupdf(fzctx_, fzdoc_);
            mupdf.SetNeedleRects(needles_);
            const core::RenderRes render_result = mupdf.RenderPage(
                page_number_, custom_rotation_, /* width(),*/ dev_pix_ratio_,
                width_goal_, zoom_goal_, screen_dpi_);
            if (render_result.buf == nullptr) {
                throw std::runtime_error(
                    "[PdfPageRender] failed to render the page");
            }
            if (render_result.page_width > 0) {
                setWidth(render_result.page_width);
            }
            if (render_result.page_height > 0) {
                setHeight(render_result.page_height);
            }
            pwidth_ = render_result.page_width;
            pheight_ = render_result.page_height;
            if (pheight_ > 0) {
                pratio_ = pwidth_ / pheight_;
            }
            zoom_dpi_last_ = render_result.zoom_dpi;
            result_zoom_last_ = render_result.result_zoom;
            // qWarning()<<"result_zoom_last"<<result_zoom_last_;
            image_ = std::make_unique<QImage>(
                render_result.buf, width() * dev_pix_ratio_,
                height() * dev_pix_ratio_, render_result.pix_stride,
                QImage::Format_RGB888,
                [](void *vbuf) {
                    const auto *buff = static_cast<unsigned char *>(vbuf);
                    delete[] buff;
                },
                render_result.buf);
            renderRubberStamps();
        } catch (const std::exception &ex) {
            qWarning() << "[PdfPageRender] " << ex.what();
            image_ = std::make_unique<QImage>(size().toSize(),
                                              QImage::Format_RGB888);
            image_->fill(Qt::white);  // Fill the image with white color
        }
    }
    // Create a texture from the image
    if (QSGTexture *texture = window()->createTextureFromImage(*image_);
        texture != nullptr) {
        rectNode->setTexture(texture);
        rectNode->setRect(QRectF(0, 0, width(), height()));
    }
    return rectNode;
}
// NOLINTEND(cppcoreguidelines-owning-memory)

/// @brief render rubber stamps on top of page
void PdfPageRender::renderRubberStamps() {
    const std::scoped_lock lock(mutex_);
    if (!rubber_stamps_.empty()) {
        for (const auto &stamps_ : rubber_stamps_) {
            if (stamps_->res && stamps_->res->image_ != nullptr) {
                // auto start =
                // std::chrono::high_resolution_clock::now();
                QPainter painter(image_.get());
                // rotate the coordinate system
                painter.rotate(custom_rotation_);
                // move the coordinate system
                int img_width = image_->width();
                int img_height = image_->height();
                switch (static_cast<int>(custom_rotation_)) {
                    case 90:
                        painter.translate(0, -image_->width());
                        std::swap(img_height, img_width);
                        break;
                    case 180:
                        painter.translate(-image_->width(), -image_->height());
                        break;
                    case 270:
                        painter.translate(-image_->height(), 0);
                        std::swap(img_height, img_width);
                        break;
                    default:
                        painter.translate(0, 0);
                        break;
                }

                const double ratio =
                    static_cast<double>(stamps_->res->data_->resolution_x) /
                    static_cast<double>(stamps_->res->data_->resolution_y);
                const int target_width =
                    static_cast<int>(stamps_->real_stamp_qml_width /
                                     stamps_->qml_width * img_width);
                const int target_height =
                    static_cast<int>(stamps_->real_stamp_qml_width / ratio /
                                     stamps_->qml_height * img_height);

                const QImage stamp_scaled = stamps_->res->image_->scaled(
                    target_width, target_height, Qt::KeepAspectRatio);
                painter.drawImage(
                    static_cast<int>(stamps_->position_x / stamps_->qml_width *
                                     img_width),
                    static_cast<int>(stamps_->position_y / stamps_->qml_height *
                                     img_height),
                    stamp_scaled);
                // auto end =
                // std::chrono::high_resolution_clock::now();
                // std::chrono::duration<double, std::milli>
                // duration =123
                //     end - start;
                // qWarning() << "pos x: " << stamps_->position_x;
                // qWarning() << "pos y: " << stamps_->position_y;
            }
        }
    }
}

/// setters to connect with the low-level MuPdf from pdf_doc_model
void PdfPageRender::setDoc(fz_document *fzdoc) { fzdoc_ = fzdoc; }
void PdfPageRender::setCtx(fz_context *fzctx) { fzctx_ = fzctx; }

/// @brief set index of a page to render
void PdfPageRender::setPageNumber(const int page_number) {
    page_number_ = page_number;
}

/// @brief set rectangles to highlight the needles
/// @details does not own the pointer
void PdfPageRender::setNeedleHighlightRects(
    core::utils::NeedleRectsOnPage needles) {
    needles_ = std::move(needles);
}

void PdfPageRender::setCurrentNeedleRect(
    const std::shared_ptr<std::pair<size_t, fz_rect>> &val) {
    if (!needles_) {
        return;
    }
    if (!val || page_number_ != val->first) {
        needles_->highlight_current = false;
        image_.reset();
        return;
    }
    needles_->highlight_current = true;
    needles_->current = val->second;
    image_.reset();
};

void PdfPageRender::setRubberStamps(
    std::vector<std::shared_ptr<core::gui::RubberStamp>> rubber_stamps) {
    const std::scoped_lock lock(mutex_);
    std::swap(rubber_stamps_, rubber_stamps);
    image_.reset();
    update();
}

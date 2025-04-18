/* File: pdf_page_render.cpp
Copyright (C) Basealt LLC,  2024
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
#include <QSGGeometryNode>
#include <QSGNode>
#include <QSGSimpleTextureNode>
#include <QScreen>
#include <QThread>
#include <QtMath>
#include <memory>

PdfPageRender::PdfPageRender() {
    setFlag(QQuickItem::ItemHasContents, true);
    setClip(true);
    const qreal pix_rat = QWindow().devicePixelRatio();
    if (pix_rat > 2) {
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

/// @brief perfom the render
// NOLINTBEGIN(cppcoreguidelines-owning-memory)
QSGNode *PdfPageRender::updatePaintNode(
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
        rectNode = new QSGSimpleTextureNode();
        rectNode->setFiltering(QSGTexture::Linear);
        rectNode->setOwnsTexture(true);
    }
    if (!image_) {
        try {
            const core::MuPageRender mupdf(fzctx_, fzdoc_);
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
                    auto *buff = static_cast<unsigned char *>(vbuf);
                    delete[] buff;
                },
                render_result.buf);
        } catch (const std::exception &ex) {
            qWarning() << "[PdfPageRender] " << ex.what();
            image_ = std::make_unique<QImage>(size().toSize(),
                                              QImage::Format_RGB888);
            image_->fill(Qt::white);  // Fill the image with white color
        }
    }
    // Create a texture from the image
    QSGTexture *texture = window()->createTextureFromImage(*image_);
    if (texture != nullptr) {
        rectNode->setTexture(texture);
        rectNode->setRect(QRectF(0, 0, width(), height()));
    }
    return rectNode;
}
// NOLINTEND(cppcoreguidelines-owning-memory)

/// setters to connect with the low-level MuPdf from pdf_doc_model
void PdfPageRender::setDoc(fz_document *fzdoc) { fzdoc_ = fzdoc; }
void PdfPageRender::setCtx(fz_context *fzctx) { fzctx_ = fzctx; }

/// @brief set index of a page to render
void PdfPageRender::setPageNumber(int page_number) {
    page_number_ = page_number;
}

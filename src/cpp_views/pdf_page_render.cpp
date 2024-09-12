#include "pdf_page_render.hpp"
#include "mupdf/fitz.h"
#include <QPainter>
#include <QSGGeometryNode>
#include <QSGNode>
#include <QSGSimpleTextureNode>

PdfPageRender::PdfPageRender() { setFlag(QQuickItem::ItemHasContents, true); }

QSGNode *PdfPageRender::updatePaintNode(
    QSGNode *node, QQuickItem::UpdatePaintNodeData *updatePaintNodeData) {
  // TODO(Oleg) implement
  /*
   * The function should return the root of the scene graph subtree for this
   * item. Most implementations will return a single QSGGeometryNode
   */
  QSGSimpleTextureNode *rectNode = nullptr;
  if (node != nullptr) {
    rectNode = static_cast<QSGSimpleTextureNode *>(node);
    rectNode->setOwnsTexture(true);
  } else {
    if (!size().isValid()){
        return node;
    }
    rectNode = new QSGSimpleTextureNode();
  }
  // Create a QImage to fill with white color
  // QImage image(size().toSize(), QImage::Format_RGB888);
  // image.fill(Qt::white); // Fill the image with white color

  fz_device *draw_device=nullptr;
  fz_pixmap *pixmap=nullptr;
  fz_separations *separation =nullptr;
  fz_page *page =nullptr;
  QImage image;
  if (fzctx_ != nullptr && fzdoc_ != nullptr) {
    fz_try(fzctx_){
    page = fz_load_page(fzctx_, fzdoc_, page_number_); // TODO drop
    fz_colorspace *color_space = fz_document_output_intent(fzctx_, fzdoc_);
    separation = fz_page_separations(fzctx_, page); // TODO drop
    const fz_rect page_rect = fz_bound_page(fzctx_, page);
    if (color_space == nullptr) {
      color_space = fz_device_rgb(fzctx_);
    }
    const int pix_width = static_cast<int>(width());
    const int pix_height = static_cast<int>(height());
    pixmap = fz_new_pixmap(fzctx_, color_space, pix_width,
                                      pix_height, separation, 0);
    fz_clear_pixmap(fzctx_, pixmap);
    float scale_factor = pix_width / (page_rect.x1 - page_rect.x0);
    fz_matrix transmorm{scale_factor, 0, 0, scale_factor, 0, 0};
    draw_device = fz_new_draw_device(fzctx_, transmorm, pixmap);
    fz_run_page(fzctx_, page, draw_device, fz_identity, nullptr);
    const ptrdiff_t pix_stride = pixmap->stride;
    unsigned char *samples = pixmap->samples;
    auto buff_size = pix_stride * pix_height;

    // unsigned char* buff= new unsigned char[buff_size + 1];
    std::vector<unsigned char> buff(pix_stride * pix_height, 0);
    std::copy(samples, samples + buff_size + 1, buff.data());
    image = QImage(buff.data(), pix_width, pix_height, pix_stride,
                   QImage::Format_RGB888);
    }
    fz_always(fzctx_){
    // clean up
        if (draw_device != nullptr) {
          fz_close_device(fzctx_, draw_device);
          fz_drop_device(fzctx_, draw_device);
        }
        if (pixmap != nullptr) {
          fz_drop_pixmap(fzctx_, pixmap);
        }
        if (separation != nullptr) {
          fz_drop_separations(fzctx_, separation);
        }
        if (page != nullptr) {
          fz_drop_page(fzctx_, page);
        }
    }
    fz_catch(fzctx_){
          qWarning()<<fz_caught_message(fzctx_);
    }

  } else {
    image = QImage(size().toSize(), QImage::Format_RGB888);
    image.fill(Qt::white); // Fill the image with white color
  }

  // Create a texture from the image
  QSGTexture *texture = window()->createTextureFromImage(image);
  rectNode->setTexture(texture);

  // Add the rectangle node to the main node
  // node->appendChildNode(rectNode);

  // Set the size of the rectangle to match the item's size
  rectNode->setRect(QRectF(0, 0, width(), height()));

  return rectNode; // Return the updated node
}

void PdfPageRender::setDoc(fz_document *fzdoc) { fzdoc_ = fzdoc; }

void PdfPageRender::setCtx(fz_context *fzctx) { fzctx_ = fzctx; }

void PdfPageRender::setPageNumber(int page_number) {
  page_number_ = page_number;
}

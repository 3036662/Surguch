#include "pdf_page_render.hpp"
#include "mupdf/fitz.h"
#include <QPainter>
#include <QSGGeometryNode>
#include <QSGNode>
#include <QSGSimpleTextureNode>
#include <QThread>
#include <QtMath>

PdfPageRender::PdfPageRender() {
  setFlag(QQuickItem::ItemHasContents, true);
  setClip(true);
  qWarning() << "constructed";
  qWarning() << "Current Thread ID:"
             << QThread::currentThread()->currentThreadId();
}

PdfPageRender::~PdfPageRender() {
  qWarning() << "Page " << page_number_ << " Destructed";
}

void PdfPageRender::geometryChange(const QRectF &newGeometry,
                                   const QRectF &oldGeometry) {

  bool needs_new_rendering = false;
  if (oldGeometry != newGeometry && pwidth_ > 1 && zoom_dpi_last_ > 0) {
    float new_zoom_dpi_ = width() / pwidth_;
    double zoom_change = qFabs(new_zoom_dpi_ - zoom_dpi_last_) / zoom_dpi_last_;
    if (zoom_change > 0.1) {
      needs_new_rendering = true;
      qWarning() << "zoom change" << zoom_change*100  << "%";
    }
  }
  if (needs_new_rendering) {
    image_.reset();
    qWarning() << "reset page " << page_number_;
  }
  update();
  QQuickItem::geometryChange(newGeometry, oldGeometry);
}

QSGNode *PdfPageRender::updatePaintNode(
    QSGNode *node, QQuickItem::UpdatePaintNodeData *updatePaintNodeData) {
  QSGSimpleTextureNode *rectNode = nullptr;

  if (node != nullptr) {
    rectNode = static_cast<QSGSimpleTextureNode *>(node);
    //delete rectNode->texture();
  } else {
    if (!size().isValid()) {
      return node;
    }
    rectNode = new QSGSimpleTextureNode();
    rectNode->setFiltering(QSGTexture::Linear);
    rectNode->setOwnsTexture(true);
  }

  fz_device *draw_device = nullptr;
  fz_pixmap *pixmap = nullptr;
  fz_separations *separation = nullptr;
  fz_page *page = nullptr;
  int page_width = 0;
  int page_height = 0;

  fz_var(draw_device);
  fz_var(pixmap);
  fz_var(separation);
  fz_var(page);
  if (!image_.isNull()) {
    qWarning() << "return same image for page" << page_number_;
  }
  if ( fzctx_ != nullptr && fzdoc_ != nullptr && image_.isNull()) {
    fz_try(fzctx_) {
      page = fz_load_page(fzctx_, fzdoc_, page_number_);
      separation = fz_page_separations(fzctx_, page);
      fz_rect page_rect = fz_bound_page(fzctx_, page);
      fz_colorspace *color_space = fz_device_rgb(fzctx_);
      // choose a zoom factor
      const qreal device_dpi = QWindow().devicePixelRatio();
      float zoom_dpi = 1; // 72 dpi x zoom
      pwidth_ = page_rect.x1 - page_rect.x0;
      if (pwidth_ > 1) {
        zoom_dpi = (width() /pwidth_);
        qWarning() << "zoom dpi" << zoom_dpi;
        qWarning() << "dpi" << int(zoom_dpi*72);
      }
      page_width = (page_rect.x1 - page_rect.x0) * zoom_dpi;
      page_height = (page_rect.y1 - page_rect.y0) * zoom_dpi;
      qWarning() << " page width= " << page_width << "page_height "
                 << page_height;
      float page_ratio = 1.43f;
      if (page_width != 0) {
        page_ratio = page_height / static_cast<float>(page_width);
      }
      qWarning() << "page ratio " << page_ratio;
      setHeight(width() * page_ratio); // set qml item ratio
      qWarning() << " element width= " << width() << "element_height "
                 << height();
      qreal pixmap_width= width() * device_dpi;
      qreal pixmap_height= height() * device_dpi;
      pixmap = fz_new_pixmap(fzctx_, color_space,pixmap_width,
                             pixmap_height, separation, 0);
      fz_clear_pixmap(fzctx_, pixmap);
      qWarning() << " pixmap width= " << page_width << "pixmax height "
                 << page_height;

      float scale_factor_x = page_width > 0 ? width() / page_width*device_dpi : 1;
      float scale_factor_y = page_height > 0 ? height() / page_height*device_dpi : 1;

      qWarning() << " scale_factor = " << scale_factor_x << "scale_factor_y"
                 << scale_factor_y;

      if (scale_factor_x > 100 || scale_factor_y > 100 || scale_factor_x == 0 ||
          scale_factor_y == 0) {
        qWarning() << "Pdf scale factor is greater than 100,set to 1";
        scale_factor_x = 1.0f;
        scale_factor_y = 1.0f;
      }

      fz_matrix transform_run_page = fz_scale(zoom_dpi, zoom_dpi);
      fz_matrix transform_device = fz_scale(scale_factor_x, scale_factor_x);
      draw_device =
          fz_new_draw_device(fzctx_, transform_device, pixmap); // fz_identity
      fz_run_page(fzctx_, page, draw_device, transform_run_page, nullptr);
      ptrdiff_t pix_stride = pixmap->stride;
      unsigned char *samples = pixmap->samples;
      auto buff_size = pix_stride * pixmap->h;
      qWarning() << "buff size " << buff_size;

      unsigned char *buff = new unsigned char[buff_size + 1];
      std::copy(samples, samples + buff_size + 1, buff);
      image_.reset(
          new QImage(buff, width() * device_dpi, height() * device_dpi,
                     pix_stride, QImage::Format_RGB888, [](void *vbuf) {
                       unsigned char *buff = static_cast<unsigned char *>(vbuf);
                       delete[] buff;
                       //qWarning()<<"free buffer"<<vbuf;
                     },buff));


      zoom_dpi_last_ = zoom_dpi;
    }
    fz_always(fzctx_) {
      // clean up
        fz_close_device(fzctx_, draw_device);
        fz_drop_device(fzctx_, draw_device);
        fz_drop_pixmap(fzctx_, pixmap);
        fz_drop_separations(fzctx_, separation);
        fz_drop_page(fzctx_, page);

    }
    fz_catch(fzctx_) { qWarning() << fz_caught_message(fzctx_); }
  } else if (image_.isNull()) {
    image_.reset(new QImage(size().toSize(), QImage::Format_RGB888));
    image_->fill(Qt::white); // Fill the image with white color
  }

  // Create a texture from the image
  QSGTexture *texture = window()->createTextureFromImage(*image_);
  qWarning() << "texture height" << texture->textureSize().height();
  qWarning() << "texture height" << texture->textureSize().width();
  rectNode->setTexture(texture);
  rectNode->setRect(QRectF(0, 0, width(), height()));
  //rectNode->setOwnsTexture(true);

  // Set the size of the rectangle to match the item's size
  return rectNode; // Return the updated node
}

void PdfPageRender::setDoc(fz_document *fzdoc) { fzdoc_ = fzdoc; }

void PdfPageRender::setCtx(fz_context *fzctx) { fzctx_ = fzctx; }

void PdfPageRender::setPageNumber(int page_number) {
  page_number_ = page_number;
}

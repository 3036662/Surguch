#ifndef PDF_PAGE_RENDER_HPP
#define PDF_PAGE_RENDER_HPP

#include "mupdf/fitz.h"
#include <QImage>
#include <QQuickItem>
#include <QScopedPointer>

class PdfPageRender : public QQuickItem {
  Q_OBJECT
public:
  PdfPageRender();
  ~PdfPageRender();

  QSGNode *updatePaintNode(
      QSGNode *oldNode,
      QQuickItem::UpdatePaintNodeData *updatePaintNodeData) override;

  void geometryChange(const QRectF &newGeometry,
                      const QRectF &oldGeometry) override;

  Q_INVOKABLE void setDoc(fz_document *fzdoc);
  Q_INVOKABLE void setCtx(fz_context *fzctx);
  Q_INVOKABLE void setPageNumber(int page_number);


signals:

private:
  fz_context *fzctx_ = nullptr;
  fz_document *fzdoc_ = nullptr;
  int page_number_ = 0;
  QScopedPointer<QImage> image_;
  float zoom_dpi_last_ = 1;
  float pwidth_ = 0;
  float dev_pix_ratio_ = 2;
};

#endif // PDF_PAGE_RENDER_HPP

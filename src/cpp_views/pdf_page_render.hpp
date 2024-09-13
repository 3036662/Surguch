#ifndef PDF_PAGE_RENDER_HPP
#define PDF_PAGE_RENDER_HPP

 #include <QQuickItem>
#include <QScopedPointer>
#include <QImage>
 #include "mupdf/fitz.h"

class PdfPageRender : public QQuickItem {
   Q_OBJECT
public:
   PdfPageRender();
   ~PdfPageRender();

  QSGNode *updatePaintNode(
      QSGNode *oldNode,
      QQuickItem::UpdatePaintNodeData *updatePaintNodeData) override;

  void geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry) override;

   Q_INVOKABLE void setDoc(fz_document* fzdoc);
   Q_INVOKABLE void setCtx(fz_context* fzctx);
   Q_INVOKABLE void setPageNumber(int page_number); 

signals:

private:
   fz_context* fzctx_=nullptr;
   fz_document* fzdoc_=nullptr;
   int page_number_=0;
   QScopedPointer<QImage> image_;
   QList<unsigned char> buff_;
   float zoom_dpi_last_=1;
   float pwidth_=0;

 };

#endif // PDF_PAGE_RENDER_HPP

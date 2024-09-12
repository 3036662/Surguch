#ifndef PDF_PAGE_RENDER_HPP
#define PDF_PAGE_RENDER_HPP

 #include <QQuickItem>
 #include "mupdf/fitz.h"

class PdfPageRender : public QQuickItem {
   Q_OBJECT
public:
   PdfPageRender();

  QSGNode *updatePaintNode(
      QSGNode *oldNode,
      QQuickItem::UpdatePaintNodeData *updatePaintNodeData) override;

   Q_INVOKABLE void setDoc(fz_document* fzdoc);
   Q_INVOKABLE void setCtx(fz_context* fzctx);
   Q_INVOKABLE void setPageNumber(int page_number);

signals:

private:
   fz_context* fzctx_=nullptr;
   fz_document* fzdoc_=nullptr;
   int page_number_=0;
 };

#endif // PDF_PAGE_RENDER_HPP

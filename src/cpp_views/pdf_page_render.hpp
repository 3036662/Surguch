#ifndef PDF_PAGE_RENDER_HPP
#define PDF_PAGE_RENDER_HPP

#include "core/mu_page_render.hpp"
#include "mupdf/fitz.h"
#include <QImage>
#include <QQuickItem>
#include <memory>

class PdfPageRender : public QQuickItem {
  Q_OBJECT
public:
  PdfPageRender();
  ~PdfPageRender() override = default;
  PdfPageRender(const PdfPageRender &) = delete;
  PdfPageRender(PdfPageRender &&) = delete;
  PdfPageRender &operator=(const PdfPageRender &) = delete;
  PdfPageRender &operator=(PdfPageRender &&) = delete;

  QSGNode *updatePaintNode(
      QSGNode *oldNode,
      QQuickItem::UpdatePaintNodeData *updatePaintNodeData) override;

  void geometryChange(const QRectF &newGeometry,
                      const QRectF &oldGeometry) override;




  Q_INVOKABLE void setDoc(fz_document *fzdoc);
  Q_INVOKABLE void setCtx(fz_context *fzctx);
  Q_INVOKABLE void setPageNumber(int page_number);

  Q_PROPERTY (float widthGoal MEMBER width_goal_);
  Q_PROPERTY (float zoomGoal MEMBER zoom_goal_);

  Q_PROPERTY (float currScreenDpi MEMBER screen_dpi_);
  Q_PROPERTY (float zoomLast MEMBER result_zoom_last_ NOTIFY zoomLastChanged);
  Q_PROPERTY (float customRotation MEMBER custom_rotation_);
  Q_PROPERTY (float pageRatio MEMBER pratio_);

signals:
  void zoomLastChanged();

private:
  fz_document *fzdoc_ = nullptr;
  fz_context *fzctx_ = nullptr;
  int page_number_ = 0;
  std::unique_ptr<QImage> image_ = nullptr;
  float zoom_dpi_last_ = 1;
  float pwidth_ = 0;
  float pheight_=0;
  float pratio_=1;

  float dev_pix_ratio_ = 2;

  float width_goal_=0;
  float zoom_goal_=1;
  float screen_dpi_=72;
  float result_zoom_last_=1;


  int custom_rotation_=0;

};

#endif // PDF_PAGE_RENDER_HPP

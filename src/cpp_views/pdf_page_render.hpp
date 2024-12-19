/* File: pdf_page_render.hpp
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

#ifndef PDF_PAGE_RENDER_HPP
#define PDF_PAGE_RENDER_HPP

#include "core/mu_page_render.hpp"
#include "mupdf/fitz.h"
#include <QImage>
#include <QQuickItem>
#include <memory>

/**
 * @brief QML Item for rendering PDF pages
 */
class PdfPageRender : public QQuickItem {
  Q_OBJECT

public:
  PdfPageRender();
  ~PdfPageRender() override = default;
  PdfPageRender(const PdfPageRender &) = delete;
  PdfPageRender(PdfPageRender &&) = delete;
  PdfPageRender &operator=(const PdfPageRender &) = delete;
  PdfPageRender &operator=(PdfPageRender &&) = delete;

  /**
   * @brief Handle the geometry change
   * @details Makes a decision if we need to rerender the whole page.
   */
  void geometryChange(const QRectF &newGeometry,
                      const QRectF &oldGeometry) override;

  /// @brief perfom the render
  QSGNode *updatePaintNode(
      QSGNode *oldNode,
      QQuickItem::UpdatePaintNodeData *updatePaintNodeData) override;

  /// setters to connect with the low-level MuPdf from pdf_doc_model
  Q_INVOKABLE void setDoc(fz_document *fzdoc);
  Q_INVOKABLE void setCtx(fz_context *fzctx);
  /// @brief set index of a page to render
  Q_INVOKABLE void setPageNumber(int page_number);

  /// @brief the goal with of element
  Q_PROPERTY(float widthGoal MEMBER width_goal_);
  /// @brief the goal zoom of element
  Q_PROPERTY(float zoomGoal MEMBER zoom_goal_);
  /// @brief current screen DPI setter
  Q_PROPERTY(float currScreenDpi MEMBER screen_dpi_);
  /// @brief the last zoom value for zoomIn/zoomOut
  Q_PROPERTY(float zoomLast MEMBER result_zoom_last_ NOTIFY zoomLastChanged);
  /// @brief rotation value in degrees
  Q_PROPERTY(float customRotation MEMBER custom_rotation_);
  /// @brief Is utilized to set the page height on page width change.
  Q_PROPERTY(float pageRatio MEMBER pratio_);

signals:

  /// @brief Notification signal for zoomLast changes
  void zoomLastChanged();

private:
  fz_document *fzdoc_ = nullptr;
  fz_context *fzctx_ = nullptr;
  int page_number_ = 0;
  std::unique_ptr<QImage> image_ = nullptr;
  float zoom_dpi_last_ = 1;
  float pwidth_ = 0;
  float pheight_ = 0;
  float pratio_ = 1;
  float dev_pix_ratio_ = 2;
  float width_goal_ = 0;
  float zoom_goal_ = 1;
  float screen_dpi_ = 72;
  float result_zoom_last_ = 1;
  float custom_rotation_ = 0;
};

#endif // PDF_PAGE_RENDER_HPP

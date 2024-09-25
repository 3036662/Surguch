#ifndef PDF_OBJ_KEEPER_HPP
#define PDF_OBJ_KEEPER_HPP
#include "mupdf/pdf.h"

namespace core {

/*!
 * \brief The PdfObjKeeper - wrapper for pdf_obj raw pointer
 * \throws runtimer error if fzctx == nullptr on construction
 */
class PdfObjKeeper {
public:
  PdfObjKeeper(fz_context *fzctx, pdf_obj *obj);
  PdfObjKeeper(const PdfObjKeeper &other) noexcept;
  PdfObjKeeper(PdfObjKeeper &&other) noexcept;
  PdfObjKeeper &operator=(const PdfObjKeeper &other) noexcept;
  PdfObjKeeper &operator=(PdfObjKeeper &&other) noexcept;
  ~PdfObjKeeper();

  [[nodiscard]] pdf_obj *get() const noexcept;

private:
  fz_context *fzctx_;
  pdf_obj *obj_;
};

} // namespace core

#endif // PDF_OBJ_KEEPER_HPP

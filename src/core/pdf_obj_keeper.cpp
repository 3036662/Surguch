#include "pdf_obj_keeper.hpp"
#include <stdexcept>
namespace core {

PdfObjKeeper::PdfObjKeeper(fz_context *fzctx, pdf_obj *obj)
    : fzctx_(fzctx), obj_(obj) {
  if (fzctx_ == nullptr) {
    throw std::runtime_error("nullptr instead of context");
  }
  if (obj != nullptr) {
    pdf_keep_obj(fzctx_, obj);
  }
}

PdfObjKeeper::PdfObjKeeper(const PdfObjKeeper &other) noexcept
    : fzctx_(other.fzctx_), obj_(other.obj_) {
  if (obj_ != nullptr) {
    pdf_keep_obj(fzctx_, obj_);
  }
}

PdfObjKeeper::PdfObjKeeper(PdfObjKeeper &&other) noexcept
    : fzctx_(other.fzctx_), obj_(other.obj_) {
  other.obj_ = nullptr;
}

PdfObjKeeper &PdfObjKeeper::operator=(const PdfObjKeeper &other) noexcept {
  if (this == &other) {
    return *this;
  }
  pdf_drop_obj(fzctx_, obj_);
  fzctx_ = other.fzctx_;
  obj_ = other.obj_;
  if (obj_ != nullptr) {
    pdf_keep_obj(fzctx_, obj_);
  }
  return *this;
}

PdfObjKeeper &PdfObjKeeper::operator=(PdfObjKeeper &&other) noexcept {
  if (this == &other) {
    return *this;
  }
  pdf_drop_obj(fzctx_, obj_);
  fzctx_ = other.fzctx_;
  obj_ = other.obj_;
  other.obj_ = nullptr;
  return *this;
}

PdfObjKeeper::~PdfObjKeeper() { pdf_drop_obj(fzctx_, obj_); }

pdf_obj *PdfObjKeeper::get() const noexcept { return obj_; }

} // namespace core

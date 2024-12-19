/* File: pdf_obj_keeper.cpp
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

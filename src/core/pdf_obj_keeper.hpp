/* File: pdf_obj_keeper.hpp
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

}  // namespace core

#endif  // PDF_OBJ_KEEPER_HPP

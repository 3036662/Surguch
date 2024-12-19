/* File: mu_page_render.hpp
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

#ifndef MU_PAGE_RENDER_HPP
#define MU_PAGE_RENDER_HPP

#include "mupdf/fitz.h"

namespace core {

/*!
 * \brief The RenderRes class
 */
struct RenderRes {
    unsigned char *buf = nullptr;
    float page_width = 0;
    float page_height = 0;
    float zoom_dpi = 0;  // zoom value,utilized to run_page
    float result_zoom = 0;
    ptrdiff_t pix_stride = 0;  // the number of pixels in one line
};

class MuPageRender {
   public:
    /*!
     * \brief MuPageRender::MuPageRender
     * \param fzctx mupdf context
     * \param fzdoc mupdf document
     * \throws runtime_error
     */
    MuPageRender(fz_context *fzctx, fz_document *fzdoc);

    /*!
     * \brief RenderPage Renders a page to raw memory
     * \param page_number A number of page to render
     * \param custom_rot_value rotation value (degrees)
     * \param width current element width
     * \param pix_ratio - see QWindow::devicePixelRatio()
     * \return RenderRes structure
     */
    [[nodiscard]] RenderRes RenderPage(int page_number, float custom_rot_value,
                                       // double width,
                                       float pix_ratio, float goal_width,
                                       float goal_zoom,
                                       float screen_dpi) const noexcept;

   private:
    fz_context *fzctx_ = nullptr;
    fz_document *fzdoc_ = nullptr;
};

}  // namespace core

#endif  // MU_PAGE_RENDER_HPP

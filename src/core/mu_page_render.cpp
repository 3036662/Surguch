/* File: mu_page_render.cpp
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

#include "mu_page_render.hpp"

#include <QDebug>
#include <QtMath>
#include <stdexcept>

namespace core {

// NOLINTBEGIN(bugprone-multi-level-implicit-pointer-conversion,bugprone-integer-division,
// cppcoreguidelines-avoid-do-while,cppcoreguidelines-pro-bounds-array-to-pointer-decay)

/*!
 * \brief MuPageRender::MuPageRender
 * \param fzctx mupdf context
 * \param fzdoc mupdf document
 * \throws runtime_error
 */
MuPageRender::MuPageRender(fz_context *fzctx, fz_document *fzdoc)
    : fzctx_(fzctx), fzdoc_(fzdoc) {
    if (fzctx_ == nullptr || fzdoc_ == nullptr) {
        throw std::runtime_error(
            "[MuPageRender] nullptr recieved on construct");
    }
}

/*!
 * \brief RenderPage Renders a page to raw memory
 * \param page_number A number of page to render
 * \param custom_rot_value rotation value (degrees)
 * \param width current element width
 * \param pix_ratio - see QWindow::devicePixelRatio()
 * \return RenderRes structure
 */
RenderRes MuPageRender::RenderPage(int page_number, float custom_rot_value,
                                   // double width,
                                   float pix_ratio, float goal_width,
                                   float goal_zoom,
                                   float screen_dpi) const noexcept {
    RenderRes res{};
    fz_device *draw_device = nullptr;
    fz_pixmap *pixmap = nullptr;
    fz_separations *separation = nullptr;
    fz_page *page = nullptr;
    // int page_width = 0;
    fz_var(draw_device);
    fz_var(pixmap);
    fz_var(separation);
    fz_var(page);
    fz_try(fzctx_) {
        page = fz_load_page(fzctx_, fzdoc_, page_number);
        separation = fz_page_separations(fzctx_, page);
        fz_rect page_rect = fz_bound_page(fzctx_, page);
        const fz_matrix custom_rotation_matrix = fz_rotate(custom_rot_value);
        page_rect = fz_transform_rect(
            page_rect, custom_rotation_matrix);  // page size 72dpi
        fz_colorspace *color_space = fz_device_rgb(fzctx_);
        // a dpi multiplier to render the pdf page with good quality
        float zoom_dpi = 1;  // 72 dpi x multiplier
        float pdf_page_width = page_rect.x1 - page_rect.x0;
        // if no wixed width - use the goal zoom value
        if (goal_width == 0) {
            if (goal_zoom <= 0) {
                goal_zoom = 1;
            }
            goal_width =
                (page_rect.x1 - page_rect.x0) * screen_dpi / 72 * goal_zoom;
            res.result_zoom = goal_zoom;
        } else {
            // qWarning() <<"goal_zoom ="<<goal_zoom;
            // qWarning() <<"goal_width ="<<goal_width;
            // qWarning() <<"page_width ="<<(page_rect.x1 - page_rect.x0);
            // qWarning() <<"screen_dpi ="<<screen_dpi;
            int p_width = page_rect.x1 - page_rect.x0;
            if (p_width > 0) {
                res.result_zoom = (goal_width / p_width) / (screen_dpi / 72);
            } else {
                res.result_zoom = 1;
            }
        }

        res.page_width = goal_width;
        // qWarning()<<"page_width "<< res.page_width;
        // qWarning()<<"render width  "<< width;
        if (res.page_width > 1) {
            zoom_dpi = goal_width / pdf_page_width;
        }
        res.page_height = (page_rect.y1 - page_rect.y0) * zoom_dpi;
        // create a new pixmap
        const int pixmap_width = qRound(goal_width * pix_ratio);
        const int pixmap_height = qRound(res.page_height * pix_ratio);
        pixmap = fz_new_pixmap(fzctx_, color_space, pixmap_width, pixmap_height,
                               separation, 0);
        fz_clear_pixmap(fzctx_, pixmap);
        // draw pdf content to pixmap
        // qWarning() <<"zoom dpi "<<zoom_dpi;
        const fz_matrix transform_run_page = fz_scale(zoom_dpi, zoom_dpi);
        fz_matrix transform_device = fz_scale(pix_ratio, pix_ratio);
        if (custom_rot_value != 0) {
            // rotate
            if (custom_rot_value == 90 || custom_rot_value == 270) {
                // move the center to the origin
                transform_device = fz_concat(
                    transform_device,
                    fz_translate(-pixmap_height / 2, -pixmap_width / 2));
                // rotate
                transform_device =
                    fz_concat(transform_device, custom_rotation_matrix);
                // move the center to the view center
                transform_device = fz_concat(
                    transform_device,
                    fz_translate(static_cast<float>(pixmap_width / 2),
                                 static_cast<float>(pixmap_height / 2)));
            } else {
                // move the center to the origin
                transform_device = fz_concat(
                    transform_device,
                    fz_translate(-pixmap_width / 2, -pixmap_height / 2));
                // rotate
                transform_device =
                    fz_concat(transform_device, custom_rotation_matrix);
                // move the center to the view center
                transform_device = fz_concat(
                    transform_device,
                    fz_translate(pixmap_width / 2, pixmap_height / 2));
            }
        }
        draw_device = fz_new_draw_device(fzctx_, transform_device,
                                         pixmap);  // fz_identity
        fz_run_page(fzctx_, page, draw_device, transform_run_page, nullptr);
        // copy pixmap data to buffer, connect this buffer with an QImage
        res.pix_stride = pixmap->stride;
        unsigned char *samples = pixmap->samples;
        auto buff_size = res.pix_stride * pixmap->h;
        res.buf = new unsigned char[buff_size + 1];
        std::copy(samples, samples + buff_size, res.buf);
        res.zoom_dpi = zoom_dpi;
    }
    fz_always(fzctx_) {
        // clean up
        fz_close_device(fzctx_, draw_device);
        fz_drop_device(fzctx_, draw_device);
        fz_drop_pixmap(fzctx_, pixmap);
        fz_drop_separations(fzctx_, separation);
        fz_drop_page(fzctx_, page);
    }
    fz_catch(fzctx_) { qWarning() << fz_caught_message(fzctx_); }
    return res;
}

// NOLINTEND(bugprone-multi-level-implicit-pointer-conversion,bugprone-integer-division,
// cppcoreguidelines-avoid-do-while,cppcoreguidelines-pro-bounds-array-to-pointer-decay)

}  // namespace core

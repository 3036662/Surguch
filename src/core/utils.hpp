/* File: utils.hpp
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

#ifndef UTILS_HPP
#define UTILS_HPP
#include <QString>
#include <cstddef>
#include <vector>

#include "mupdf/fitz.h"

namespace core::utils {

/**
 * @brief Read a PDF HEXstring to simple vector
 *
 * @param str pointer to data
 * @param size size of the data
 * @return std::vector<unsigned char>
 */
std::vector<unsigned char> hexStringToByteArray(const char* str,
                                                size_t size) noexcept;

/**
 * @brief Extract text from the given page.
 * @param fzctx the MuPDF context
 * @param fzdoc the MuPdf document context
 * @param page_index
 * @return QString text
 */
QString pageToQString(fz_context* fzctx, fz_document* fzdoc, int page_index);

using PagesTextCache = std::unique_ptr<std::vector<std::pair<size_t, QString>>>;

/**
 * @brief Extract all text from all pages in the document.
 * @param fzctx the MuPDF context
 * @param fzdoc the MuPdf document context
 * @return @see PagesTextCache, null on error
 * @throws does not throw
 * @details This function is supposed to be run as an async function.
 */
PagesTextCache extractTextAllPages(fz_context* fzctx,
                                   fz_document* fzdoc) noexcept;

}  // namespace core::utils

#endif  // UTILS_HPP

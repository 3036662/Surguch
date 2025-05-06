/* File: utils.cpp
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

#include "utils.hpp"

#include <QDebug>
#include <iostream>

namespace core::utils {

/**
 * @brief Read a PDF HEX string to simple vector
 * @param str pointer to data
 * @param size size of the data
 * @return std::vector<unsigned char>
 */
std::vector<unsigned char> hexStringToByteArray(const char *str,
                                                size_t size) noexcept {
  std::vector<unsigned char> res;
  if (size == 0 || str == nullptr) {
    return res;
  }
  std::string copy(str, str + size);
  if (copy.empty()) {
    return res;
  }
  if (copy.front() == '<') {
    copy.erase(copy.begin());
  }
  if (copy.back() == '>') {
    copy.pop_back();
  }
  if (copy.size() % 2 != 0) {
    copy.push_back('0');
  }
  for (size_t i = 0; i < copy.size(); i += 2) {
    const std::string tmp = copy.substr(i, 2);
    size_t pos = 0;
    try {
      int val = std::stoi(tmp, &pos, 16);
      if (pos != tmp.size()) {
        throw std::runtime_error("parse error");
      }
      res.push_back(static_cast<uint8_t>(val));
    } catch (const std::exception &ex) {
      qWarning() << "[hexStringToByteArray] error parsing hexstring "
                 << ex.what();
      return {};
    }
  }
  return res;
}

/**
 * @brief Extract text from the given page.
 * @param fzctx the MuPDF context
 * @param fzdoc the MuPdf document context
 * @param page_index
 * @return QString text
 */
QString pageToQString(fz_context *fzctx, fz_document *fzdoc, int page_index) {
  if (fzctx == nullptr || fzdoc == nullptr) {
    throw std::invalid_argument(
        "[core::utils::pageToQString] nullptr recieved");
  }
  QString extracted_string;
  extracted_string.reserve(256);
  bool mu_exception_catched = false;

  fz_stext_page *stpage = nullptr;
  fz_device *stext_dev = nullptr;
  fz_page *page = nullptr;

  fz_var(stpage);
  fz_var(stext_dev);
  fz_var(page);
  fz_try(fzctx) {
    page = fz_load_page(fzctx, fzdoc, page_index);
    stpage = fz_new_stext_page(fzctx, fz_bound_page(fzctx, page));
    const fz_stext_options opts = {FZ_STEXT_DEHYPHENATE, 1.0f};
    stext_dev = fz_new_stext_device(fzctx, stpage, &opts);
    fz_run_page_contents(fzctx, page, stext_dev, fz_identity, nullptr);
    fz_close_device(fzctx, stext_dev);
    // for each block
    for (fz_stext_block *block = stpage->first_block; block != nullptr;
         block = block->next) {
      if (block->type != FZ_STEXT_BLOCK_TEXT) {
        continue;
      }
      // for each line
      for (fz_stext_line *line = block->u.t.first_line; line != nullptr;
           line = line->next) {
        for (fz_stext_char *symbol = line->first_char; symbol != nullptr;
             symbol = symbol->next) {
          if (symbol->c <= 0xFFFF) {
            extracted_string.append(QChar(symbol->c));
          } else {
            auto arr = QChar::fromUcs4(symbol->c);
            std::for_each(arr.begin(), arr.end(),
                          [&extracted_string](char16_t ch) {
                            extracted_string.append(QChar(ch));
                          });
          }
        }
        extracted_string.append(QChar('\n'));
      }
      extracted_string.append(QChar('\n'));
    }
  }
  fz_always(fzctx) {
    fz_drop_page(fzctx, page);
    fz_drop_stext_page(fzctx, stpage);
    fz_drop_device(fzctx, stext_dev);
  }
  fz_catch(fzctx) {
    mu_exception_catched = true;
    qWarning() << fz_caught_message(fzctx);
  }

  if (mu_exception_catched) {
    throw std::runtime_error("[core::utils::pageToQString] MuPdf error");
  }

  return extracted_string;
}

/**
 * @brief Extract all text from all pages in the document.
 * @param fzctx the MuPDF context
 * @param fzdoc the MuPdf document context
 * @return @see PagesTextCache, null on error
 * @throws does not throw
 * @details This function is supposed to be run as an async function.
 */
PagesTextCache extractTextAllPages(fz_context *fzctx,
                                   fz_document *fzdoc) noexcept {
  if (fzctx == nullptr || fzdoc == nullptr) {
    qWarning() << "[extractTextAllPages] nullptr recieved\n";
    return nullptr;
  }
  bool exception_catched = false;
  PagesTextCache result =
      std::make_unique<std::vector<PagesTextCacheSinglePage>>();
  int page_count = 0;
  fz_var(page_count);
  fz_try(fzctx) { page_count = fz_count_pages(fzctx, fzdoc); }
  fz_catch(fzctx) {
    exception_catched = true;
    fz_report_error(fzctx);
  }
  for (int i = 0; i < page_count; ++i) {
    try {
      result->emplace_back(i, pageToQString(fzctx, fzdoc, i));
    } catch (const std::exception &ex) {
      qWarning() << ex.what();
      exception_catched = true;
    }
  }
  if (exception_catched) {
    qWarning() << "[extractTextAllPages] error occured";
    return nullptr;
  }
  return result;
};

/**
 * @brief findPageWithText
 * @param needle
 * @param haystack
 * @return vector of page indexes
 */
std::vector<size_t> findPagesWithText(const QString &needle,
                                      const PagesTextCache &haystack,
                                      bool case_sensitive) {
  std::vector<size_t> res;
  if (!haystack || haystack->empty() || needle.isEmpty()) {
    return res;
  }
  const Qt::CaseSensitivity case_sens =
      case_sensitive ? Qt::CaseSensitive : Qt::CaseInsensitive;
  std::for_each(
      haystack->cbegin(), haystack->cend(),
      [&res, &needle, &case_sens](const PagesTextCacheSinglePage &page_cached) {
        if (page_cached.second.contains(needle, case_sens)) {
          res.push_back(page_cached.first);
        }
      });
  return res;
}

/**
 * @brief Find a rectangle for each needle on the given page.
 * @param needle
 * @param page_index
 * @param case_sensitive
 * @return an array of rects
 * @throws does not throw
 * @details This function is supposed to be run as an async function.
 */
NeedleRectsOnPage findNeedleRectsOnPage(const QString &needle,
                                        size_t page_index, bool case_sensitive,
                                        fz_context *fzctx,
                                        fz_document *fzdoc) noexcept {
  bool mu_exception_catched = false;
  if (needle.isEmpty() || fzctx == nullptr || fzdoc == nullptr) {
    qWarning() << "[findNeedleRectsOnPage] invalid args";
    return nullptr;
  }
  int page_count = 0;
  fz_var(page_count);
  fz_try(fzctx) { page_count = fz_count_pages(fzctx, fzdoc); }
  fz_catch(fzctx) {
    mu_exception_catched = true;
    fz_report_error(fzctx);
  }
  if (page_index >= page_count ||
      page_index > std::numeric_limits<int>::max()) {
    qWarning() << "[findNeedleRectsOnPage] invalid page_index";
    return nullptr;
  }
  const Qt::CaseSensitivity case_sens =
      case_sensitive ? Qt::CaseSensitive : Qt::CaseInsensitive;
  NeedleRectsOnPage res = std::make_shared<std::vector<fz_rect>>();
  fz_stext_page *stpage = nullptr;
  fz_device *stext_dev = nullptr;
  fz_page *page = nullptr;
  fz_var(stpage);
  fz_var(stext_dev);
  fz_var(page);

  // extract text from page
  fz_try(fzctx) {
    page = fz_load_page(fzctx, fzdoc, static_cast<int>(page_index));
    stpage = fz_new_stext_page(fzctx, fz_bound_page(fzctx, page));
    const fz_stext_options opts = {FZ_STEXT_DEHYPHENATE, 1.0F};
    stext_dev = fz_new_stext_device(fzctx, stpage, &opts);
    fz_run_page_contents(fzctx, page, stext_dev, fz_identity, nullptr);
    fz_close_device(fzctx, stext_dev);

    // for each block of text
    for (fz_stext_block *block = stpage->first_block; block != nullptr;
         block = block->next) {
      if (block->type != FZ_STEXT_BLOCK_TEXT) {
        continue;
      }
      // for each line, extract the line end the rune rects
      for (fz_stext_line *line = block->u.t.first_line; line != nullptr;
           line = line->next) {
        QString extracted_string;
        std::vector<fz_quad> extracted_quads;
        for (fz_stext_char *symbol = line->first_char; symbol != nullptr;
             symbol = symbol->next) {
          if (symbol->c <= 0xFFFF) {
            extracted_string.append(QChar(symbol->c));
            extracted_quads.push_back(symbol->quad);
          } else {
            auto arr = QChar::fromUcs4(symbol->c);
            std::for_each(
                arr.begin(), arr.end(),
                [&extracted_string, &extracted_quads, &symbol](char16_t ch16) {
                  extracted_string.append(QChar(ch16));
                  extracted_quads.push_back(symbol->quad);
                });
            //
          }
        }
        // After the line was extracted, search for the needle.
        qsizetype pos = 0;
        while ((pos = extracted_string.indexOf(needle, pos, case_sens)) != -1) {
          // rects for the signle needle
          std::vector<fz_rect> tmp_res;
          std::transform(extracted_quads.cbegin() + pos,
                         extracted_quads.cbegin() + pos + needle.length(),
                         std::back_inserter(tmp_res),
                         [&tmp_res](const fz_quad &quad) {
                           return fz_rect_from_quad(quad);
                         });
          pos += needle.length();
          // merge tmp_res rect to one rect
          fz_rect single_needle_rect = fz_empty_rect;
          std::for_each(tmp_res.cbegin(), tmp_res.cend(),
                        [&single_needle_rect](fz_rect val) {
                          single_needle_rect =
                              fz_union_rect(single_needle_rect, val);
                        });
          // push the resulting rect to the res
          if (fz_is_empty_rect(single_needle_rect) == 0 &&
              fz_is_infinite_rect(single_needle_rect) == 0 &&
              fz_is_valid_rect(single_needle_rect)) {
            res->push_back(single_needle_rect);
          }
        }
      }
    }
  }
  fz_always(fzctx) {
    fz_drop_page(fzctx, page);
    fz_drop_stext_page(fzctx, stpage);
    fz_drop_device(fzctx, stext_dev);
  }
  fz_catch(fzctx) {
    mu_exception_catched = true;
    fz_report_error(fzctx);
  }
  if (mu_exception_catched) {
    qWarning() << "[findNeedleRectsOnPage] error occured";
    return nullptr;
  }
  return res;
}

}  // namespace core::utils

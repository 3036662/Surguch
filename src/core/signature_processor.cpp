/* File: signature_processor.cpp
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

#include "signature_processor.hpp"
#include "raw_signature.hpp"
#include <QDebug>
#include <stdexcept>

namespace core {

// NOLINTBEGIN(bugprone-multi-level-implicit-pointer-conversion,
// cppcoreguidelines-avoid-do-while,cppcoreguidelines-pro-bounds-array-to-pointer-decay)

SignatureProcessor::SignatureProcessor(fz_context *fzctx, pdf_document *pdfdoc)
    : fzctx_(fzctx), pdfdoc_(pdfdoc) {
  if (fzctx_ == nullptr || pdfdoc_ == nullptr) {
    throw std::runtime_error(
        "[SignatureProcessor] nullptr context on construction");
  }
  fz_try(fzctx_) {
    trailer_ = pdf_trailer(fzctx_, pdfdoc_);
    if (pdf_is_dict(fzctx_, trailer_) == 0) {
      trailer_ = nullptr;
    }
    pdf_root_ = pdf_dict_getp(fzctx_, trailer_, "Root");
  }
  fz_catch(fzctx_) {
    qWarning() << fz_caught_message(fzctx_);
    fz_caught(fzctx_);
  }
  if (trailer_ == nullptr || pdf_root_ == nullptr) {
    throw std::runtime_error(
        "[SignatureProcessor] document root was not found");
  }
}

///@brief find signature objects, place them in signarures_ptrs_
bool SignatureProcessor::findSignatures() noexcept {
  signarures_ptrs_.clear();
  // pdf_debug_obj(fzctx_, trailer_);
  // pdf_debug_obj(fzctx_, pdf_root_);
  pdf_acro_ = findAcroForm();
  // no acroform in the document
  if (pdf_acro_ == nullptr) {
    return true;
  }
  // pdf_debug_obj(fzctx_, pdf_acro_);
  //  read SigFlags
  sig_flags_ = getFormSigFlags();
  // find signatures
  fz_try(fzctx_) {
    // get fields array
    pdf_obj *fields_array = pdf_dict_getp(fzctx_, pdf_acro_, "Fields");
    // iterate fields
    if (fields_array != nullptr && pdf_is_array(fzctx_, fields_array) != 0) {
      const int fields_len = pdf_array_len(fzctx_, fields_array);
      for (int i = 0; i < fields_len; ++i) {
        pdf_obj *curr = pdf_array_get(fzctx_, fields_array, i);
        if (curr != nullptr && pdf_is_dict(fzctx_, curr) != 0) {
          pdf_obj *ft_val = pdf_dict_getp(fzctx_, curr, "FT");
          if (ft_val != nullptr && pdf_is_name(fzctx_, ft_val) &&
              QString(pdf_to_name(fzctx_, ft_val)) == "Sig") {
            signarures_ptrs_.emplace_back(fzctx_, curr);
            // pdf_debug_obj(fzctx_, curr);
          }
        }
      }
    }
  }
  fz_catch(fzctx_) {
    qWarning() << fz_caught_message(fzctx_);
    fz_caught(fzctx_);
    return false;
  }
  return true;
}

/// @brief ind AcroForm object
/// @details ISO 32000 [12.7.2]
pdf_obj *SignatureProcessor::findAcroForm() const noexcept {
  if (pdf_root_ == nullptr) {
    return nullptr;
  }
  pdf_obj *result = nullptr;
  int found_counter = 0;
  fz_try(fzctx_) {
    // count Acroforms
    const int dict_len = pdf_dict_len(fzctx_, pdf_root_);
    for (int i = 0; i < dict_len; ++i) {
      pdf_obj *curr = pdf_dict_get_key(fzctx_, pdf_root_, i);
      if (pdf_is_name(fzctx_, curr) != 0) {
        if (QString(pdf_to_name(fzctx_, curr)) == "AcroForm") {
          ++found_counter;
          pdf_obj *form_value = pdf_dict_get_val(fzctx_, pdf_root_, i);
          if (pdf_is_dict(fzctx_, form_value) != 0) {
            result = form_value;
          }
        }
      }
    }
  }
  fz_catch(fzctx_) {
    qWarning() << fz_caught_message(fzctx_);
    fz_caught(fzctx_);
    return nullptr;
  }
  if (found_counter > 1) {
    qWarning() << "Only one acroform is expected in root, " << found_counter
               << " was found";
    return nullptr;
  }
  return result;
}

/// @brief Get the SigFlags bitset
/// @details ISO32000 [12.7.2]
std::bitset<32> SignatureProcessor::getFormSigFlags() const noexcept {
  std::bitset<32> res;
  if (pdf_acro_ == nullptr) {
    return res;
  }
  fz_try(fzctx_) {
    pdf_obj *sig_flags_obj = pdf_dict_getp(fzctx_, pdf_acro_, "SigFlags");
    if (sig_flags_obj != nullptr && pdf_is_int(fzctx_, sig_flags_obj) != 0) {
      res = pdf_to_int(fzctx_, sig_flags_obj);
    }
  }
  fz_catch(fzctx_) {
    qWarning() << fz_caught_message(fzctx_);
    fz_caught(fzctx_);
  }
  return res;
}

///@brief parses signature object
///@return returns array of RawSignature objects
std::vector<RawSignature> SignatureProcessor::ParseSignatures() noexcept {
  if (signarures_ptrs_.empty()) {
    return {};
  }
  if (fzctx_ == nullptr) {
    qWarning() << "[ParseSignatures] nullptr context, can not proceed";
    return {};
  }
  std::vector<RawSignature> res;
  for (const auto &sig : signarures_ptrs_) {
    try {
      res.emplace_back(fzctx_, sig);
    } catch (const std::exception &ex) {
      qWarning() << "Error parsing the signature " << ex.what();
    }
  }
  return res;
}

// NOLINTEND(bugprone-multi-level-implicit-pointer-conversion,
// cppcoreguidelines-avoid-do-while,cppcoreguidelines-pro-bounds-array-to-pointer-decay)

} // namespace core

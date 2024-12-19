/* File: signature_processor.hpp
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

#ifndef SIGNATURE_PROCESSOR_HPP
#define SIGNATURE_PROCESSOR_HPP

#include "mupdf/pdf.h"
#include "pdf_obj_keeper.hpp"
#include "raw_signature.hpp"
#include <bitset>
#include <vector>

namespace core {

/**
 * @brief Find signatures in the given document
 * @throws runtime_error on construct
 */
class SignatureProcessor {
public:
  SignatureProcessor(fz_context *fzctx, pdf_document *pdfdoc);
  SignatureProcessor(const SignatureProcessor &) = delete;
  SignatureProcessor(SignatureProcessor &&) = delete;
  SignatureProcessor &operator=(const SignatureProcessor &) = delete;
  SignatureProcessor &operator=(SignatureProcessor &&) = delete;
  ~SignatureProcessor() = default;

  ///@brief find signature objects, place them in signarures_ptrs_
  [[nodiscard]] bool findSignatures() noexcept;

  ///@brief parses signature object
  ///@return returns array of RawSignature objects
  std::vector<RawSignature> ParseSignatures() noexcept;

  /// @brief Get the SigFlags bitset
  /// @details ISO32000 [12.7.2]
  [[nodiscard]] std::bitset<32> getFormSigFlags() const noexcept;

  /// @brief The number of signatures in the file
  [[nodiscard]] size_t getSignaturesCount() const noexcept {
    return signarures_ptrs_.size();
  }

  /// @brief ind AcroForm object
  /// @details ISO 32000 [12.7.2]
  [[nodiscard]] pdf_obj *findAcroForm() const noexcept;

private:
  fz_context *fzctx_ = nullptr;
  pdf_document *pdfdoc_ = nullptr;
  pdf_obj *trailer_ = nullptr;
  pdf_obj *pdf_root_ = nullptr;
  pdf_obj *pdf_acro_ = nullptr;
  std::bitset<32> sig_flags_;
  std::vector<PdfObjKeeper> signarures_ptrs_;
};

} // namespace core

#endif // SIGNATURE_PROCESSOR_HPP

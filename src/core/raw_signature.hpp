/* File: raw_signature.hpp
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

#ifndef RAW_SIGNATURE_HPP
#define RAW_SIGNATURE_HPP
#include "mupdf/pdf.h"
#include "pdf_obj_keeper.hpp"
#include <cstdint>
#include <vector>

namespace core {

using BytesVector = std::vector<unsigned char>;
using RangesVector = std::vector<std::pair<uint64_t, uint64_t>>;

/**
 * @brief Extracts row signature data from a pdf_obj
 * @throws  runtime_error if construction fails
 */
class RawSignature {
public:
  /// @brief create from PdfObjKeeper
  explicit RawSignature(fz_context *fzctx, const PdfObjKeeper &sig_obj);

  /// @brief create from a raw signature value and a byteranges
  explicit RawSignature(BytesVector sigdata, RangesVector byteranges)
      : sigdata_{std::move(sigdata)}, byteranges_{std::move(byteranges)} {}

  /// @brief get the raw signature data
  [[nodiscard]] const BytesVector &getSigData() const noexcept {
    return sigdata_;
  }

  [[nodiscard]] const RangesVector &getByteRanges() const noexcept {
    return byteranges_;
  }

private:
  /// @brief read the 'Contents'
  /// @details ISO 32000 [table 252]
  bool readContent(fz_context *fzctx, pdf_obj *sig_val) noexcept;

  /// @brief read the 'ByteRange'
  /// @details ISO 32000 [table 252]
  bool readByteRanges(fz_context *fzctx, pdf_obj *sig_val) noexcept;

  BytesVector sigdata_;
  RangesVector byteranges_;
};

}; // namespace core
#endif // RAW_SIGNATURE_HPP

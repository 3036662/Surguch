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

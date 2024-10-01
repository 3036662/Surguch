#include "raw_signature.hpp"
#include "utils.hpp"
#include <iostream>
#include <stdexcept>
#include <string>

namespace core {

// NOLINTBEGIN(bugprone-multi-level-implicit-pointer-conversion,
// cppcoreguidelines-avoid-do-while,cppcoreguidelines-pro-bounds-array-to-pointer-decay)

RawSignature::RawSignature(fz_context *fzctx, const PdfObjKeeper &sig_obj) {
  if (fzctx == nullptr) {
    throw std::runtime_error(
        "[RawSignature] nullptr recieved instead of context");
  }
  if (sig_obj.get() == nullptr) {
    throw std::runtime_error("[RawSignature] empty object recieved");
  }
  std::string exception_caught;
  fz_try(fzctx) {
    // pdf_debug_obj(fzctx,sig_obj.get());
    pdf_obj *sig_val = pdf_dict_getp(fzctx, sig_obj.get(), "V");
    if (sig_val != nullptr && pdf_is_dict(fzctx, sig_val) != 0) {
      // pdf_debug_obj(fzctx,sig_val);
      //  read hex string with contets
      if (!readContent(fzctx, sig_val)) {
        exception_caught = "error reading content";
      }
      // read byteranges
      if (!readByteRanges(fzctx, sig_val)) {
        exception_caught = "error reading byteranges";
      }
      // pdf_debug_obj(fzctx,digest_hex);
    }
  }
  fz_catch(fzctx) {
    exception_caught = fz_caught_message(fzctx);
    fz_caught(fzctx);
  }
  if (!exception_caught.empty()) {
    throw std::runtime_error(exception_caught.c_str());
  }
}

bool RawSignature::readContent(fz_context *fzctx, pdf_obj *sig_val) noexcept {
  if (fzctx == nullptr || sig_val == nullptr) {
    return false;
  }
  pdf_obj *digest_hex = pdf_dict_getp(fzctx, sig_val, "Contents");
  if (pdf_is_string(fzctx, digest_hex) != 0) {
    size_t str_size = 0;
    // pdf_debug_obj(fzctx,digest_hex);
    const char *hex_string = pdf_to_string(fzctx, digest_hex, &str_size);
    sigdata_.clear();
    std::copy(hex_string, hex_string + str_size, std::back_inserter(sigdata_));
  }
  return true;
}

bool RawSignature::readByteRanges(fz_context *fzctx,
                                  pdf_obj *sig_val) noexcept {
  if (fzctx == nullptr || sig_val == nullptr) {
    return false;
  }
  pdf_obj *byteranges_obj = pdf_dict_getp(fzctx, sig_val, "ByteRange");
  if (byteranges_obj != nullptr && pdf_is_array(fzctx, byteranges_obj) != 0) {
    const size_t arr_size = pdf_array_len(fzctx, byteranges_obj);
    if (arr_size % 2 != 0) {
      return false;
    }
    int64_t start = 0;
    int64_t end = 0;
    for (int i = 0; i < arr_size; ++i) {
      auto val = pdf_array_get_int(fzctx, byteranges_obj, i);
      if (i % 2 == 0) {
        start = val;
      } else {
        end = val;
        byteranges_.emplace_back(start, end);
      }
    }
  }
  return true;
}

// NOLINTEND(bugprone-multi-level-implicit-pointer-conversion,
// cppcoreguidelines-avoid-do-while,cppcoreguidelines-pro-bounds-array-to-pointer-decay)

} // namespace core

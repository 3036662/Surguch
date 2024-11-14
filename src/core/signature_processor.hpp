#ifndef SIGNATURE_PROCESSOR_HPP
#define SIGNATURE_PROCESSOR_HPP

#include "mupdf/pdf.h"
#include "pdf_obj_keeper.hpp"
#include "raw_signature.hpp"
#include <bitset>
#include <vector>

namespace core {

/**
 * @brief The SignatureProcessor class
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

  [[nodiscard]] pdf_obj *findAcroForm() const noexcept;

  [[nodiscard]] std::bitset<32> getFormSigFlags() const noexcept;

  [[nodiscard]] size_t getSignaturesCount() const noexcept {
    return signarures_ptrs_.size();
  }

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

#ifndef SIGNATUES_VALIDATOR_HPP
#define SIGNATUES_VALIDATOR_HPP

#include "csp_response.hpp"
#include "raw_signature.hpp"
#include <QObject>
#include <memory>

namespace core {

class SignaturesValidator : public QObject {
  Q_OBJECT

public:
  struct CoverageInfo {
    RangesVector gaps;
    RangesVector byteranges;
    bool gap_after_end = false;
    bool invalid_range = false;
    bool full_coverage = false;
    bool can_be_casted_to_full_coverage = false;
    uint64_t coverage = 0;
    size_t sig_data_size = 0;
    size_t gaps_size = 0;
    size_t file_size = 0;
  };

  // explicit SignaturesValidator(QObject *parent = nullptr);

  ~SignaturesValidator() override = default;

  void abort() { abort_recieved_ = true; };

public slots:

  /// @brief validate all non-empty signatures by creating CspResponse objects
  void validateSignatures(std::vector<core::RawSignature> raw_signatures,
                          QString file_source);

signals:
  void validationFinished();

  void validatationResult(std::shared_ptr<CSPResponse> validation_result,
                          size_t index);

private:
#ifdef WITH_QTEST
public:
#endif
  /** @brief Analyze byteranges
   *  @details In perfect case, there is at least one signature covering the
   * whole file content, if no,we need to emit a message
   */
  void
  analyzeSigCoverages(const std::vector<core::RawSignature> &raw_signatures_,
                      const QString &file_source) noexcept;

  CoverageInfo analyzeOneSigCoverage(const core::RawSignature &sig,
                                     size_t file_size) noexcept;

  bool abort_recieved_ = false;
};

} // namespace core

#endif // SIGNATUES_VALIDATOR_HPP

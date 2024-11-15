#ifndef SIGNATUES_VALIDATOR_HPP
#define SIGNATUES_VALIDATOR_HPP

#include "raw_signature.hpp"
#include "validation_result.hpp"
#include <QObject>
#include <memory>

namespace core {

class DocStatusEnum : public QObject {
  Q_OBJECT
public:
  enum class CommonDocCoverageStatus : uint8_t {
    kEverythingIsFine = 0,
    kDocCanBeRecovered = 1,
    kDocCantBeTrusted = 2,
    kDocSuspiciousPrevious = 3,
    kDocCanBeRecoveredButSuspicious = 4
  };
  Q_ENUM(CommonDocCoverageStatus);

  explicit DocStatusEnum(QObject *parent = nullptr) : QObject(parent) {}
};

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

  explicit SignaturesValidator(QObject *parent = nullptr) : QObject{parent} {};

  ~SignaturesValidator() override = default;

  void abort() { abort_recieved_ = true; };

public slots:

  /// @brief validate all non-empty signatures by creating CspResponse objects
  void validateSignatures(std::vector<core::RawSignature> raw_signatures,
                          QString file_source);

signals:
  void validationFinished(DocStatusEnum::CommonDocCoverageStatus);

  void validatationResult(std::shared_ptr<ValidationResult> validation_result,
                          size_t index);

private:
#ifdef WITH_QTEST
public:
#endif

  CoverageInfo analyzeOneSigCoverage(const core::RawSignature &sig,
                                     size_t file_size) noexcept;

  DocStatusEnum::CommonDocCoverageStatus
  coverageStatus(const std::map<size_t, CoverageInfo> &coverage_infos,
                 bool raw_signatures_empty);

  bool abort_recieved_ = false;
};

} // namespace core

#endif // SIGNATUES_VALIDATOR_HPP

#include "signatures_validator.hpp"
#include <QDebug>
#include <QFileInfo>
#include <QThread>

namespace core {

// SignaturesValidator::SignaturesValidator(QObject *parent) : QObject{parent} {
//   qWarning() << "Validator constructed";
// }

void SignaturesValidator::validateSignatures(
    std::vector<core::RawSignature> raw_signatures, QString file_source) {
  bool aborted = false;
  for (size_t i = 0; i < raw_signatures.size(); ++i) {
    const auto &sig = raw_signatures[i];
    if (abort_recieved_ ||
        QThread::currentThread()->isInterruptionRequested()) {
      qWarning() << "Validation abort";
      aborted = true;
      break;
    }
    qWarning() << "validating signatures " << raw_signatures.size();
    std::shared_ptr<CSPResponse> result;
    // do not parse empty sigbatures
    if (sig.getSigData().empty() || sig.getByteRanges().empty()) {
      continue;
    }
    try {
      result = std::make_shared<CSPResponse>(sig, file_source.toStdString());
    } catch (const std::exception &ex) {
      qWarning() << ex.what();
    }
    if (result) {
      emit validatationResult(result, i);
    }
  }
  if (aborted) {
    qWarning() << "quit without sending results";
    QThread::currentThread()->quit();
  } else {
    emit validationFinished();
  }
}

void SignaturesValidator::analyzeSigCoverages(
    const std::vector<core::RawSignature> &raw_signatures,
    const QString &file_source) noexcept {
  const QFileInfo src_file(file_source);
  const qint64 file_size = src_file.size();
  if (!src_file.exists() || file_size == 0) {
    qWarning() << "[AnalyzeByteRanges] file not found " << src_file;
  }
  std::map<size_t, size_t> coverage_map; // coverage -> index
  bool full_coverage_found = false;
  size_t covering_sig_index = 0;

  for (const auto &sig : raw_signatures) {
    analyzeOneSigCoverage(sig, file_size);
  }
}

SignaturesValidator::CoverageInfo
SignaturesValidator::analyzeOneSigCoverage(const core::RawSignature &sig,
                                           size_t file_size) noexcept {
  CoverageInfo res{};
  res.file_size = file_size;
  res.sig_data_size = sig.getSigData().size();
  res.byteranges = sig.getByteRanges();
  std::sort(res.byteranges.begin(), res.byteranges.end(),
            [](const std::pair<uint64_t, uint64_t> &lhs,
               const std::pair<uint64_t, uint64_t> &rhs) {
              return lhs.first < rhs.first;
            });
  size_t current_pos = 0;
  for (const auto &range : res.byteranges) {
    if (range.first > current_pos) {
      res.gaps.emplace_back(current_pos, range.first);
      current_pos = range.first;
    }
    const uint64_t advance_diff = current_pos - range.first;
    const uint64_t advance_val = range.second - advance_diff;
    current_pos += advance_val;
  }
  // if there is a gap after last range
  if (current_pos < file_size) {
    res.gaps.emplace_back(current_pos, file_size);
    current_pos = file_size;
    res.gap_after_end = true;
  }
  if (current_pos != file_size) {
    res.invalid_range = true;
  }
  // total gap size
  res.gaps_size = std::accumulate(
      res.gaps.cbegin(), res.gaps.cend(), 0,
      [](uint64_t val, const std::pair<uint64_t, uint64_t> &gap) {
        return val + (gap.second - gap.first);
      });
  // total coverage
  res.coverage = file_size - res.gaps_size;

  // clang-format off
  res.full_coverage =
      res.byteranges.size() > 1 &&
      res.gaps.size() == 1 &&
      !res.gap_after_end &&
      res.coverage == file_size - res.sig_data_size &&
      res.sig_data_size > 0 &&
      res.gaps_size == res.sig_data_size &&
      !res.invalid_range;
  res.can_be_casted_to_full_coverage =
      res.byteranges.size() > 1 &&
      res.gaps.size() == 2 &&
      res.gaps[0].second - res.gaps[0].first == res.sig_data_size &&
      res.gap_after_end && res.coverage < file_size - res.sig_data_size &&
      res.sig_data_size > 0 && res.gaps_size > res.sig_data_size &&
      !res.invalid_range;
  // clang-format on
  return res;
}

} // namespace core

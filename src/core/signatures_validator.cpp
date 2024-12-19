/* File: signatures_validator.cpp
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

#include "signatures_validator.hpp"

#include <QDebug>
#include <QFileInfo>
#include <QThread>
#include <cstdint>

namespace core {

/// @brief validate all non-empty signatures by creating CspResponse objects
void SignaturesValidator::validateSignatures(
    std::vector<core::RawSignature> raw_signatures,
    const QString &file_source) {
    bool aborted = false;
    std::map<size_t, CoverageInfo> coverage_infos;
    size_t index_curr_sig = 0;
    for (index_curr_sig = 0; index_curr_sig < raw_signatures.size();
         ++index_curr_sig) {
        const auto &sig = raw_signatures[index_curr_sig];
        if (abort_recieved_ ||
            QThread::currentThread()->isInterruptionRequested()) {
            qWarning() << "Validation abort";
            aborted = true;
            break;
        }
        qWarning() << "validating signatures " << raw_signatures.size();
        std::shared_ptr<ValidationResult> result;
        // do not parse empty signatures
        if (sig.getSigData().empty() || sig.getByteRanges().empty()) {
            continue;
        }
        try {
            result = std::make_shared<ValidationResult>(
                sig, file_source.toStdString());
            // analyse byteranges
            const QFileInfo f_info(file_source);
            CoverageInfo cov_info = analyzeOneSigCoverage(sig, f_info.size());
            result->full_coverage = cov_info.full_coverage;
            result->can_be_casted_to_full_coverage =
                cov_info.can_be_casted_to_full_coverage;
            result->byteranges = sig.getByteRanges();
            result->sig_curr_index = index_curr_sig;
            result->file_path = file_source;
            coverage_infos.insert_or_assign(index_curr_sig, cov_info);
        } catch (const std::exception &ex) {
            qWarning() << ex.what();
            emit validationFailedForSignature(index_curr_sig);
        }
        if (result) {
            emit validatationResult(result, index_curr_sig);
        }
    }
    if (aborted) {
        qWarning() << "quit without sending results";
        QThread::currentThread()->quit();
    } else {
        // emit message about signatures coverages
        const DocStatusEnum::CommonDocCoverageStatus status =
            coverageStatus(coverage_infos, raw_signatures.empty());
        emit validationFinished(status);
    }
}

/**
 * @brief Analyze how the signatures cover the document.
 * @details In a perfect world, we expect at least one signature to protect the
 * whole document content.
 * @param sig a RawSignature
 * @param file_size
 * @return CoverageInfo
 */
SignaturesValidator::CoverageInfo SignaturesValidator::analyzeOneSigCoverage(
    const core::RawSignature &sig, size_t file_size) noexcept {
    CoverageInfo res{};
    res.file_size = file_size;
    // convert to HEX string x2 + 2 bytes for brackets
    res.sig_data_size = sig.getSigData().size() * 2 + 2;
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
        res.gaps.cbegin(), res.gaps.cend(), static_cast<uint64_t>(0),
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

/**
 * @brief Decide what level of trust can we suggest for the document
 * @param coverage_infos
 * @param raw_signatures_empty
 * @return DocStatusEnum::CommonDocCoverageStatus
 */
DocStatusEnum::CommonDocCoverageStatus SignaturesValidator::coverageStatus(
    const std::map<size_t, CoverageInfo> &coverage_infos,
    bool raw_signatures_empty) {
    const bool at_least_one_full_coverage =
        std::any_of(coverage_infos.cbegin(), coverage_infos.cend(),
                    [](const std::pair<size_t, CoverageInfo> &val) {
                        return val.second.full_coverage;
                    });
    const bool at_least_one_recoverable =
        std::any_of(coverage_infos.cbegin(), coverage_infos.cend(),
                    [](const std::pair<size_t, CoverageInfo> &val) {
                        return val.second.can_be_casted_to_full_coverage;
                    });
    const bool at_lest_one_suspicious =
        std::any_of(coverage_infos.cbegin(), coverage_infos.cend(),
                    [](const std::pair<size_t, CoverageInfo> &val) {
                        return !val.second.can_be_casted_to_full_coverage &&
                               !val.second.full_coverage;
                    });
    const bool all_full_or_recoverable =
        std::all_of(coverage_infos.cbegin(), coverage_infos.cend(),
                    [](const std::pair<size_t, CoverageInfo> &val) {
                        return val.second.full_coverage ||
                               val.second.can_be_casted_to_full_coverage;
                    });
    // at least one is good, the rest are recoverable
    const bool everything_is_fine =
        at_least_one_full_coverage && all_full_or_recoverable;
    // the document can be recovered
    const bool doc_can_be_recovered = all_full_or_recoverable &&
                                      !at_least_one_full_coverage &&
                                      !raw_signatures_empty;
    // doc can't be trusted
    // const bool doc_cant_be_trusted =
    //     !at_least_one_full_coverage && !at_least_one_recoverable;
    // some signatures are suspicious
    const bool doc_suspicious_previous =
        at_least_one_full_coverage && at_lest_one_suspicious;
    const bool doc_can_be_recoverd_but_suspicious =
        at_least_one_recoverable && at_lest_one_suspicious;
    // cann't be trusted by default
    DocStatusEnum::CommonDocCoverageStatus status =
        DocStatusEnum::CommonDocCoverageStatus::kDocCantBeTrusted;
    if (everything_is_fine || raw_signatures_empty) {
        status = DocStatusEnum::CommonDocCoverageStatus::kEverythingIsFine;
    }
    if (doc_can_be_recovered) {
        status = DocStatusEnum::CommonDocCoverageStatus::kDocCanBeRecovered;
    }
    if (doc_suspicious_previous) {
        status = DocStatusEnum::CommonDocCoverageStatus::kDocSuspiciousPrevious;
    }
    if (doc_can_be_recoverd_but_suspicious) {
        status = DocStatusEnum::CommonDocCoverageStatus::
            kDocCanBeRecoveredButSuspicious;
    }
    return status;
}

}  // namespace core

/* File: signatures_validator.hpp
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

#ifndef SIGNATUES_VALIDATOR_HPP
#define SIGNATUES_VALIDATOR_HPP

#include <QObject>
#include <memory>

#include "raw_signature.hpp"
#include "validation_result.hpp"

namespace core {

/*
/**
 * @brief Describes suggested trust level for the document
 * @details wrapped into QObject to be seen from QML
 */
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

    static QString toString(CommonDocCoverageStatus stat){
        switch (stat) {
        case CommonDocCoverageStatus::kEverythingIsFine:
            return "kEverythingIsFine";
            break;
        case CommonDocCoverageStatus::kDocCanBeRecovered:
            return "kDocCanBeRecovered";
            break;
        case CommonDocCoverageStatus::kDocSuspiciousPrevious:
            return "kDocSuspiciousPrevious";
            break;
        case CommonDocCoverageStatus::kDocCanBeRecoveredButSuspicious:
            return "kDocCanBeRecoveredButSuspicious";
            break;
        default:
            return "kDocCantBeTrusted";
            break;
        }
    }
    explicit DocStatusEnum(QObject *parent = nullptr) : QObject(parent) {}
};


/**
 * @brief Class for signatures validation
 */
class SignaturesValidator : public QObject {
    Q_OBJECT

   public:
    /// @brief Describes how the document is covered by signature
    struct CoverageInfo {
        RangesVector gaps;  // one or two gaps expected
        RangesVector byteranges;
        bool gap_after_end =
            false;  // second gap can be found after the byterange
        bool invalid_range = false;
        bool full_coverage = false;
        bool can_be_casted_to_full_coverage = false;
        uint64_t coverage = 0;     // how many bytes are covered
        size_t sig_data_size = 0;  // supposed to be equal to first gap size
        size_t gaps_size = 0;      // how many bytes are uncovered
        size_t file_size = 0;
    };

    explicit SignaturesValidator(QObject *parent = nullptr)
        : QObject{parent} {};

    void abort() { abort_recieved_ = true; };

   public slots:

    /// @brief validate all non-empty signatures by creating CspResponse objects
    void validateSignatures(std::vector<core::RawSignature> raw_signatures,
                            const QString &file_source);

   signals:

    //void validationFinished(QString coverage_status);

    /// @brief Validation was finished for all signatures.
    void validationFinished(DocStatusEnum::CommonDocCoverageStatus);

    /// @brief Validation is finished for one of the signatures.
    void validatationResult(std::shared_ptr<ValidationResult> validation_result,
                            size_t index);

    /// @brief validation failed
    void validationFailedForSignature(size_t index);

   private:
#ifdef WITH_QTEST
   public:
#endif

    /**
     * @brief Analyze how the signatures cover the document.
     * @details In a perfect world, we expect at least one signature to protect
     * the whole document content.
     * @param sig a RawSignature
     * @param file_size
     * @return CoverageInfo
     */
    static CoverageInfo analyzeOneSigCoverage(const core::RawSignature &sig,
                                              size_t file_size) noexcept;

    /**
     * @brief Decide what level of trust can we suggest for the document
     * @param coverage_infos
     * @param raw_signatures_empty
     * @return DocStatusEnum::CommonDocCoverageStatus
     */
    DocStatusEnum::CommonDocCoverageStatus static coverageStatus(
        const std::map<size_t, CoverageInfo> &coverage_infos,
        bool raw_signatures_empty);

    bool abort_recieved_ = false;
};

}  // namespace core

#endif  // SIGNATUES_VALIDATOR_HPP

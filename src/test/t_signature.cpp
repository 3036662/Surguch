#include "t_signature.hpp"
#include "core/csp_c_bridge/bridge_utils.hpp"
#include "core/signature_processor.hpp"
#include "core/signatures_validator.hpp"
#include "core/utils.hpp"
#include "models/pdf_doc_model.hpp"
#include <QTest>
#include <fstream>
#include <iostream>

TSignature::TSignature(QObject *parent) : QObject{parent} {}

void TSignature::testTrue() { QVERIFY(true); }

void TSignature::testModelConstruct() {
  PdfDocModel model;
  model.setSource(file1_);
  QVERIFY(model.rowCount({}) == 14);
  QVERIFY(model.getCtx() != nullptr);
  QVERIFY(model.getDoc() != nullptr);
  QVERIFY(model.getPdfDoc() != nullptr);
}

void TSignature::createProcessorFail() {
  QVERIFY_THROWS_EXCEPTION(std::runtime_error,
                           core::SignatureProcessor(nullptr, nullptr));
}

void TSignature::createProcessor() {
  PdfDocModel model;
  model.setSource(file1_);
  std::unique_ptr<core::SignatureProcessor> processor = nullptr;
  QVERIFY_THROWS_NO_EXCEPTION(processor =
                                  std::make_unique<core::SignatureProcessor>(
                                      model.getCtx(), model.getPdfDoc()));
  QVERIFY(processor);
}

void TSignature::findSignatures() {
  PdfDocModel model;
  model.setSource(file1_);
  std::unique_ptr<core::SignatureProcessor> processor = nullptr;
  QVERIFY_THROWS_NO_EXCEPTION(processor =
                                  std::make_unique<core::SignatureProcessor>(
                                      model.getCtx(), model.getPdfDoc()));
  QVERIFY(processor);
  QVERIFY(processor->findAcroForm() != nullptr);
  QVERIFY(processor->findSignatures());
  std::bitset<32> sig_flags = processor->getFormSigFlags();
  QVERIFY2(sig_flags.test(0), "SignatureExist flag is not set");
  QVERIFY2(sig_flags.test(1), "AppendOnly flag is not set");
  QVERIFY2(sig_flags.to_ulong() == 3 || sig_flags.to_ulong() == 1,
           "Unknown SigFlag is set");
  QVERIFY(processor->getSignaturesCount() == 1);
  auto signatures = processor->ParseSignatures();
  QVERIFY(signatures.size() == 1);
  QVERIFY(!signatures[0].getSigData().empty());
  QVERIFY(!signatures[0].getByteRanges().empty());
  const auto &byteranges = signatures[0].getByteRanges();
  const auto &sig_data = signatures[0].getSigData();
  const core::RangesVector expected_range{{0, 2808856}, {2868858, 396}};
  QVERIFY(!sig_data.empty());
  QVERIFY(byteranges == expected_range);
}

void TSignature::signaturesNumber() {
  QVERIFY(sigNumb(file1_, 1));
  QVERIFY(sigNumb(file2_, 1));
  QVERIFY(sigNumb(file3_, 5));
}

void TSignature::parseHexString() {
  std::vector<unsigned char> expected1{0x00, 0x01, 0x02, 0x03, 0x04, 0x05};
  std::vector<unsigned char> expected2{0x00, 0x01, 0x02, 0x03,
                                       0x04, 0x05, 0x00};
  std::vector<unsigned char> expected_fail;
  QVERIFY(core::utils::hexStringToByteArray("000102030405", 12) == expected1);
  QVERIFY(core::utils::hexStringToByteArray("0001020304050", 13) == expected2);
  QVERIFY(core::utils::hexStringToByteArray("<000102030405>", 14) == expected1);
  QVERIFY(core::utils::hexStringToByteArray("<0001020304050>", 15) ==
          expected2);
  QVERIFY(core::utils::hexStringToByteArray("<0001020304x050>", 16) ==
          expected_fail);
  QVERIFY(core::utils::hexStringToByteArray("<0001020304050", 14) == expected2);
  QVERIFY(core::utils::hexStringToByteArray("0001020304050>", 14) == expected2);
}

bool TSignature::sigNumb(const QString &file, int sig_expected) const {
  PdfDocModel model;
  model.setSource(file);
  core::SignatureProcessor prc(model.getCtx(), model.getPdfDoc());
  return prc.findSignatures() && (prc.getSignaturesCount() == sig_expected);
}

void TSignature::cBridge() {
  PdfDocModel model;
  model.setSource(file1_);
  std::unique_ptr<core::SignatureProcessor> processor = nullptr;
  QVERIFY_THROWS_NO_EXCEPTION(processor =
                                  std::make_unique<core::SignatureProcessor>(
                                      model.getCtx(), model.getPdfDoc()));
  QVERIFY(processor);
  QVERIFY(processor->findSignatures());
  std::vector<core::RawSignature> signatures = processor->ParseSignatures();
  QVERIFY(!signatures.empty());
  core::SignaturesValidator validator;
  validator.validateSignatures(signatures, file1_);
}

void TSignature::cBridgeCertList() {
  QString res = core::bridge_utils::getCertListJSON();
  QVERIFY(!res.isEmpty());
  qWarning() << res;
}

void TSignature::OneSigCoverage() {
  core::SignaturesValidator validator;
  // full coverage
  {
    const size_t file_size = 1602;
    const std::vector<unsigned char> sig_data(500, 0xFF);
    const core::RangesVector ranges{{0, 100}, {1102, 500}};
    const core::RawSignature rsig(sig_data, ranges);
    const core::SignaturesValidator::CoverageInfo info =
        validator.analyzeOneSigCoverage(rsig, file_size);
    QVERIFY(info.byteranges.size() > 1);
    QVERIFY(info.gaps.size() == 1);
    QVERIFY(info.gap_after_end == false);
    QVERIFY(info.coverage == file_size - info.sig_data_size);
    QVERIFY(!sig_data.empty());
    QVERIFY(info.gaps_size == file_size - info.coverage);
    QVERIFY(info.invalid_range == false);
    QVERIFY(info.full_coverage);
    QVERIFY(!info.can_be_casted_to_full_coverage);
  }
  // gap at the beginnig
  {
    const size_t file_size = 1602;
    const std::vector<unsigned char> sig_data(500, 0xFF);
    const core::RangesVector ranges{{1, 100}, {1102, 500}};
    const core::RawSignature rsig(sig_data, ranges);
    core::SignaturesValidator::CoverageInfo info =
        validator.analyzeOneSigCoverage(rsig, file_size);
    QVERIFY(info.byteranges.size() > 1);
    QVERIFY(info.gaps.size() == 2);
    QVERIFY(info.gap_after_end == false);
    QVERIFY(info.coverage ==
            file_size - info.sig_data_size); // coverage is equal the signature
                                          // size,gives nothing
    QVERIFY(!sig_data.empty());
    QVERIFY(info.gaps_size == file_size - info.coverage);
    QVERIFY(info.invalid_range == false);
    QVERIFY(info.full_coverage == false);
    QVERIFY(!info.can_be_casted_to_full_coverage);
  }
  // updated - gap at at the end can be casted to valid file
  {
    const size_t file_size = 1702;
    const std::vector<unsigned char> sig_data(500, 0xFF);
    const core::RangesVector ranges{{0, 100}, {1102, 500}};
    const core::RawSignature rsig(sig_data, ranges);
    const core::SignaturesValidator::CoverageInfo info =
        validator.analyzeOneSigCoverage(rsig, file_size);
    QVERIFY(info.full_coverage == false);
    QVERIFY(info.can_be_casted_to_full_coverage);
    QVERIFY(!info.invalid_range);
  }

  // updated - gap at at the end can be casted to valid file
  {
    const size_t file_size = 17020;
    const std::vector<unsigned char> sig_data(500, 0xFF);
    const core::RangesVector ranges{{0, 100}, {1102, 500}};
    const core::RawSignature rsig(sig_data, ranges);
    const core::SignaturesValidator::CoverageInfo info =
        validator.analyzeOneSigCoverage(rsig, file_size);
    QVERIFY(info.full_coverage == false);
    QVERIFY(info.can_be_casted_to_full_coverage);
    QVERIFY(!info.invalid_range);
  }
  // first gap < signature - can't be casted
  {
    const size_t file_size = 17020;
    const std::vector<unsigned char> sig_data(500, 0xFF);
    const core::RangesVector ranges{{0, 101}, {1102, 500}};
    const core::RawSignature rsig(sig_data, ranges);
    const core::SignaturesValidator::CoverageInfo info =
        validator.analyzeOneSigCoverage(rsig, file_size);
    QVERIFY(!info.full_coverage);
    QVERIFY(!info.can_be_casted_to_full_coverage);
    QVERIFY(!info.invalid_range);
  }
  // first gap > signature - can't be casted
  {
    const size_t file_size = 17020;
    const std::vector<unsigned char> sig_data(500, 0xFF);
    const core::RangesVector ranges{{0, 99}, {1102, 500}};
    const core::RawSignature rsig(sig_data, ranges);
    const core::SignaturesValidator::CoverageInfo info =
        validator.analyzeOneSigCoverage(rsig, file_size);
    QVERIFY(!info.full_coverage);
    QVERIFY(!info.can_be_casted_to_full_coverage);
    QVERIFY(!info.invalid_range);
  }
  // not full coverage
  {
      const size_t file_size = 1602;
      const std::vector<unsigned char> sig_data(499, 0xFF);
      const core::RangesVector ranges{{0, 100}, {1102, 500}};
      const core::RawSignature rsig(sig_data, ranges);
      const core::SignaturesValidator::CoverageInfo info =
          validator.analyzeOneSigCoverage(rsig, file_size);
      QVERIFY(!info.full_coverage);
      QVERIFY(!info.can_be_casted_to_full_coverage);
      QVERIFY(!info.invalid_range);
  }
  // one range
  {
      const size_t file_size = 1602;
      const std::vector<unsigned char> sig_data(500, 0xFF);
      const core::RangesVector ranges{{1002, 500}};
      const core::RawSignature rsig(sig_data, ranges);
      const core::SignaturesValidator::CoverageInfo info =
          validator.analyzeOneSigCoverage(rsig, file_size);
      QVERIFY(!info.full_coverage);
      QVERIFY(!info.can_be_casted_to_full_coverage);
      QVERIFY(!info.invalid_range);
  }
  // one range
  {
      const size_t file_size = 10000;
      const std::vector<unsigned char> sig_data(499, 0xFF);
      const core::RangesVector ranges{{0, 9000}};
      const core::RawSignature rsig(sig_data, ranges);
      const core::SignaturesValidator::CoverageInfo info =
          validator.analyzeOneSigCoverage(rsig, file_size);
      QVERIFY(!info.full_coverage);
      QVERIFY(!info.can_be_casted_to_full_coverage);
      QVERIFY(!info.invalid_range);
  }
  //empty sig
  {
      const size_t file_size = 10000;
      const std::vector<unsigned char> sig_data;
      const core::RangesVector ranges{{0, 5000},{5000,5000}};
      const core::RawSignature rsig(sig_data, ranges);
      const core::SignaturesValidator::CoverageInfo info =
          validator.analyzeOneSigCoverage(rsig, file_size);
      QVERIFY(!info.full_coverage);
      QVERIFY(!info.can_be_casted_to_full_coverage);
      QVERIFY(!info.invalid_range);
  }
  //three ranges - full coverage
  {
      const size_t file_size = 10000;
      const std::vector<unsigned char> sig_data(499,0xff);
      const core::RangesVector ranges{{0, 5000},{5000,1000},{7000,3000}};
      const core::RawSignature rsig(sig_data, ranges);
      const core::SignaturesValidator::CoverageInfo info =
          validator.analyzeOneSigCoverage(rsig, file_size);
      QVERIFY(info.full_coverage);
      QVERIFY(!info.can_be_casted_to_full_coverage);
      QVERIFY(!info.invalid_range);
  }
  // overlapping ranges -full coverage
  {
      const size_t file_size = 10000;
      const std::vector<unsigned char> sig_data(499,0xff);
      const core::RangesVector ranges{{0, 5000},{4000,2000},{7000,3000}};
      const core::RawSignature rsig(sig_data, ranges);
      const core::SignaturesValidator::CoverageInfo info =
          validator.analyzeOneSigCoverage(rsig, file_size);
      QVERIFY(info.full_coverage);
      QVERIFY(!info.can_be_casted_to_full_coverage);
      QVERIFY(!info.invalid_range);
  }
  // overlapping ranges - not full coverage
  {
      const size_t file_size = 10000;
      const std::vector<unsigned char> sig_data(499,0xff);
      const core::RangesVector ranges{{0, 5000},{4000,1500},{7000,3000}};
      const core::RawSignature rsig(sig_data, ranges);
      const core::SignaturesValidator::CoverageInfo info =
          validator.analyzeOneSigCoverage(rsig, file_size);
      QVERIFY(!info.full_coverage);
      QVERIFY(!info.can_be_casted_to_full_coverage);
      QVERIFY(!info.invalid_range);
  }
  // overlapping ranges - can be casted
  {
      const size_t file_size = 11000;
      const std::vector<unsigned char> sig_data(499,0xff);
      const core::RangesVector ranges{{0, 5000},{4000,2000},{7000,3000}};
      const core::RawSignature rsig(sig_data, ranges);
      const core::SignaturesValidator::CoverageInfo info =
          validator.analyzeOneSigCoverage(rsig, file_size);
      QVERIFY(!info.full_coverage);
      QVERIFY(info.can_be_casted_to_full_coverage);
      QVERIFY(!info.invalid_range);
  }
  // coverage > file size
  {
      const size_t file_size = 10000;
      const std::vector<unsigned char> sig_data(499,0xff);
      const core::RangesVector ranges{{0, 5000},{6000,6000}};
      const core::RawSignature rsig(sig_data, ranges);
      const core::SignaturesValidator::CoverageInfo info =
          validator.analyzeOneSigCoverage(rsig, file_size);
      QVERIFY(!info.full_coverage);
      QVERIFY(!info.can_be_casted_to_full_coverage);
      QVERIFY(info.invalid_range);
  }
}

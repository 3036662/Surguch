#include "t_signature.hpp"
#include "models/pdf_page_model.hpp"
#include "core/signature_processor.hpp"
#include "core/signatures_validator.hpp"
#include "core/utils.hpp"
#include <QTest>
#include <iostream>
#include <fstream>

TSignature::TSignature(QObject *parent)
    : QObject{parent}
{

}

void TSignature::testTrue(){
     QVERIFY(true);
}

void TSignature::testModelConstruct(){
    PdfPageModel model;
    model.setSource(file1_);
    QVERIFY(model.rowCount({})==14);
    QVERIFY(model.getCtx()!=nullptr);
    QVERIFY(model.getDoc()!=nullptr);
    QVERIFY(model.getPdfDoc()!=nullptr);
}


void TSignature::createProcessorFail(){
    QVERIFY_THROWS_EXCEPTION(std::runtime_error,core::SignatureProcessor(nullptr,nullptr));
}

void TSignature::createProcessor(){
    PdfPageModel model;
    model.setSource(file1_);
    std::unique_ptr<core::SignatureProcessor> processor=nullptr;
    QVERIFY_THROWS_NO_EXCEPTION(processor=std::make_unique<core::SignatureProcessor>(model.getCtx(),model.getPdfDoc()));
    QVERIFY(processor);
}


void TSignature::findSignatures(){
    PdfPageModel model;
    model.setSource(file1_);
    std::unique_ptr<core::SignatureProcessor> processor=nullptr;
    QVERIFY_THROWS_NO_EXCEPTION(processor=std::make_unique<core::SignatureProcessor>(model.getCtx(),model.getPdfDoc()));
    QVERIFY(processor);
    QVERIFY(processor->findAcroForm()!=nullptr);
    QVERIFY(processor->findSignatures());
    std::bitset<32> sig_flags=processor->getFormSigFlags();
    QVERIFY2(sig_flags.test(0),"SignatureExist flag is not set");
    QVERIFY2(sig_flags.test(1),"AppendOnly flag is not set");
    QVERIFY2(sig_flags.to_ulong()==3 || sig_flags.to_ulong()==1,"Unknown SigFlag is set");
    QVERIFY(processor->getSignaturesCount()==1);
    auto signatures=processor->ParseSignatures();
    QVERIFY(signatures.size()==1);
    QVERIFY(!signatures[0].getSigData().empty());
    QVERIFY(!signatures[0].getByteRanges().empty());
    const auto& byteranges=signatures[0].getByteRanges();
    const auto& sig_data=signatures[0].getSigData();
    const core::RangesVector expected_range{{0,2808856},{2868858,396}};
    QVERIFY(!sig_data.empty());
    QVERIFY(byteranges==expected_range);
}


void TSignature::signaturesNumber(){
    QVERIFY(sigNumb(file1_,1));
    QVERIFY(sigNumb(file2_,1));
    QVERIFY(sigNumb(file3_,5));
}

void TSignature::parseHexString(){
    std::vector<unsigned char> expected1{0x00,0x01,0x02,0x03,0x04,0x05};
    std::vector<unsigned char> expected2{0x00,0x01,0x02,0x03,0x04,0x05,0x00};
    std::vector<unsigned char> expected_fail;
    QVERIFY(core::utils::hexStringToByteArray("000102030405",12)==expected1);
    QVERIFY(core::utils::hexStringToByteArray("0001020304050",13)==expected2);
    QVERIFY(core::utils::hexStringToByteArray("<000102030405>",14)==expected1);
    QVERIFY(core::utils::hexStringToByteArray("<0001020304050>",15)==expected2);
    QVERIFY(core::utils::hexStringToByteArray("<0001020304x050>",16)==expected_fail);
    QVERIFY(core::utils::hexStringToByteArray("<0001020304050",14)==expected2);
    QVERIFY(core::utils::hexStringToByteArray("0001020304050>",14)==expected2);
}

bool TSignature::sigNumb(const QString& file,int sig_expected) const{
    PdfPageModel model;
    model.setSource(file);
    core::SignatureProcessor prc(model.getCtx(),model.getPdfDoc());
    return prc.findSignatures() && (prc.getSignaturesCount()==sig_expected);
}

void TSignature::cBridge(){
    PdfPageModel model;
    model.setSource(file1_);
    std::unique_ptr<core::SignatureProcessor> processor=nullptr;
    QVERIFY_THROWS_NO_EXCEPTION(processor=std::make_unique<core::SignatureProcessor>(model.getCtx(),model.getPdfDoc()));
    QVERIFY(processor);
    QVERIFY(processor->findSignatures());
    std::vector<core::RawSignature> signatures=processor->ParseSignatures();
    QVERIFY(!signatures.empty());
    core::SignaturesValidator validator;
    validator.validateSignatures(signatures,file1_);


}



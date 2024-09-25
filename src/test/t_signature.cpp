#include "t_signature.hpp"
#include "models/pdf_page_model.hpp"
#include "core/signature_processor.hpp"
#include <QTest>
#include <iostream>

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
}


void TSignature::signaturesNumber(){
    QVERIFY(sigNumb(file1_,1));
    QVERIFY(sigNumb(file2_,1));
    QVERIFY(sigNumb(file3_,5));
}

bool TSignature::sigNumb(const QString& file,int sig_expected) const{
    PdfPageModel model;
    model.setSource(file);
    core::SignatureProcessor prc(model.getCtx(),model.getPdfDoc());
    return prc.findSignatures() && (prc.getSignaturesCount()==sig_expected);
}

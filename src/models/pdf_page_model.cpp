#include "pdf_page_model.hpp"
#include <QThread>
#include "core/signature_processor.hpp"

// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay)

PdfPageModel::PdfPageModel(QObject *parent)
    : QAbstractListModel(parent),
      fzctx_{fz_new_context(nullptr, nullptr, 500000000)} {
  if (fzctx_ == nullptr) {
    qWarning("Error creating muPDF context");
    return;
  }
  fz_try(fzctx_) {
    fz_set_aa_level(fzctx_, 8);
    fz_register_document_handlers(fzctx_);
  }
  fz_catch(fzctx_) { fz_report_error(fzctx_); }
  // qWarning()<<"Model created"<< QThread::currentThreadId();
}

PdfPageModel::~PdfPageModel() {
  if (fzdoc_ != nullptr) {
    fz_drop_document(fzctx_, fzdoc_);
  }
  if (fzctx_ != nullptr) {
    fz_drop_context(fzctx_);
  }
}

QVariant PdfPageModel::headerData(int /*section*/,
                                  Qt::Orientation /*orientation*/,
                                  int /*role*/) const {
  return {};
}

int PdfPageModel::rowCount(const QModelIndex &parent) const {
  // For list models only the root node (an invalid parent) should return the
  // list's size. For all other (valid) parents, rowCount() should return 0 so
  // that it does not become a tree model.
  if (parent.isValid()) {
    return 0;
  }
  return page_count_;
}

QVariant PdfPageModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid()) {
    return {};
  }
  if (index.row() < 0 || index.row() > page_count_ - 1) {
    return {};
  }

  if (role == Qt::DisplayRole) {
    return index.row();
  }
  return {};
}

void PdfPageModel::setSource(const QString &path) {
  fz_drop_document(fzctx_, fzdoc_);
  fz_drop_context(fzctx_);
  fzctx_ = fz_new_context(nullptr, nullptr, 500000000);
  fz_try(fzctx_) {
    fz_set_aa_level(fzctx_, 8);
    fz_register_document_handlers(fzctx_);
  }
  fz_catch(fzctx_) { fz_report_error(fzctx_); }
  const QString prefix = "file://";
  QString file_path;
  if (path.startsWith(prefix)) {
    file_path = path.mid(prefix.length());
  } else {
    file_path = path;
  }
  fz_try(fzctx_) {
    // open the pdf file
    fzdoc_ = fz_open_document(fzctx_, file_path.toStdString().c_str());
    if (fzdoc_ == nullptr) {
      qWarning("Can't open file");
    }
    pdfdoc_ = pdf_specifics(fzctx_, fzdoc_);
    if (pdfdoc_ == nullptr) {
      qWarning("Not a pdf document");
    }
    emit beginResetModel();
    file_source_ = file_path;
    // get the number of pages
    page_count_ = fz_count_pages(fzctx_, fzdoc_);
    emit endResetModel();
  }
  fz_catch(fzctx_) {
    qWarning() << fz_caught_message(fzctx_);
    fz_caught(fzctx_);
  }
  if (process_signatures_){
  processSignatures();
  }
}

fz_document *PdfPageModel::getDoc() const { return fzdoc_; }

fz_context *PdfPageModel::getCtx() const { return fzctx_; }

pdf_document* PdfPageModel::getPdfDoc() const {return pdfdoc_;}

void PdfPageModel::redrawAll() {
  beginResetModel();
  endResetModel();
}

void PdfPageModel::processSignatures() {
    std::unique_ptr<core::SignatureProcessor> prc;
    const QString err_str="[PdfPageModel] Error processing signatures ";
    try{
        prc=std::make_unique<core::SignatureProcessor>(fzctx_,pdfdoc_);
    } catch (const std::exception& ex){
        qWarning()<<err_str<<ex.what();
        return;
    }
    if (!prc->findSignatures()){
        qWarning()<<err_str;
        return;
    }
    std::vector<core::RawSignature> signatures=prc->ParseSignatures();
    emit signaturesFound(signatures,file_source_);
    qWarning()<< "signatures found "<<signatures.size();
}

// NOLINTEND(cppcoreguidelines-avoid-do-while,cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay)

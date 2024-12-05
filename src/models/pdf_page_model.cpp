#include "pdf_page_model.hpp"
#include "core/signature_processor.hpp"
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QThread>
#include <QUrl>

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
  file_source_.clear(); // drop current source to allow deletion
  processFileDelete();  // delete temp files
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
  file_source_.clear();
  processFileDelete();
  fzctx_ = fz_new_context(nullptr, nullptr, 500000000);
  fz_try(fzctx_) {
    fz_set_aa_level(fzctx_, 8);
    fz_register_document_handlers(fzctx_);
  }
  fz_catch(fzctx_) { fz_report_error(fzctx_); }

  const QString file_path = QUrl(path).toString(QUrl::PreferLocalFile);
  const QFile finfo(file_path);
  const std::string local_path_std = file_path.toStdString();
  if (!finfo.exists()) {
    qWarning() << "[PdfPageModel::setSource] file does not exist" << file_path;
    return;
  }

  fz_try(fzctx_) {
    // open the pdf file
    fzdoc_ = fz_open_document(fzctx_, local_path_std.c_str());
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
  if (process_signatures_) {
    processSignatures();
  }
}

Q_INVOKABLE QString PdfPageModel::getSource() const { return file_source_; };

fz_document *PdfPageModel::getDoc() const { return fzdoc_; }

fz_context *PdfPageModel::getCtx() const { return fzctx_; }

pdf_document *PdfPageModel::getPdfDoc() const { return pdfdoc_; }

void PdfPageModel::redrawAll() {
  beginResetModel();
  endResetModel();
}

void PdfPageModel::processSignatures() {
  std::unique_ptr<core::SignatureProcessor> prc;
  const QString err_str = "[PdfPageModel] Error processing signatures ";
  try {
    prc = std::make_unique<core::SignatureProcessor>(fzctx_, pdfdoc_);
  } catch (const std::exception &ex) {
    qWarning() << err_str << ex.what();
    return;
  }
  if (!prc->findSignatures()) {
    qWarning() << err_str;
    return;
  }
  std::vector<core::RawSignature> signatures = prc->ParseSignatures();
  emit signaturesCounted(signatures.size());
  emit signaturesFound(signatures, file_source_);
  qWarning() << "signatures found " << signatures.size();
}

void PdfPageModel::processFileDelete() {
  if (!process_file_delete_) {
    return;
  }
  std::vector<QString> resulting_queue;
  for (const auto &path : tmp_files_to_delete_) {
    // don't delete curr source
    if (path == file_source_) {
      resulting_queue.push_back(path);
      continue;
    }
    QFile file(path);
    if (!file.exists()) {
      qWarning() << "file " << path << " does not exist";
      continue;
    }
    if (!file.remove()) {
      qWarning() << "Failed to remove file:" << path;
      resulting_queue.push_back(path);
    }
  }
  tmp_files_to_delete_ = std::move(resulting_queue);
}

Q_INVOKABLE void PdfPageModel::deleteFileLater(QString path) {
  if (!process_file_delete_) {
    return;
  }
  tmp_files_to_delete_.emplace_back(std::move(path));
}

Q_INVOKABLE bool PdfPageModel::saveCurrSourceTo(QString path,
                                                bool delete_curr_source) {
  const QString dest_path = QUrl(path).toString(QUrl::PreferLocalFile);
  QFile src_file(file_source_);
  if (!src_file.exists()) {
    qWarning() << "[SaveCurrSourceTo] source file does not exist";
    return false;
  }
  QFile dest_file(dest_path);
  if (dest_file.exists()) {
    dest_file.remove();
  }

  if (!src_file.copy(dest_path)) {
    qWarning() << "Failed to copy file to " << dest_path;
    return false;
  }
  // Put the current source in the queue of files that should be deleted.
  if (delete_curr_source) {
    tmp_files_to_delete_.emplace_back(file_source_);
  }
  return true;
}

void PdfPageModel::showInFolder(){
  QUrl folder_url=QUrl::fromLocalFile( QFileInfo(file_source_).absoluteDir().absolutePath());
  QDesktopServices::openUrl(folder_url);
}

// NOLINTEND(cppcoreguidelines-avoid-do-while,cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay)

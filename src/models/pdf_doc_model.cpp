#include "pdf_doc_model.hpp"
#include "core/signature_processor.hpp"
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QGuiApplication>
#include <QScreen>
#include <QThread>
#include <QUrl>
#include <QWindow>

// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay)

PdfDocModel::PdfDocModel(QObject *parent)
    : QAbstractListModel(parent),
      fzctx_{fz_new_context(nullptr, nullptr, 500000000)} {
  if (fzctx_ == nullptr) {
    qWarning("Error creating muPDF context");
    return;
  }
  fz_try(fzctx_) {
    fz_set_aa_level(fzctx_, 0);
    fz_register_document_handlers(fzctx_);
  }
  fz_catch(fzctx_) { fz_report_error(fzctx_); }
  // ---------------------------
  // watch for current screen dpi
  QGuiApplication *app =
      dynamic_cast<QGuiApplication *>(QGuiApplication::instance());
  QWindow *p_window = nullptr;
  QScreen *p_screen = nullptr;
  QWindowList window_list;
  if (app && !(window_list = app->topLevelWindows()).isEmpty() &&
      (p_window = window_list.at(0)) != nullptr &&
      (p_screen = p_window->screen()) != nullptr) {
    physical_screen_dpi_ = p_screen->physicalDotsPerInch();
    screenDpiChanged();
    // catch change dpi event
    connect(p_window, &QWindow::screenChanged, [this](QScreen *screen) {
      if (screen != nullptr && process_signatures_) { // if main view
        physical_screen_dpi_ = screen->physicalDotsPerInch();
        screenDpiChanged();
        redrawAll();
      }
    });
  }
  // qWarning()<<"Model created"<< QThread::currentThreadId();
}

PdfDocModel::~PdfDocModel() {
  if (fzdoc_ != nullptr) {
    fz_drop_document(fzctx_, fzdoc_);
  }
  if (fzctx_ != nullptr) {
    fz_drop_context(fzctx_);
  }
  file_source_.clear(); // drop current source to allow deletion
  processFileDelete();  // delete temp files
}

QVariant PdfDocModel::headerData(int /*section*/,
                                 Qt::Orientation /*orientation*/,
                                 int /*role*/) const {
  return {};
}

int PdfDocModel::rowCount(const QModelIndex &parent) const {
  // For list models only the root node (an invalid parent) should return the
  // list's size. For all other (valid) parents, rowCount() should return 0 so
  // that it does not become a tree model.
  if (parent.isValid()) {
    return 0;
  }
  return page_count_;
}

QVariant PdfDocModel::data(const QModelIndex &index, int role) const {
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

void PdfDocModel::setSource(const QString &path) {
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
    qWarning() << "[PdfDocModel::setSource] file does not exist" << file_path;
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

Q_INVOKABLE QString PdfDocModel::getSource() const { return file_source_; };

fz_document *PdfDocModel::getDoc() const { return fzdoc_; }

fz_context *PdfDocModel::getCtx() const { return fzctx_; }

pdf_document *PdfDocModel::getPdfDoc() const { return pdfdoc_; }

void PdfDocModel::redrawAll() {
  qWarning()<<"redraw all";
  beginResetModel();
  endResetModel();
}

void PdfDocModel::processSignatures() {
  std::unique_ptr<core::SignatureProcessor> prc;
  const QString err_str = "[PdfDocModel] Error processing signatures ";
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

void PdfDocModel::processFileDelete() {
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

Q_INVOKABLE void PdfDocModel::deleteFileLater(QString path) {
  if (!process_file_delete_) {
    return;
  }
  tmp_files_to_delete_.emplace_back(std::move(path));
}

Q_INVOKABLE bool PdfDocModel::saveCurrSourceTo(QString path,
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

void PdfDocModel::showInFolder() {
  QUrl folder_url =
      QUrl::fromLocalFile(QFileInfo(file_source_).absoluteDir().absolutePath());
  QDesktopServices::openUrl(folder_url);
}

// NOLINTEND(cppcoreguidelines-avoid-do-while,cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay)

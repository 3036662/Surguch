/* File: pdf_doc_model.cpp
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

#include "pdf_doc_model.hpp"

#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QGuiApplication>
#include <QMimeDatabase>
#include <QScreen>
#include <QThread>
#include <QUrl>
#include <QWindow>

#include "core/signature_processor.hpp"

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
    QWindow *p_window = nullptr;
    QScreen *p_screen = nullptr;
    const QWindowList window_list = QGuiApplication::topLevelWindows();
    if (!window_list.isEmpty() && (p_window = window_list.at(0)) != nullptr &&
        (p_screen = p_window->screen()) != nullptr) {
        physical_screen_dpi_ = p_screen->physicalDotsPerInch();
        screenDpiChanged();
        // catch change dpi event
        connect(p_window, &QWindow::screenChanged, [this](QScreen *screen) {
            if (screen != nullptr && process_signatures_) {  // if main view
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
    file_source_.clear();  // drop current source to allow deletion
    processFileDelete();   // delete temp files
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

/// @brief setSource open new pdf file
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
    const QMimeDatabase mime_database;
    const QMimeType mime_type = mime_database.mimeTypeForFile(file_path);
    const std::string local_path_std = file_path.toStdString();
    if (!finfo.exists()) {
        qWarning() << "[PdfDocModel::setSource] file does not exist"
                   << file_path;
        emit errorOpenFile(tr("File does not exist"));
        return;
    }
    if (mime_type.name() != "application/pdf") {
        emit errorOpenFile(tr("Wrong file type"));
        return;
    }

    bool mu_exception_caught = false;
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
        page_count_ = fz_count_pages(fzctx_, fzdoc_);
        // if not a valid pdf
        if (fzdoc_ == nullptr || pdfdoc_ == nullptr || page_count_ <= 0) {
            file_source_ = "";
            emit errorOpenFile(tr("Can not open file"));
        } else {
            file_source_ = file_path;
        }
        // get the number of pages
        emit endResetModel();
    }
    fz_catch(fzctx_) {
        qWarning() << fz_caught_message(fzctx_);
        mu_exception_caught = true;
        fz_caught(fzctx_);
        file_source_ = "";
    }
    if (mu_exception_caught) {
        emit errorOpenFile(tr("Can not open file"));
        file_source_ = "";
        return;
    }
    if (process_signatures_) {
        processSignatures();
    }
}

/// @brief get current source path
Q_INVOKABLE QString PdfDocModel::getSource() const { return file_source_; };

fz_document *PdfDocModel::getDoc() const { return fzdoc_; }

fz_context *PdfDocModel::getCtx() const { return fzctx_; }

pdf_document *PdfDocModel::getPdfDoc() const { return pdfdoc_; }

/// @brief resert the whole model
void PdfDocModel::redrawAll() {
    // qWarning() << "redraw all";
    beginResetModel();
    endResetModel();
}

/// @brief find all signatures
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
    const std::vector<core::RawSignature> signatures = prc->ParseSignatures();
    emit signaturesCounted(static_cast<int>(signatures.size()));
    emit signaturesFound(signatures, file_source_);
    qWarning() << "signatures found " << signatures.size();
}

/// @brief delete all files scheduled for deletion
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

/// @brief schedule the given file for deletion
Q_INVOKABLE void PdfDocModel::deleteFileLater(QString path) {
    if (!process_file_delete_) {
        return;
    }
    tmp_files_to_delete_.emplace_back(std::move(path));
}

/// @brief the 'save file as' implementation
Q_INVOKABLE bool PdfDocModel::saveCurrSourceTo(const QString &path,
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

/// @brief Open a folder that contains the current file in the file browser.
void PdfDocModel::showInFolder() {
    const QUrl folder_url = QUrl::fromLocalFile(
        QFileInfo(file_source_).absoluteDir().absolutePath());
    QDesktopServices::openUrl(folder_url);
}

// NOLINTEND(cppcoreguidelines-avoid-do-while,cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay)

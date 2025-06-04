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
#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrent>
#include <QGuiApplication>
#include <QImage>
#include <QMimeDatabase>
#include <QScreen>
#include <QThread>
#include <QUrl>
#include <QWindow>

#include "core/signature_processor.hpp"
#include "core/utils.hpp"

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
    if (fzdoc_text_ != nullptr) {
        fz_drop_document(fzctx_text_, fzdoc_text_);
    }
    if (fzctx_text_ != nullptr) {
        fz_drop_context(fzctx_text_);
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
        fz_set_aa_level(fzctx_, 0);
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
    bool was_repaired = false;
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
        if (pdf_was_repaired(fzctx_, pdfdoc_) > 0) {
            was_repaired = true;
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
        if (was_repaired && process_signatures_) {
            emit docWasRepaired();
        }
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
    // Extract text
    if (extract_text_) {
        fz_drop_document(fzctx_text_, fzdoc_text_);
        fz_drop_context(fzctx_text_);
        fzctx_text_ = fz_new_context(nullptr, nullptr, 100000000);
        bool text_ctx_err_catched = false;
        fz_var(fzdoc_text_);
        fz_var(text_ctx_err_catched);
        fz_try(fzctx_text_) {
            fz_set_aa_level(fzctx_text_, 0);
            fz_register_document_handlers(fzctx_text_);
            fzdoc_text_ = fz_open_document(fzctx_text_, local_path_std.c_str());
            if (fzdoc_text_ == nullptr) {
                qWarning("Can't open file");
            }
        }
        fz_catch(fzctx_text_) {
            text_ctx_err_catched = true;
            fz_report_error(fzctx_text_);
        }
        if (!text_ctx_err_catched) {
            text_extractor_ =
                std::make_unique<core::TextExtractor>(fzctx_text_, fzdoc_text_);
            text_extractor_->updateCache();
            QObject::connect(text_extractor_.get(),
                             &core::TextExtractor::searchCompleted, this,
                             &PdfDocModel::handleSearchCompleted);
        };
    }
}

/// @brief get current source path
Q_INVOKABLE QString PdfDocModel::getSource() const { return file_source_; };

fz_document *PdfDocModel::getDoc() const { return fzdoc_; }

fz_context *PdfDocModel::getCtx() const { return fzctx_; }

pdf_document *PdfDocModel::getPdfDoc() const { return pdfdoc_; }

/// @brief resert the whole model
void PdfDocModel::redrawAll() {
    qWarning() << "[PdfDocModel] redraw all";
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
        std::ignore = dest_file.remove();
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

/// @brief returns a vector of rectangles to highligt
PdfDocModel::NeedleRectsOnPage PdfDocModel::getNeedlesForPage(
    size_t page_index) {
    // qWarning() << "getNeedlesForPage" << page_index;
    if (!text_extractor_) {
        return nullptr;
    }
    return text_extractor_->getNeedlesForPage(page_index);
}

/// @brief search for text
void PdfDocModel::performSearch(QString needle) {
    qWarning() << "search for " << needle;
    if (text_extractor_) {
        text_extractor_->performSearch(needle, false);
    }
}

void PdfDocModel::handleSearchCompleted() {
    if (!text_extractor_) {
        return;
    }
    const auto needles_total = text_extractor_->getNeedlesTotal();
    if (needles_total > std::numeric_limits<int>::max()) {
        qWarning() << "[PdfDocModel] needles_total is too big";
        return;
    }
    const auto needle = text_extractor_->getNeedlePageAndXY(0);
    qWarning() << "first needle was found on page" << needle.first;
    if (needle.first > std::numeric_limits<int>::max()) {
        qWarning() << "[handleSearchCompleted] page index is to big for int";
    }
    emit searchCompleted(static_cast<int>(needle.first),
                         static_cast<int>(needles_total), needle.second.first,
                         needle.second.second);
}

void PdfDocModel::jumpToNeedle(int needle_index) {
    if (needle_index < 0 || !text_extractor_) {
        return;
    }
    const auto needle = text_extractor_->getNeedlePageAndXY(needle_index);
    if (needle.first > std::numeric_limits<int>::max()) {
        qWarning()
            << "[PdfDocModel::jumpToNeedle] the page index is to big for int";
    }
    emit jumpToNeedleCompleted(static_cast<int>(needle.first),
                               needle.second.first, needle.second.second);
    // qWarning() << "[PdfDocModel] Jump to needle " << needle_index;
}

std::shared_ptr<core::TextExtractor::RectToHiglightCurrent>
PdfDocModel::getCurrentNeedleRect(size_t page_index) {
    if (!text_extractor_) {
        return nullptr;
    }
    return text_extractor_->getCurrentNeedleRect(page_index);
}

void PdfDocModel::placeRubberStamp(const QVariantMap& qvparams){
    auto params = prepareParams(qvparams);
    auto params_wrapper = createParams(params);
    qWarning() << "[PlaceRubberStamp]" << "parsed and ready to place tags";
    image_watcher_ = std::make_unique<ImageFutureWatcher>();
    QObject::connect(image_watcher_.get(), &ImageFutureWatcher::finished,
                     [this]() {
                         // qWarning() << "finished";
                         saveImage();
                     });
    image_future_ = std::make_unique<ImageFuture>(
        QtConcurrent::run(core::gui::prepareImage, params_wrapper));
    image_watcher_->setFuture(*image_future_);


}

QList<std::shared_ptr<core::gui::RubberStamp>> PdfDocModel::getRubberStampForPage(size_t page_index) const {
    if (!history_manager_) {
        return {};
    }
    return history_manager_->getActionsOnPage(page_index);
}

void PdfDocModel::undoRubberStamp(){
    history_manager_->undoAction();

    emit updateDoc();
}

void PdfDocModel::redoRubberStamp(){
    history_manager_->redoAction();

    emit updateDoc();
}


void PdfDocModel::saveImage() {
    if (!history_manager_) {
        history_manager_ = std::make_unique<core::gui::HistoryManager>();
    }
    if (image_future_ && image_future_->isValid()) {
        history_manager_->addAction(std::make_unique<core::gui::RubberStamp>(
    core::gui::RubberStamp{
        .page_index = page_index_,
        .position_x = position_x_,
        .position_y = position_y_,
        .qml_width = page_width_,
        .qml_height = page_height_,
        .res = image_future_->takeResult()
    }));
    }
    emit imageReady();
}

core::gui::RubberParams PdfDocModel::prepareParams(const QVariantMap &qvparams)  {
    if (qvparams.contains("page_index")) {
        page_index_ = qvparams.value("page_index").toUInt();
    }
    if (qvparams.contains("stamp_x")) {
        position_x_ = qvparams.value("stamp_x").toDouble();
    }
    if (qvparams.contains("stamp_y")) {
        position_y_ = qvparams.value("stamp_y").toDouble();
    }
    if (qvparams.contains("page_width")) {
        page_width_ = qvparams.value("page_width").toDouble();
    }
    if (qvparams.contains("page_height")) {
        page_height_ = qvparams.value("page_height").toDouble();
    }
    if (qvparams.contains("stamp_width")) {
        params.stamp_width = qvparams.value("stamp_width").toUInt();
        //params.stamp_width = 400;
    }
    if (qvparams.contains("stamp_height")) {
        //params.stamp_height = qvparams.value("height").toUInt();
        params.stamp_height = 400;
    }
    if (qvparams.contains("annotation_width")) {
        params.annotation_width = qvparams["annotation_width"].toUInt();
    }
    if (qvparams.contains("border_width")) {
        params.border_width = qvparams.value("border_width").toUInt();
    }
    if (qvparams.contains("border_radius")) {
        params.border_radius = qvparams.value("border_radius").toUInt();
    }
    if (qvparams.contains("bg_transparent")) {
        params.bg_transparent = qvparams.value("bg_transparent").toBool();
    }
    if (qvparams.contains("create_from_image")) {
        params.create_from_image = qvparams.value("create_from_image").toBool();
    }
    if (qvparams.contains("stamp_preserve_ratio")) {
        //params.stamp_preserve_ratio = qvparams.value("stamp_preserve_ratio").toBool();
        params.stamp_preserve_ratio = true;
    }
    if (qvparams.contains("bg_opacity")) {
        params.bg_opacity = qvparams.value("bg_opacity").toUInt();
    }
    if (qvparams.contains("font_size")) {
        params.font_size = qvparams.value("font_size").toUInt();
    }
    if (qvparams.contains("font_weight")) {
        //params.font_weight = qvparams.value("font_weight").toUInt();
        params.font_weight = 400;
    }
    if (qvparams.contains("img_path")) {
        params.img_path = qvparams.value("img_path").toUrl().toLocalFile();
        if (params.img_path.isEmpty()) {
            params.img_path = qvparams.value("img_path").toString();
        }
    }
    if (qvparams.contains("annotation_text")) {
        params.annotation_text = qvparams.value("annotation_text").toString();
    }
    if (qvparams.contains("font_family")) {
        params.font_family = qvparams.value("font_family").toString();
    }
    if (qvparams.contains("border_color_red")) {
        params.border_color.R = qvparams.value("border_color_red").toUInt();
    }
    if (qvparams.contains("border_color_green")) {
        params.border_color.G = qvparams.value("border_color_green").toUInt();
    }
    if (qvparams.contains("border_color_blue")) {
        params.border_color.B = qvparams.value("border_color_blue").toUInt();
    }
    if (qvparams.contains("text_color_red")) {
        params.text_color.R = qvparams.value("text_color_red").toUInt();
    }
    if (qvparams.contains("text_color_green")) {
        params.text_color.G = qvparams.value("text_color_green").toUInt();
    }
    if (qvparams.contains("text_color_blue")) {
        params.text_color.B = qvparams.value("text_color_blue").toUInt();
    }
    if (qvparams.contains("bg_color_red")) {
        params.bg_color.R = qvparams.value("bg_color_red").toUInt();
    }
    if (qvparams.contains("bg_color_green")) {
        params.bg_color.G = qvparams.value("bg_color_green").toUInt();
    }
    if (qvparams.contains("bg_color_blue")) {
        params.bg_color.B = qvparams.value("bg_color_blue").toUInt();
    }
    return params;
}

 core::gui::SharedParamWrapper PdfDocModel::createParams(const core::gui::RubberParams& params) const {
    auto paramswrapper = std::make_shared<core::gui::CRubberParamsWrapper>();
    pdfcsp::pdf::RubberStampParams &pod_params = paramswrapper->pod_params;
    paramswrapper->qb_img_path = params.img_path.toUtf8();
    if (!paramswrapper->qb_img_path.isEmpty()) {
        pod_params.src_img_path = paramswrapper->qb_img_path.data();
    }
    pod_params.target_x = params.stamp_width;
    pod_params.target_y = params.stamp_height;
    pod_params.stamp_preserve_ratio = params.stamp_preserve_ratio;
    pod_params.create_from_image = params.create_from_image;
    paramswrapper->qb_annotation_text = params.annotation_text.toUtf8();
    if (!paramswrapper->qb_annotation_text.isEmpty()) {
        pod_params.annotation_text = paramswrapper->qb_annotation_text.data();
    }
    pod_params.bg_color.red = params.bg_color.R;
    pod_params.bg_color.green = params.bg_color.G;
    pod_params.bg_color.blue = params.bg_color.B;
    pod_params.font_color.red = params.text_color.R;
    pod_params.font_color.green = params.text_color.G;
    pod_params.font_color.blue = params.text_color.B;
    pod_params.border_color.red = params.border_color.R;
    pod_params.border_color.green = params.border_color.G;
    pod_params.border_color.blue = params.border_color.B;
    paramswrapper->qb_font_family = params.font_family.toUtf8();
    if (!paramswrapper->qb_font_family.isEmpty()) {
        pod_params.font_family = paramswrapper->qb_font_family.data();
    }
    pod_params.border_radius = params.border_radius;
    pod_params.border_width = params.border_width;
    pod_params.font_size = 1; //params.font_size;
    pod_params.font_weight = 400; //params.font_weight;
    pod_params.bg_transparent = params.bg_transparent;
    pod_params.bg_opacity = params.bg_opacity;
    pod_params.annotation_width = params.annotation_width;
    return paramswrapper;
}

// NOLINTEND(cppcoreguidelines-avoid-do-while,cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay)

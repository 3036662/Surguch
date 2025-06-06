/* File: pdf_doc_model.hpp
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

#ifndef pdf_doc_model_HPP
#define pdf_doc_model_HPP

#include <QAbstractListModel>
#include <QVariant>

#include "core/raw_signature.hpp"
#include "core/text_extractor.hpp"

#include "gui_core/gui_utils.hpp"
#include "gui_core/history_manager.hpp"
#include "mupdf/fitz.h"
#include "mupdf/pdf.h"
#include "pdf_csp_c.hpp"

class PdfDocModel : public QAbstractListModel {
    Q_OBJECT
    using NeedleRectsOnPage = core::utils::NeedleRectsOnPage;

   public:
    explicit PdfDocModel(QObject *parent = nullptr);

    PdfDocModel(const PdfDocModel &) = delete;
    PdfDocModel(PdfDocModel &&) = delete;
    PdfDocModel &operator=(const PdfDocModel &) = delete;
    PdfDocModel &operator=(PdfDocModel &&) = delete;
    ~PdfDocModel() override;

    using ImageFuture = QFuture<std::unique_ptr<core::gui::BakeRubberResult>>;
    using ImageFutureWatcher = QFutureWatcher<std::unique_ptr<core::gui::BakeRubberResult>>;

    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation,
                                      int role) const override;

    [[nodiscard]] int rowCount(const QModelIndex &parent) const override;

    [[nodiscard]] QVariant data(const QModelIndex &index,
                                int role) const override;

    /// @brief setSource open new pdf file
    Q_INVOKABLE void setSource(const QString &path);

    /// @brief get current source path
    [[nodiscard]] Q_INVOKABLE QString getSource() const;

    /// @brief resert the whole model
    Q_INVOKABLE void redrawAll();

    /// @brief the 'save file as' implementation
    Q_INVOKABLE bool saveCurrSourceTo(const QString &path,
                                      bool delete_curr_source);

    /// @brief schedule the given file for deletion
    Q_INVOKABLE void deleteFileLater(QString path);

    /// @brief Open a folder that contains the current file in the file browser.
    Q_INVOKABLE void showInFolder();

    /**
     *  @details There is no need to read signatures when the model is utilized
     * to render pdf previews.
     */
    Q_PROPERTY(bool mustProcessSignatures MEMBER process_signatures_);

    /// @brief this model must delete temporary files if TRUE
    Q_PROPERTY(bool mustDeleteTmpFiles MEMBER process_file_delete_)

    /// @brief current screen dpi for render and scaling purposes
    Q_PROPERTY(
        qreal screenDpi MEMBER physical_screen_dpi_ NOTIFY screenDpiChanged)

    /// @brief Extract the text for search purposes.
    Q_PROPERTY(bool mustExtractText MEMBER extract_text_)

    /// low level getters to connect the MuPDF model with renderer
    [[nodiscard]] Q_INVOKABLE fz_document *getDoc() const;
    [[nodiscard]] Q_INVOKABLE fz_context *getCtx() const;
    [[nodiscard]] Q_INVOKABLE pdf_document *getPdfDoc() const;

    /// @brief create rubber stamps on document
    Q_INVOKABLE void placeRubberStamp(const QVariantMap &qvparams);

    /// @brief create image to get default tag height
    Q_INVOKABLE void prepareImage(const QVariantMap &qvparams);

    /// @brief return a vector of stamps to render
    [[nodiscard]] Q_INVOKABLE std::vector<std::shared_ptr<core::gui::RubberStamp>> getRubberStampForPage(size_t page_index) const;

    /// @brief undo last placed stamp
    Q_INVOKABLE void undoRubberStamp();

    /// @brief redo last removed stamp
    Q_INVOKABLE void redoRubberStamp();

    /// @brief clear history
    Q_INVOKABLE void clearHistory() const;

    /// @brief get annot params for embedding in pdf
    [[nodiscard]] Q_INVOKABLE std::vector<pdfcsp::pdf::CAnnotParams> getAnnotParams() const;

    /// @brief returns a vector of rectangles to highligt
    [[nodiscard]] Q_INVOKABLE NeedleRectsOnPage
    getNeedlesForPage(size_t page_index);

    /// @brief search for text
    Q_INVOKABLE void performSearch(QString needle);

    Q_INVOKABLE void jumpToNeedle(int needle_index);

    Q_INVOKABLE std::shared_ptr<core::TextExtractor::RectToHiglightCurrent>
    getCurrentNeedleRect(size_t page_index);

   signals:

    /// @brief some signatures found
    void signaturesFound(std::vector<core::RawSignature> sigs,
                         QString file_path);

    /// @brief The signatures were counted.
    void signaturesCounted(int sig_count);

    /// @brief Current screen DPI was changed.
    void screenDpiChanged();

    void errorOpenFile(const QString &err_string);

    void docWasRepaired();

    /// @brief search is completed
    void searchCompleted(int first_needle_page_index, int total_needles,
                         float x_position, float y_position);

    /// @brief jump to needle by index completed
    void jumpToNeedleCompleted(int page_index, float rel_x, float rel_y);

    /// @brief size estimated
    void sizeReady(int height);

    /// @brief signal for update after undo or redo
    void updateDoc();

   private slots:
    void handleSearchCompleted();

   private:
    /// @brief parse and prepare params we get from QML
    [[nodiscard]] core::gui::RubberParams prepareParams(const QVariantMap &qvparams) ;

    /// @brief Gather all parameters (pdfcsp::pdf::CSignParam)
    [[nodiscard]] core::gui::SharedParamWrapper createParams(const core::gui::RubberParams &params) const;

    /// @brief get data from csp lib and send estimated sizes
    void estimateTagHeight();

    /// @brief find all signatures
    void processSignatures();

    /// @brief delete all files scheduled for deletion
    void processFileDelete();

    /// @brief save generated image to history_manager_
    void saveImage();

    fz_context *fzctx_ = nullptr;
    QString file_source_;
    fz_document *fzdoc_ = nullptr;
    pdf_document *pdfdoc_ = nullptr;
    int page_count_ = 0;
    bool process_signatures_ = false;
    bool process_file_delete_ = false;
    qreal physical_screen_dpi_ = 72;  // default MuPDF DPI
    std::vector<QString> tmp_files_to_delete_;
    bool extract_text_ = false;
    std::unique_ptr<core::TextExtractor> text_extractor_;
    std::unique_ptr<ImageFuture> image_future_;
    std::unique_ptr<ImageFutureWatcher> image_watcher_;
    std::unique_ptr<core::gui::HistoryManager> history_manager_;
    core::gui::RubberParams params;

    fz_context *fzctx_text_ = nullptr;
    fz_document *fzdoc_text_ = nullptr;
};

#endif  // pdf_doc_model_HPP

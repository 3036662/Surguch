#ifndef pdf_doc_model_HPP
#define pdf_doc_model_HPP

#include "core/raw_signature.hpp"
#include "mupdf/fitz.h"
#include "mupdf/pdf.h"
#include <QAbstractListModel>

class PdfDocModel : public QAbstractListModel {
  Q_OBJECT

public:
  explicit PdfDocModel(QObject *parent = nullptr);

  PdfDocModel(const PdfDocModel &) = delete;
  PdfDocModel(PdfDocModel &&) = delete;
  PdfDocModel &operator=(const PdfDocModel &) = delete;
  PdfDocModel &operator=(PdfDocModel &&) = delete;
  ~PdfDocModel() override;

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
   *  @details There is no need to read signatures when the model is utilized to
   * render pdf previews.
   */
  Q_PROPERTY(bool mustProcessSignatures MEMBER process_signatures_);

  /// @brief this model must delete temporary files if TRUE
  Q_PROPERTY(bool mustDeleteTmpFiles MEMBER process_file_delete_)

  /// @brief current screen dpi for render and scaling purposes
  Q_PROPERTY(
      qreal screenDpi MEMBER physical_screen_dpi_ NOTIFY screenDpiChanged)

  /// low level getters to connect the MuPDF model with renderer
  [[nodiscard]] Q_INVOKABLE fz_document *getDoc() const;
  [[nodiscard]] Q_INVOKABLE fz_context *getCtx() const;
  [[nodiscard]] Q_INVOKABLE pdf_document *getPdfDoc() const;

signals:

  /// @brief some signatures found
  void signaturesFound(std::vector<core::RawSignature> sigs, QString file_path);

  /// @brief The signatures were counted.
  void signaturesCounted(int sig_count);

  /// @brief Current screen DPI was changed.
  void screenDpiChanged();

  void errorOpenFile(const QString& err_string);

private:
  /// @brief find all signatures
  void processSignatures();

  /// @brief delete all files scheduled for deletion
  void processFileDelete();

  fz_context *fzctx_ = nullptr;
  QString file_source_;
  fz_document *fzdoc_ = nullptr;
  pdf_document *pdfdoc_ = nullptr;
  int page_count_ = 0;
  bool process_signatures_ = false;
  bool process_file_delete_ = false;
  qreal physical_screen_dpi_ = 72;// default MuPDF DPI
  std::vector<QString> tmp_files_to_delete_;
};

#endif // pdf_doc_model_HPP

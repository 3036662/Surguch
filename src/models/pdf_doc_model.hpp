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

  /**
   * @brief setSource open new pdf file
   * @param path
   */
  Q_INVOKABLE void setSource(const QString &path);

  /**
   * @brief get current source path
   * @return string
   */
  Q_INVOKABLE QString getSource() const;

  /**
   * @brief resert the whole model
   */
  Q_INVOKABLE void redrawAll();

  /**
   *  @details There is no need to read signatures when the model is utilized to
   * render pdf previews.
   */
  Q_PROPERTY(bool mustProcessSignatures READ mustProcessSignatures WRITE
                 setMustProcessSignatures);
  bool mustProcessSignatures() const { return process_signatures_; }
  void setMustProcessSignatures(bool val) { process_signatures_ = val; }

  Q_PROPERTY(bool mustDeleteTmpFiles MEMBER process_file_delete_)

  Q_INVOKABLE bool saveCurrSourceTo(const QString path,
                                    bool delete_curr_source);

  Q_INVOKABLE void deleteFileLater(QString path);

  Q_INVOKABLE void showInFolder();

  Q_PROPERTY(
      qreal screenDpi MEMBER physical_screen_dpi_ NOTIFY screenDpiChanged)

  [[nodiscard]] Q_INVOKABLE fz_document *getDoc() const;

  [[nodiscard]] Q_INVOKABLE fz_context *getCtx() const;

  [[nodiscard]] Q_INVOKABLE pdf_document *getPdfDoc() const;

signals:
  void signaturesFound(std::vector<core::RawSignature> sigs, QString file_path);
  void signaturesCounted(int sig_count);
  void screenDpiChanged();

private:
  void processSignatures();

  void processFileDelete();

  fz_context *fzctx_ = nullptr;
  QString file_source_;
  fz_document *fzdoc_ = nullptr;
  pdf_document *pdfdoc_ = nullptr;
  int page_count_ = 0;
  bool process_signatures_ = false;
  bool process_file_delete_ = false;

  // std::unique_ptr<QSizeF>  physical_screen_size_;
  qreal physical_screen_dpi_;

  std::vector<QString> tmp_files_to_delete_;
};

#endif // pdf_doc_model_HPP

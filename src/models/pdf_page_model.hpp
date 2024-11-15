#ifndef PDF_PAGE_MODEL_HPP
#define PDF_PAGE_MODEL_HPP

#include "core/raw_signature.hpp"
#include "mupdf/fitz.h"
#include "mupdf/pdf.h"
#include <QAbstractListModel>

class PdfPageModel : public QAbstractListModel {
  Q_OBJECT

public:
  explicit PdfPageModel(QObject *parent = nullptr);

  PdfPageModel(const PdfPageModel &) = delete;
  PdfPageModel(PdfPageModel &&) = delete;
  PdfPageModel &operator=(const PdfPageModel &) = delete;
  PdfPageModel &operator=(PdfPageModel &&) = delete;
  ~PdfPageModel() override;

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

  [[nodiscard]] Q_INVOKABLE fz_document *getDoc() const;

  [[nodiscard]] Q_INVOKABLE fz_context *getCtx() const;

  [[nodiscard]] Q_INVOKABLE pdf_document *getPdfDoc() const;

signals:
  void signaturesFound(std::vector<core::RawSignature> sigs, QString file_path);
  void signaturesCounted(int sig_count);

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

  std::vector<QString> tmp_files_to_delete_;
};

#endif // PDF_PAGE_MODEL_HPP

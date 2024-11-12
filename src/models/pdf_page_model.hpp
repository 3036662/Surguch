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

  // Header:
  [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation,
                                    int role) const override;

  // Basic functionality:
  [[nodiscard]] int rowCount(const QModelIndex &parent) const override;

  [[nodiscard]] QVariant data(const QModelIndex &index,
                              int role) const override;

  Q_INVOKABLE void setSource(const QString &path);

  Q_INVOKABLE QString getSource() const;

  [[nodiscard]] Q_INVOKABLE fz_document *getDoc() const;

  [[nodiscard]] Q_INVOKABLE fz_context *getCtx() const;

  [[nodiscard]] Q_INVOKABLE pdf_document *getPdfDoc() const;

  Q_INVOKABLE void redrawAll();

  Q_PROPERTY(bool mustProcessSignatures READ mustProcessSignatures WRITE
                 setMustProcessSignatures);
  bool mustProcessSignatures() const { return process_signatures_; }
  void setMustProcessSignatures(bool val) { process_signatures_ = val; }

signals:
  void signaturesFound(std::vector<core::RawSignature> sigs, QString file_path);
  void signaturesCounted(int sig_count);

private:
  void processSignatures();

  fz_context *fzctx_ = nullptr;
  QString file_source_;
  fz_document *fzdoc_ = nullptr;
  pdf_document *pdfdoc_ = nullptr;
  int page_count_ = 0;
  bool process_signatures_ = false;
};

#endif // PDF_PAGE_MODEL_HPP

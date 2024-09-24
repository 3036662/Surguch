#ifndef PDF_PAGE_MODEL_HPP
#define PDF_PAGE_MODEL_HPP

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

  [[nodiscard]] Q_INVOKABLE fz_document *getDoc() const;

  [[nodiscard]] Q_INVOKABLE fz_context *getCtx() const;

  Q_INVOKABLE void redrawAll();

private:
  fz_context *fzctx_ = nullptr;
  QString file_source_;
  fz_document *fzdoc_ = nullptr;
  pdf_document *pdfdoc_ = nullptr;
  int page_count_ = 0;
};

#endif // PDF_PAGE_MODEL_HPP

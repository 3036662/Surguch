#ifndef PDF_PAGE_MODEL_HPP
#define PDF_PAGE_MODEL_HPP

#include "mupdf/fitz.h"
#include <QAbstractListModel>

class PdfPageModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit PdfPageModel(QObject *parent = nullptr);
    ~PdfPageModel();

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    Q_INVOKABLE void setSource(const QString& path);

    Q_INVOKABLE fz_document* getDoc() const;

    Q_INVOKABLE fz_context* getCtx() const;
private:
    fz_context* fzctx_=nullptr;
    QString file_source_;
    fz_document* fzdoc_=nullptr;
    int page_count_=0;

};

#endif // PDF_PAGE_MODEL_HPP

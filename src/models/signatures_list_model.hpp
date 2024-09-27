#ifndef SIGNATURES_LIST_MODEL_HPP
#define SIGNATURES_LIST_MODEL_HPP

#include "core/raw_signature.hpp"

#include <QAbstractListModel>
#include <memory>
#include <QThread>

class SignaturesListModel : public QAbstractListModel
{
    Q_OBJECT

    enum RoleNames{
        SigInfoRole = Qt::UserRole,
        CheckStatusRole =Qt::UserRole+1,
        ValidRole= Qt::UserRole+2,
        EmptyRole = Qt::UserRole+3
    };


public:
    explicit SignaturesListModel(QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    [[nodiscard]] int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    [[nodiscard]] QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QHash<int, QByteArray> roleNames() const override;

public slots:

    void updateSigList(std::vector<core::RawSignature> sigs);


private:
    QHash<int, QByteArray> role_names_;
    std::vector<core::RawSignature> raw_signatures_;
    std::unique_ptr<QThread> worker_thread_;
};

#endif // SIGNATURES_LIST_MODEL_HPP

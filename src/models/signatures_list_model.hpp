#ifndef SIGNATURES_LIST_MODEL_HPP
#define SIGNATURES_LIST_MODEL_HPP

#include "core/raw_signature.hpp"
#include "core/signatures_validator.hpp"

#include <QAbstractListModel>
#include <QThread>
#include <memory>

class SignaturesListModel : public QAbstractListModel {
  Q_OBJECT

  enum RoleNames {
    SigInfoRole = Qt::UserRole,
    CheckStatusRole = Qt::UserRole + 1,
    ValidRole = Qt::UserRole + 2,
    EmptyRole = Qt::UserRole + 3,
    SigData = Qt::UserRole + 4
  };

public:
  explicit SignaturesListModel(QObject *parent = nullptr);

  QVariant headerData(int section, Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const override;

  [[nodiscard]] int
  rowCount(const QModelIndex &parent) const override;

  [[nodiscard]] QVariant data(const QModelIndex &index,
                              int role) const override;

  QHash<int, QByteArray> roleNames() const override;

public slots:

  /// @brief store RawSignatures, send all signatures to validation
  void updateSigList(std::vector<core::RawSignature> sigs, QString file_source);

  void
  saveValidationResult(std::shared_ptr<core::CSPResponse> validation_result,
                       size_t ind);

private:
  QHash<int, QByteArray> role_names_;
  std::vector<core::RawSignature> raw_signatures_;
  QThread *worker_thread_ = nullptr;
  core::SignaturesValidator *validator_ = nullptr;
  std::map<size_t, std::shared_ptr<core::CSPResponse>> validation_results_;
};

#endif // SIGNATURES_LIST_MODEL_HPP

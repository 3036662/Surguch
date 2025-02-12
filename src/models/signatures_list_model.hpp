/* File: signatures_list_model.hpp
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

#ifndef SIGNATURES_LIST_MODEL_HPP
#define SIGNATURES_LIST_MODEL_HPP

#include <QAbstractListModel>
#include <QThread>
#include <memory>

#include "core/file_recover_worker.hpp"
#include "core/raw_signature.hpp"
#include "core/signatures_validator.hpp"

/**
 * @brief Model for list of signatures
 *
 */
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

    [[nodiscard]] QVariant headerData(int /*section*/,
                                      Qt::Orientation /*orientation*/,
                                      int /*role*/) const override;

    [[nodiscard]] int rowCount(const QModelIndex &parent) const override;

    [[nodiscard]] QVariant data(const QModelIndex &index,
                                int role) const override;

    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    /// @brief recover the document to state when it signed by signature
    Q_INVOKABLE void recoverDoc(qint64 sig_index);

   signals:

    /// @brief send common document status to QML
    void commonDocStatus(QString common_coverage_status);

    // TODO(Oleg) Passing the enum works perfect in qt6.8 but does not work in qt6.4
    //void commonDocStatus(core::DocStatusEnum::CommonDocCoverageStatus);

    /// @brief file was recovered
    void fileRecovered(QString path);
    /// @brief Validation error happend
    void validationFailedForSignature(size_t index);

   public slots:

    /// @brief store RawSignatures, send all signatures to validation
    void updateSigList(std::vector<core::RawSignature> sigs,
                       const QString &file_source);

    /// @brief Save validation result for one signature
    void saveValidationResult(
        std::shared_ptr<core::ValidationResult> validation_result, size_t ind);

   private:
    QHash<int, QByteArray> role_names_;
    std::vector<core::RawSignature> raw_signatures_;
    std::vector<std::unique_ptr<QThread>> worker_threads_;
    std::vector<std::unique_ptr<core::SignaturesValidator>> validators_;
    size_t curr_worker_index_ = 0;
    QThread *recover_thread_ = nullptr;
    core::FileRecoverWorker *recover_worker_ = nullptr;
    std::map<size_t, std::shared_ptr<core::ValidationResult>>
        validation_results_;
};

#endif  // SIGNATURES_LIST_MODEL_HPP

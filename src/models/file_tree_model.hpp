/* File: file_tree_model.hpp
Copyright (C) Basealt LLC,  2025
Author: Daniil-Viktor Ratkin, <ratkinda@basealt.ru>

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

#ifndef FILE_TREE_MODEL_HPP
#define FILE_TREE_MODEL_HPP

#include <QAbstractItemModel>
#include <QFuture>
#include <QFutureWatcher>
#include <QJsonArray>
#include <QVariant>

#ifndef MOC_TREE_FOR_TESTS
#include <doc_archive_public.hpp>
#else
#include "test/mocked_tree.hpp"
#endif

#include "tree_item.hpp"
#include "tree_sign_helper.hpp"
#include "validation_result.hpp"

/// @brief model for  file tree
class FileTreeModel : public QAbstractItemModel {
    Q_OBJECT
    Q_PROPERTY(bool isDraft READ isDraft NOTIFY isDraftChanged)

    enum Operation { Add, Delete, Wasted };

    enum State { Done, RunningDraft, RunningSigns };

    enum Roles {
        FileNameRole = Qt::UserRole + 1,
        SizeRole,
        LastEditRole,
        SigStatusRole,
        SigColorRole,
        MrpaStatusRole,
        MrpaColorRole,
        HasKidsRole,
        DescriptionRole,
        TypeRole,
        FullPathRole,
        UidRole,
        IdRole,
        RefsNumberRole,
        RefsListRole,
        MrpaNumberRole,
        MrpaListRole,
        SigCheckResultRole,
        EncryptedRole,
    };

    struct OperationData {
        Operation operation = Wasted;
        std::optional<int> row;
        std::optional<QUuid> file_uid;
        std::optional<int> file_id;
    };

   public:
    explicit FileTreeModel(QObject *parent = nullptr);

    using TreeFuture = QFuture<std::optional<std::string>>;
    using TreeFutureWatcher = QFutureWatcher<std::optional<std::string>>;
    using SignFuture = QFuture<bool>;
    using SignFutureWatcher = QFutureWatcher<bool>;

    [[nodiscard]] QVariant data(const QModelIndex &index,
                                int role) const override;
    [[nodiscard]] Qt::ItemFlags flags(const QModelIndex &index) const override;
    [[nodiscard]] QModelIndex index(
        int row, int column, const QModelIndex &parent = {}) const override;
    [[nodiscard]] QModelIndex parent(const QModelIndex &index) const override;
    [[nodiscard]] int rowCount(const QModelIndex &parent = {}) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;
    [[nodiscard]] int columnCount(
        const QModelIndex &parent = {}) const override;

    /// @brief check if model is in draft mode
    [[nodiscard]] bool isDraft() const;

    /// @brief send list of certificates to GUI
    Q_INVOKABLE void getCertList(int file_id);
    /// @brief get JSON with data about MRPA
    Q_INVOKABLE QJsonArray getMrpaData(int node_id);
    /// @brief add node to file tree
    Q_INVOKABLE bool addNode(const QVariantList &list);
    /// @brief delete node from tree
    Q_INVOKABLE bool deleteNode(const QString &full_path, int row, QUuid uid,
                                int id);
    /// @brief delete all nodes from tree and drop context
    Q_INVOKABLE void deleteTree();
    /// @brief sign all files in with parameters from profile
    Q_INVOKABLE void signTree(const QVariantMap &qvparams);

   signals:
    /// @brief emitted when tree state changes
    void isDraftChanged();

    /// @brief emitted when checking signature done
    void signatureReady(
        std::vector<std::shared_ptr<core::ValidationResult>> validation_result);

    /// @brief emit number of signatures for sigList panel
    void updateSigCount(int count);

    /// @brief emitted when signing all files done
    void signDone(QVariant sign_result, bool sign_done);

    /// @brief emitted after tree cleared to drop expanded states of items in
    /// tree
    void dropState();

   private:
    /// @brief recurisve parse all data from libmrpa to build tree
    void setupModelData(const QJsonArray &doc, TreeItem *parent);
    /// @brief correctly process added node
    void processAdd(const QJsonArray &arr);
    /// @brief correctly process node deletion
    void processDelete(const QJsonArray &arr);
    /// @brief process data of draft tree
    void processDraftTree();
    /// @brief process data of checked tree
    void processSignedTree();
    /// @brief process singature check result
    void processSignResult();
    /// @brief add files into UI while context busy
    void addFilesUI(const QStringList &file_list);
    /// @brief delete files from UI while context busy
    void deleteFilesUI(int row, QUuid uid, int id);

    /// @brief parse data to select icons/tooltips in UI for items
    void parseChecksResults(int id);

    void parseMrpaCheckResults(std::shared_ptr<TreeItem> &item);
    void parseSignatureCheckResults(std::shared_ptr<TreeItem> &item);
    void parseFileCheckResults(std::shared_ptr<TreeItem> &item);

#ifndef MOC_TREE_FOR_TESTS
    pdfcsp::DocTree tree_;
#else
    MocedTree tree_;
#endif

    core::TreeSignHelper sign_helper_;
    std::unique_ptr<TreeFuture> tree_future_;
    std::unique_ptr<TreeFutureWatcher> tree_watcher_;
    std::unique_ptr<SignFuture> sign_future_;
    std::unique_ptr<SignFutureWatcher> sign_watcher_;

    /// map to hold added/deleted files we cant process while context busy
    std::unordered_map<QString, OperationData> operation_data_;

    std::map<int, std::weak_ptr<TreeItem>> item_map;
    std::shared_ptr<TreeItem> root_item;
    QHash<int, QByteArray> role_names_;

    State state_ = Done;
    bool ctx_available_ = true;
    bool is_draft_ = true;

#ifdef MOC_TREE_FOR_TESTS
    friend class TTree;
#endif
};

#endif  // FILE_TREE_MODEL_HPP

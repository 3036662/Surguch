#ifndef FILE_TREE_MODEL_HPP
#define FILE_TREE_MODEL_HPP

#include <QAbstractItemModel>
#include <QFuture>
#include <QFutureWatcher>
#include <QJsonArray>
#include <QVariant>
#include <doc_archive_public.hpp>

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
    Q_INVOKABLE void getCertList(int fie_id);
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
        std::vector<std::shared_ptr<core::ValidationResult>> validation_result,
        std::vector<size_t> ind);

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
    void processChecks(int id);

    pdfcsp::DocTree tree_;
    core::TreeSignHelper sign_helper_;
    std::unique_ptr<TreeFuture> tree_future_;
    std::unique_ptr<TreeFutureWatcher> tree_watcher_;
    std::unique_ptr<SignFuture> sign_future_;
    std::unique_ptr<SignFutureWatcher> sign_watcher_;

    std::unordered_map<QString, OperationData> operation_data_;

    std::map<int, std::weak_ptr<TreeItem>> item_map;
    std::shared_ptr<TreeItem> root_item;
    QHash<int, QByteArray> role_names_;

    State state_ = Done;
    bool ctx_available_ = true;
    bool is_draft_ = true;
};

#endif  // FILE_TREE_MODEL_HPP

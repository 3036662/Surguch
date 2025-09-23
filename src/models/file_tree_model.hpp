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

    [[nodiscard]] bool isDraft() const;

    Q_INVOKABLE void getCertList(int fie_id);
    Q_INVOKABLE QJsonArray getMrpaData(int node_id);
    Q_INVOKABLE bool addNode(const QVariantList &list);
    Q_INVOKABLE bool deleteNode(const QString &full_path, int row, QUuid uid,
                                int id);
    Q_INVOKABLE void deleteTree();
    Q_INVOKABLE void signTree(const QVariantMap &qvparams);

   signals:
    void isDraftChanged();

    void signatureReady(
        std::vector<std::shared_ptr<core::ValidationResult>> validation_result,
        std::vector<size_t> ind);

    void updateSigCount(int count);

    void signDone(QVariant sign_result);

   private:
    void setupModelData(const QJsonArray &doc, TreeItem *parent);
    void processAdd(const QJsonArray &arr);
    void processDelete(const QJsonArray &arr);
    void processDraftTree();
    void processSignedTree();
    void processSignResult();
    void addFilesUI(const QStringList &file_list);
    void deleteFilesUI(int row, QUuid uid, int id);

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

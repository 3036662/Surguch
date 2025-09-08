#ifndef FILE_TREE_MODEL_HPP
#define FILE_TREE_MODEL_HPP

#include <QAbstractItemModel>
#include <QFuture>
#include <QFutureWatcher>
#include <QJsonArray>
#include <QVariant>
#include <doc_archive_public.hpp>

#include "tree_item.hpp"

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

    [[nodiscard]] QVariant data(const QModelIndex &index,
                                int role) const override;
    [[nodiscard]] Qt::ItemFlags flags(const QModelIndex &index) const override;
    [[nodiscard]] QModelIndex index(
        int row, int column, const QModelIndex &parent = {}) const override;
    [[nodiscard]] QModelIndex parent(const QModelIndex &index) const override;
    [[nodiscard]] int rowCount(const QModelIndex &parent = {}) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const;
    [[nodiscard]] int columnCount(
        const QModelIndex &parent = {}) const override;

    [[nodiscard]] bool isDraft() const;

    Q_INVOKABLE std::vector<int> getCertList(int fie_id);
    Q_INVOKABLE bool addNode(const QVariantList& list);
    Q_INVOKABLE bool deleteNode(const QString &full_path, int row, QUuid uid,
                                int id);
    Q_INVOKABLE void deleteTree();

    signals:
    void isDraftChanged();

   private:
    void setupModelData(const QJsonArray &doc, TreeItem *parent);
    void processAdd(const QJsonArray &arr);
    void processDelete(const QJsonArray &arr);
    void processDraftTree();
    void processSignedTree();
    void addFilesUI(const QStringList &file_list);
    void deleteFilesUI(int row, QUuid uid, int id);

    pdfcsp::DocTree tree_;
    std::unique_ptr<TreeFuture> tree_future_;
    std::unique_ptr<TreeFutureWatcher> tree_watcher_;

    std::unordered_map<QString, OperationData> operation_data_;

    std::map<int, std::weak_ptr<TreeItem>> item_map;
    std::shared_ptr<TreeItem> root_item;
    QHash<int, QByteArray> role_names_;

    State state_ = Done;
    bool ctx_available_ = true;
    bool is_draft_ = true;
};

#endif  // FILE_TREE_MODEL_HPP

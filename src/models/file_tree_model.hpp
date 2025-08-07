#ifndef FILE_TREE_MODEL_HPP
#define FILE_TREE_MODEL_HPP

#include <QAbstractItemModel>
#include <QVariant>

#include "tree_item.hpp"

class FileTreeModel : public QAbstractItemModel {
    Q_OBJECT

    enum Roles {
        FileNameRole = Qt::UserRole + 1,
        SizeRole,
        LastEditRole,
        SigStatusRole,
        HasKidsRole,
        DescriptionRole,
        TypeRole,
        UidRole,
        IdRole,
        RefsNumberRole,
        RefsListRole,
        MrpaNumberRole,
        MrpaListRole
    };

   public:
    explicit FileTreeModel(QObject *parent = nullptr);

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

    Q_INVOKABLE std::vector<int> getCertList(int fie_id);

    Q_INVOKABLE bool addNode(const QStringList& fie_list);

    Q_INVOKABLE bool deleteNode(int row, QUuid uid);

   private:
    void setupModelData(const QJsonArray &doc, TreeItem *parent);

    std::map<int, std::weak_ptr<TreeItem>> item_map;
    std::shared_ptr<TreeItem> root_item;
    QHash<int, QByteArray> role_names_;

    bool ctx_available_ = true;
};

#endif  // FILE_TREE_MODEL_HPP

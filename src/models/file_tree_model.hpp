#ifndef FILE_TREE_MODEL_HPP
#define FILE_TREE_MODEL_HPP

#include <QAbstractItemModel>
#include <QVariant>

#include "tree_item.hpp"

class FileTreeModel: public QAbstractItemModel {
    Q_OBJECT

    enum Roles {
        FileNameRole = Qt::UserRole + 1,
        SizeRole,
        LastEditRole,
        StatusRole,
        HasKidsRole,
        DescriptionRole,
        IdRole
    };

    public:
    explicit FileTreeModel(QObject *parent = nullptr);

    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;
    [[nodiscard]] Qt::ItemFlags flags(const QModelIndex &index) const override;
    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    [[nodiscard]] QModelIndex index(int row, int column,
                      const QModelIndex &parent = {}) const override;
    [[nodiscard]] QModelIndex parent(const QModelIndex &index) const override;
    [[nodiscard]] int rowCount(const QModelIndex &parent = {}) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames()const;
    [[nodiscard]] int columnCount(const QModelIndex &parent = {}) const override;

private:
    static void setupModelData(const QJsonArray &doc, TreeItem *parent);

    std::unique_ptr<TreeItem> root_item;
    QHash<int, QByteArray> role_names_;
};



#endif //FILE_TREE_MODEL_HPP

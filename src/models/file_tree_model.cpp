#include "file_tree_model.hpp"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStack>
#include <QStringList>

FileTreeModel::FileTreeModel(QObject *parent)
    : QAbstractItemModel(parent)
    , root_item(std::make_unique<TreeItem>(QVariantList{tr("Title")}))
{
    QFile file("/home/dv/tree.json");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    const QByteArray file_data = file.readAll();
    file.close();
    const QJsonDocument json_doc = QJsonDocument::fromJson(file_data);
    if (json_doc.isObject()) {
        QVariantList itemData;
        if (json_doc["Type"].toString() == "Root") {
            itemData << json_doc["Type"].toString();
            itemData << json_doc["id"].toInt();
        }
        QJsonArray const data_ = json_doc["children"].toArray();
        setupModelData(data_, root_item.get());
        file.close();
        qWarning() << "[DEBUG]" << " Positive reading json";
    }
}

int FileTreeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
    return root_item->columnCount();
}

QVariant FileTreeModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    const TreeItem *item = static_cast<const TreeItem*>(index.internalPointer());

    switch (role) {
        case Qt::DisplayRole:
            return item->data(0);//return item->data(index.column());
        case FileNameRole:
            return item->data(0);
        case SizeRole:
            return item->data(1);
        case LastEditRole:
            return item->data(2);
        case TypeRole:
            return item->data(3);
        case SigStatusRole:
            return item->data(4);
        case HasKidsRole:
            return item->childCount() > 0;
        case DescriptionRole:
            return item->description();
        case IdRole:
            return item->id();
        default:
            return QVariant();
    }
}

Qt::ItemFlags FileTreeModel::flags(const QModelIndex &index) const
{
    return index.isValid()
               ? QAbstractItemModel::flags(index) : Qt::ItemFlags(Qt::NoItemFlags);
}

QVariant FileTreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    return orientation == Qt::Horizontal && role == Qt::DisplayRole
               ? root_item->data(section) : QVariant{};
}

QModelIndex FileTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return {};

    TreeItem *parentItem = parent.isValid()
                               ? static_cast<TreeItem*>(parent.internalPointer())
                               : root_item.get();

    if (auto *childItem = parentItem->child(row))
        return createIndex(row, column, childItem);
    return {};
}

QModelIndex FileTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return {};

    auto *childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem *parentItem = childItem->parentItem();

    return parentItem != root_item.get()
               ? createIndex(parentItem->row(), 0, parentItem) : QModelIndex{};
}

int FileTreeModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0)
        return 0;

    const TreeItem *parentItem = parent.isValid()
                                     ? static_cast<const TreeItem*>(parent.internalPointer())
                                     : root_item.get();

    return parentItem->childCount();
}

QHash<int, QByteArray> FileTreeModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[FileNameRole] = "name";
    roles[SizeRole] = "size";
    roles[LastEditRole] = "modification_time";
    roles[SigStatusRole] = "sig_status";
    roles[DescriptionRole] = "description";
    roles[TypeRole] = "type";
    roles[IdRole] = "id";
    return roles;
}

void FileTreeModel::setupModelData(const QJsonArray &array, TreeItem *parent) {
    for (const QJsonValue &value : array) {
        if (!value.isObject()) continue;

        QJsonObject const obj = value.toObject();
        QJsonObject const statArray = obj["stat"].toObject();

        QVariantList itemData;
        itemData << statArray["name"].toString();
        itemData << statArray["size"].toInt();
        itemData << QLocale().toString(QDateTime::fromSecsSinceEpoch(statArray["modification_time"].toInt()), "yyyy.MM.dd hh:mm");
        itemData << obj["type"].toString();
        if (obj.contains("has_check_result")) {
            itemData << obj["has_check_result"].toBool();
        }
        itemData << obj["id"].toInt();

        TreeItem *newItem = new TreeItem(itemData, parent);
        parent->appendChild(std::unique_ptr<TreeItem>(newItem));

        if (obj.contains("description")) {
            newItem->setDescription(obj["description"].toVariant());
        }

        if (obj.contains("children") && obj["children"].isArray()) {
            setupModelData(obj["children"].toArray(), newItem);
        }
    }
}

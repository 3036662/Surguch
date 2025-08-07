#include "file_tree_model.hpp"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStack>
#include <QStringList>

FileTreeModel::FileTreeModel(QObject *parent)
    : QAbstractItemModel(parent),
      root_item(std::make_unique<TreeItem>(FileData(), QUuid())) {}

int FileTreeModel::columnCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return static_cast<TreeItem *>(parent.internalPointer())->columnCount();
    return root_item->columnCount();
}

QVariant FileTreeModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) return QVariant();

    const TreeItem *item =
        static_cast<const TreeItem *>(index.internalPointer());

    switch (role) {
        case FileNameRole:
            return item->data().name;
        case SizeRole:
            return item->data().size;
        case LastEditRole:
            return QLocale().toString(
                QDateTime::fromSecsSinceEpoch(
                    item->data().last_modified),
                "yyyy.MM.dd hh:mm");
        case TypeRole:
            return item->data().type;
        case SigStatusRole:
            return item->data().has_check_result;
        case UidRole:
            return item->uid();
        case IdRole:
            return item->data().id;
        case RefsNumberRole:
            return item->data().ref_id_size;
        // case RefsListRole:
        //     return item->data().ref_ids;
        // case MrpaNumberRole:
        //     return item->data(8);
        // case MrpaListRole:
        //     return item->data(8);
        case HasKidsRole:
            return item->childCount() > 0;
        default:
            return QVariant();
    }
}

Qt::ItemFlags FileTreeModel::flags(const QModelIndex &index) const {
    return index.isValid() ? QAbstractItemModel::flags(index)
                           : Qt::ItemFlags(Qt::NoItemFlags);
}

QModelIndex FileTreeModel::index(int row, int column,
                                 const QModelIndex &parent) const {
    if (!hasIndex(row, column, parent)) return {};

    TreeItem *parentItem =
        parent.isValid() ? static_cast<TreeItem *>(parent.internalPointer())
                         : root_item.get();

    if (auto *childItem = parentItem->child(row))
        return createIndex(row, column, childItem);
    return {};
}

QModelIndex FileTreeModel::parent(const QModelIndex &index) const {
    if (!index.isValid()) return {};

    auto *childItem = static_cast<TreeItem *>(index.internalPointer());
    TreeItem *parentItem = childItem->parentItem();

    return parentItem != root_item.get()
               ? createIndex(parentItem->row(), 0, parentItem)
               : QModelIndex{};
}

int FileTreeModel::rowCount(const QModelIndex &parent) const {
    if (parent.column() > 0) return 0;

    const TreeItem *parentItem =
        parent.isValid()
            ? static_cast<const TreeItem *>(parent.internalPointer())
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
    roles[UidRole] = "uid";
    roles[IdRole] = "id";
    roles[RefsNumberRole] = "refs_num";
    roles[RefsListRole] = "refs_list";
    roles[MrpaNumberRole] = "mrpa_num";
    roles[MrpaListRole] = "mrpa_list";
    return roles;
}

std::vector<int> FileTreeModel::getCertList(int file_id) {
    if (!item_map.at(file_id).expired()) {
        return item_map[file_id].lock()->data().ref_ids;
    }
    return {};
}

bool FileTreeModel::addNode(const QStringList& file_list) {
    qWarning() << "[DEBUG] " << "FileTreeModel::addNode()" << file_list;
    if (!file_list.isEmpty()) {
        if (ctx_available_) {
            QJsonArray file_array;
            std::for_each(file_list.begin(), file_list.end(),[&file_array](const QString& file_name) {
                file_array.append(QUrl(file_name).toLocalFile());
            });

            // waiting Oleg
            QFile file("/home/dv/tree3.json");
            file.open(QIODevice::ReadOnly | QIODevice::Text);
            const QByteArray file_data = file.readAll();
            file.close();
            const QJsonDocument json_doc = QJsonDocument::fromJson(file_data);
            // waiting Oleg

            if (json_doc.isObject()) {
                beginInsertRows(QModelIndex(), root_item->childCount(),
                            root_item->childCount());
                QJsonArray const data_ = json_doc["children"].toArray();
                setupModelData(data_, root_item.get());
                file.close();
                qWarning() << "[DEBUG]" << " Positive reading json";
                endInsertRows();
            }
            ctx_available_ = false;
            return true;
        } else {
            QJsonArray data_;
            std::for_each(file_list.begin(), file_list.end(),[&data_](const QString& file) {
                QJsonObject obj;
                obj["name"] = QUrl(file).fileName();
                obj["id"] = QUuid::createUuid().toString();
                obj["type"] = "temp";
                obj["size"] = 0;
                obj["modification_time"] = 0;
                obj["has_check_result"] = false;
                data_.append(obj);
            });
            beginInsertRows(QModelIndex(), root_item->childCount(),
                            root_item->childCount() + file_list.size() - 1);
            setupModelData(data_, root_item.get());
            qWarning() << "[DEBUG]" << " Positive add file";
            endInsertRows();
        }
    }
    return false;
}

bool FileTreeModel::deleteNode(int row, QUuid id) {
    beginRemoveRows(QModelIndex(), row, row);
    root_item->deleteItem(id);
    endRemoveRows();
    return true;
}

void FileTreeModel::setupModelData(const QJsonArray &array, TreeItem *parent) {
    for (const QJsonValue &value : array) {
        if (!value.isObject()) {
            continue;
        }

        QJsonObject const obj = value.toObject();
        QJsonObject statArray;
        if (obj.contains("stat")) {
            statArray = obj["stat"].toObject();
        }

        FileData fileData;
        if (statArray.contains("name")) {
            fileData.name = statArray["name"].toString();
        } else {
            fileData.name = obj["name"].toString();
        }
        if (statArray.contains("size")) {
            fileData.size = statArray["size"].toInt();
        }
        if (statArray.contains("modification_time")) {
            fileData.last_modified = statArray["modification_time"].toInt();
        }
        if (obj.contains("type")) {
            fileData.type = obj["type"].toString();
        }
        if (obj.contains("has_check_result")) {
            fileData.has_check_result = obj["has_check_result"].toBool();
        } else {
            fileData.has_check_result = false;
        }

        if (obj.contains("assoc_refs_number")) {
            fileData.ref_id_size = obj["assoc_refs_number"].toInt();
        }
        if (obj.contains("refs_ids")) {
            fileData.ref_ids = obj["ref_ids"].toArray().toVariantList();
            // qWarning() << "[DEBUG]" << " ref_ids for "
            //            << statArray["name"].toString() << " : "
            //            << obj["ref_ids"].toArray().toVariantList();
        }
        if (obj.contains("mrpa_ids")) {
            // fileData.mrpa_ids = obj["mrpa_ids"].toArray().toVariantList();
        }

        QUuid const uid(QUuid::createUuid());
        //qWarning() << "[DEBUG]"<< "file = " << itemData[0] << " uid = " << uid.toString();

        auto newItem = std::make_shared<TreeItem>(fileData, uid, parent);
        parent->appendChild(std::shared_ptr<TreeItem>(newItem));

        if (ctx_available_) {
            item_map.emplace(obj["id"].toInt(), newItem);
        }

        if (obj.contains("children") && obj["children"].isArray()) {
            setupModelData(obj["children"].toArray(), newItem.get());
        }
    }
}

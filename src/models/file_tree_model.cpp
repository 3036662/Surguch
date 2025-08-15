#include "file_tree_model.hpp"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStringList>
#include <QtConcurrent>
#include <iostream>
#include <thread>

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
                QDateTime::fromSecsSinceEpoch(item->data().last_modified),
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
        case FullPathRole:
            return item->data().full_path;
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
    roles[FullPathRole] = "full_path";
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

bool FileTreeModel::addNode(QStringList file_list) {
    if (!file_list.isEmpty()) {
        file_list.erase(
            std::remove_if(file_list.begin(), file_list.end(),
                           [this](const QString &file_name) {
                               return root_item->contains(file_name);
                           }),
            file_list.end());
        QJsonArray file_array;
        std::for_each(
            file_list.begin(), file_list.end(),
            [this, &file_array](const QString &file_name) {
                if (!root_item->contains(QUrl(file_name).toLocalFile())) {
                    file_array.append(QUrl(file_name).toLocalFile());
                }
            });
        if (!ctx_available_) {
            std::for_each(
                file_list.begin(), file_list.end(),
                [this](const QString &file_name) {
                    if (operation_data_.count(file_name) > 0 &&
                        operation_data_[file_name].operation == Delete) {
                        operation_data_.erase(file_name);
                        return;
                    }
                    operation_data_[file_name].operation = Add;
                    operation_data_[file_name].row = std::nullopt;
                    operation_data_[file_name].file_uid = std::nullopt;
                    operation_data_[file_name].file_id = std::nullopt;
                });
        }
        processAdd(file_array);
        qWarning() << "[DEBUG] " << "FileTreeModel::addNode()" << file_list;
        return true;
    }
    return false;
}

bool FileTreeModel::deleteNode(const QString &full_path, int row, QUuid uid,
                               int id) {
    if (row >= 0 && id > 0) {
        if (!ctx_available_) {
            if (operation_data_.count(full_path) > 0 &&
                operation_data_[full_path].operation == Add) {
                operation_data_.erase(full_path);
            }
            operation_data_[full_path].operation = Delete;
            operation_data_[full_path].row = row;
            operation_data_[full_path].file_uid = uid;
            operation_data_[full_path].file_id = id;
        }
        QJsonObject obj;
        obj["row"] = row;
        obj["uid"] = uid.toString();
        obj["id"] = id;
        QJsonArray delete_array;
        delete_array.append(obj);
        processDelete(delete_array);
    }
    qWarning() << "[DEBUG] "
               << "FileTreeModel::deleteNode(): " << "Incorrect row or id";
    return false;
}

void FileTreeModel::deleteTree() {
    beginResetModel();
    root_item->deleteChildren();
    tree_.ResetContext();
    endResetModel();
}

void FileTreeModel::setupModelData(const QJsonArray &doc, TreeItem *parent) {
    for (const auto &value : doc) {
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
        if (obj.contains("id")) {
            fileData.id = obj["id"].toInt();
        }
        if (obj.contains("full_path")) {
            fileData.full_path = obj["full_path"].toString();
        }

        if (obj.contains("assoc_refs_number")) {
            fileData.ref_id_size = obj["assoc_refs_number"].toInt();
        }
        if (obj.contains("refs_ids")) {
            // fileData.ref_ids = obj["ref_ids"].toArray().toVariantList();
            //  qWarning() << "[DEBUG]" << " ref_ids for "
            //             << statArray["name"].toString() << " : "
            //             << obj["ref_ids"].toArray().toVariantList();
        }
        if (obj.contains("mrpa_ids")) {
            // fileData.mrpa_ids = obj["mrpa_ids"].toArray().toVariantList();
        }

        QUuid const uid(QUuid::createUuid());
        // qWarning() << "[DEBUG]"<< "file = " << itemData[0] << " uid = " <<
        // uid.toString();

        auto newItem = std::make_shared<TreeItem>(fileData, uid, parent);
        parent->appendChild(std::shared_ptr<TreeItem>(newItem));

        if (ctx_available_) {
            item_map[obj["id"].toInt()] = newItem;
        }

        if (obj.contains("children") && obj["children"].isArray()) {
            setupModelData(obj["children"].toArray(), newItem.get());
        }
    }
}

void FileTreeModel::processAdd(const QJsonArray &arr) {
    switch (state_) {
        case Done: {
            state_ = RunningDraft;
            ctx_available_ = false;
            tree_watcher_ = std::make_unique<TreeFutureWatcher>();
            QObject::connect(tree_watcher_.get(), &TreeFutureWatcher::finished,
                             [this]() { processDraftTree(); });
            tree_future_ = std::make_unique<TreeFuture>(
                QtConcurrent::run([this, f_arr = std::move(arr)]() {
                    return tree_.AddFilesJsonList(
                        QJsonDocument(f_arr).toJson().toStdString());
                }));
            tree_watcher_->setFuture(*tree_future_);
            return;
        }
        case RunningDraft: {
            QStringList file_list;
            for (const auto &item : arr) {
                if (item.isString()) {
                    file_list.append(item.toString());
                }
            }
            addFilesUI(file_list);
            qWarning() << "[DEBUG] " << "FileTreeModel::addNode()"
                       << ": updated UI, queue changes";
            return;
        }
        case RunningSigns: {
            QStringList file_list;
            for (const auto &item : arr) {
                if (item.isString()) {
                    file_list.append(item.toString());
                }
            }
            addFilesUI(file_list);
            qWarning() << "[DEBUG] " << "FileTreeModel::addNode()"
                       << ": updated UI, queue changes";
            return;
        }
    }
}

void FileTreeModel::processDelete(const QJsonArray &arr) {
    switch (state_) {
        case Done: {
            state_ = RunningDraft;
            QJsonArray file_array;
            for (const auto &item : arr) { //todo error here!!!
                if (item.isObject()) {
                    QJsonObject obj = item.toObject();
                    int id = obj["id"].toInt();
                    file_array.append(id);

                }
            }
            ctx_available_ = false;
            tree_watcher_ = std::make_unique<TreeFutureWatcher>();
            QObject::connect(tree_watcher_.get(), &TreeFutureWatcher::finished,
                             [this]() { processDraftTree(); });
            tree_future_ = std::make_unique<TreeFuture>(
                QtConcurrent::run([this, f_arr = std::move(file_array)]() {
                    return tree_.RemoveFilesJsonList(
                        QJsonDocument(f_arr).toJson().toStdString());
                }));
            tree_watcher_->setFuture(*tree_future_);
            qWarning() << "[DEBUG] "
                       << "FileTreeModel::deleteNode(): " << "id not found";
            return;
        }
        case RunningDraft: {
            for (const auto &item : arr) {
                if (item.isObject()) {
                    QJsonObject obj = item.toObject();
                    int id = obj["id"].toInt(-1);
                    int row = obj["row"].toInt(-1);
                    QString uid_str = obj["uid"].toString();
                    deleteFilesUI(row, QUuid(uid_str), id);
                }
            }
            return;
        }
        case RunningSigns: {
            for (const auto &item : arr) {
                if (item.isObject()) {
                    QJsonObject obj = item.toObject();
                    int id = obj["id"].toInt();
                    int row = obj["row"].toInt();
                    QString uid_str = obj["uid"].toString();
                    deleteFilesUI(row, QUuid(uid_str), id);
                }
            }
            return;
        }
    }
}

void FileTreeModel::processDraftTree() {
    if (tree_future_ && tree_future_->isValid()) {
        std::optional<std::string> res = tree_future_->takeResult();
        ctx_available_ = true;
        if (operation_data_.empty()) {
            qWarning() << "[DEBUG]" << " FileTreeModel::processDraftTree(): "
                       << "starting check signs";
            if (res.has_value()) {
                const QJsonDocument json_doc =
                    QJsonDocument::fromJson(res.value().data());
                if (json_doc.isObject()) {
                    QJsonArray const data_ = json_doc["children"].toArray();
                    beginResetModel();
                    root_item->deleteChildren();
                    setupModelData(data_, root_item.get());
                    qWarning() << "[DEBUG]"
                               << " FileTreeModel::processDraftTree(): "
                               << "completed and ready";
                    endResetModel();
                }
            }
            state_ = RunningSigns;
            ctx_available_ = false;
            tree_watcher_ = std::make_unique<TreeFutureWatcher>();
            QObject::connect(tree_watcher_.get(), &TreeFutureWatcher::finished,
                             [this]() { processSignedTree(); });
            tree_future_ = std::make_unique<TreeFuture>(
                QtConcurrent::run([this]() { return tree_.BuildTree(); }));
            tree_watcher_->setFuture(*tree_future_);
            return;
        }
        state_ = Done;
        QJsonArray add_array;
        std::for_each(
            operation_data_.begin(), operation_data_.end(),
            [this, &add_array](const auto &pair) {
                    if (pair.second.operation == Add) {
                        add_array.append(pair.first);
                    }
                }
            );
        std::for_each(add_array.begin(), add_array.end(),[this](const auto &item) {
            if (item.isString()) {
            operation_data_.erase(item.toString());
            }
        });
        if (!add_array.empty()) {
            processAdd(add_array);
        }

        QJsonArray delete_array;
        QJsonArray delete_array_helper;
        std::for_each(
            operation_data_.begin(), operation_data_.end(),
            [this, &delete_array, &delete_array_helper](const auto &pair) {
                    if (pair.second.operation == Delete) {
                        delete_array.append(pair.second.file_id.value());
                        delete_array_helper.append(pair.first);
                    }
                }
            );
        std::for_each(delete_array_helper.begin(), delete_array_helper.end(),[this](const auto &item) {
            if (item.isString()) {
                operation_data_.erase(item.toString());
            }
        });
        if (!delete_array.empty()) {
            processDelete(delete_array);
        }
    }
}

void FileTreeModel::processSignedTree() {
    if (tree_future_ && tree_future_->isValid()) {
        std::optional<std::string> res = tree_future_->takeResult();
        ctx_available_ = true;
        state_ = Done;
        if (operation_data_.empty()) {
            if (res.has_value()) {
                const QJsonDocument json_doc =
                    QJsonDocument::fromJson(res.value().data());
                if (json_doc.isObject()) {
                    QJsonArray const data_ = json_doc["children"].toArray();
                    beginResetModel();
                    root_item->deleteChildren();
                    setupModelData(data_, root_item.get());
                    qWarning() << "[DEBUG]"
                               << " FileTreeModel::processSignedTree(): "
                               << "completed and ready";
                    qWarning() << "[DEBUG]" << "root child count: " << root_item->childCount();
                    endResetModel();
                }
            }
            return;
        }
    }

    QJsonArray add_array;
    std::for_each(
        operation_data_.begin(), operation_data_.end(),
        [this, &add_array](const auto &pair) {
                if (pair.second.operation == Add) {
                    add_array.append(pair.first);
                }
            }
        );
    std::for_each(add_array.begin(), add_array.end(),[this](const auto &item) {
            if (item.isString()) {
            operation_data_.erase(item.toString());
            }
        });
    if (!add_array.empty()) {
        processAdd(add_array);
    }

    QJsonArray delete_array;
    QJsonArray delete_array_helper;
    std::for_each(
        operation_data_.begin(), operation_data_.end(),
        [this, &delete_array, &delete_array_helper](const auto &pair) {
                if (pair.second.operation == Delete) {
                    delete_array.append(pair.second.file_id.value());
                    delete_array_helper.append(pair.first);
                }
            }
        );
    std::for_each(delete_array_helper.begin(), delete_array_helper.end(),[this](const auto &item) {
            if (item.isString()) {
                operation_data_.erase(item.toString());
            }
        });
    if (!delete_array.empty()) {
        processDelete(delete_array);
    }
}

void FileTreeModel::addFilesUI(const QStringList &file_list) {
    QJsonArray data_;
    std::for_each(file_list.begin(), file_list.end(),
                  [&data_](const QJsonValue &file) {
                      QJsonObject obj;
                      obj["name"] = QUrl(file.toString()).fileName();
                      obj["full_path"] = QUrl(file.toString()).toLocalFile();
                      // obj["id"] = QUuid::createUuid().toString();
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

void FileTreeModel::deleteFilesUI(int row, QUuid uid, int id) {
    beginRemoveRows(QModelIndex(), row, row);
    root_item->deleteItem(uid);
    endRemoveRows();
}

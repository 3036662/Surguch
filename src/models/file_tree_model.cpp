#include "file_tree_model.hpp"

#include <qpalette.h>

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
            return item->data().sig_text;
        case SigColorRole:
            return item->data().sig_color;
        case MrpaStatusRole:
            return item->data().mrpa_text;
        case MrpaColorRole:
            return item->data().mrpa_color;
        case UidRole:
            return item->uid();
        case IdRole:
            return item->data().id;
        case RefsNumberRole:
            return item->data().ref_id_size;
        case FullPathRole:
            return item->data().full_path;
        case RefsListRole:
            return QVariant::fromValue(item->data().ref_ids);
        case MrpaNumberRole:
            return item->data().mrpa_id_size;
        case MrpaListRole:
            return QVariant::fromValue(item->data().mrpa_ids);
        case EncryptedRole:
            return item->data().encrypted;
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
    roles[SigColorRole] = "sig_color";
    roles[MrpaStatusRole] = "mrpa_status";
    roles[MrpaColorRole] = "mrpa_color";
    roles[DescriptionRole] = "description";
    roles[FullPathRole] = "full_path";
    roles[TypeRole] = "type";
    roles[UidRole] = "uid";
    roles[IdRole] = "id";
    roles[RefsNumberRole] = "refs_num";
    roles[RefsListRole] = "refs_list";
    roles[MrpaNumberRole] = "mrpa_num";
    roles[MrpaListRole] = "mrpa_list";
    roles[SigCheckResultRole] = "sig_check_result";
    roles[EncryptedRole] = "encrypted";
    return roles;
}

bool FileTreeModel::isDraft() const { return is_draft_; }

void FileTreeModel::getCertList(int file_id) {
    if (item_map.at(file_id).expired()) {
        return;
    }

    std::vector<std::shared_ptr<core::ValidationResult>> res;
    std::vector<size_t> res_ind;
    switch (item_map.at(file_id).lock()->data().type) {
        case File:
            emit updateSigCount(
                item_map.at(file_id).lock()->data().ref_id_size);
            for (size_t i = 0; i < item_map[file_id].lock()->data().ref_id_size;
                 ++i) {
                pdfcsp::c_bridge::CPodResult const *pod =
                    tree_.GetCheckResultForNode(
                        item_map[file_id].lock()->data().ref_ids[i], file_id);
                core::ValidationResult val_res;
                core::createCSPResponse(val_res, pod);
                res.emplace_back(std::move(
                    std::make_shared<core::ValidationResult>(val_res)));
                res_ind.emplace_back(i);
            }
            emit signatureReady(res, res_ind);
            break;
        case Sig:
            emit updateSigCount(
                item_map.at(file_id).lock()->data().ref_id_size);
            for (int i = 0; i < item_map[file_id].lock()->data().ref_id_size;
                 ++i) {
                pdfcsp::c_bridge::CPodResult const *pod =
                    tree_.GetCheckResultForNode(
                        file_id, item_map[file_id].lock()->data().ref_ids[i]);
                core::ValidationResult val_res;
                core::createCSPResponse(val_res, pod);
                res.emplace_back(std::move(
                    std::make_shared<core::ValidationResult>(val_res)));
                res_ind.emplace_back(i);
            }
            emit signatureReady(res, res_ind);
            break;
        case Asig:
            emit updateSigCount(
                item_map.at(file_id).lock()->data().ref_id_size);
            for (int i = 0; i < item_map[file_id].lock()->data().ref_id_size;
                 ++i) {
                pdfcsp::c_bridge::CPodResult const *pod =
                    tree_.GetCheckResultForNode(
                        file_id, item_map[file_id].lock()->data().ref_ids[i]);
                core::ValidationResult val_res;
                core::createCSPResponse(val_res, pod);
                res.emplace_back(std::move(
                    std::make_shared<core::ValidationResult>(val_res)));
                res_ind.emplace_back(i);
            }
            emit signatureReady(res, res_ind);
            break;
        default:
            break;
    }
}

QJsonArray FileTreeModel::getMrpaData(int node_id) {
    QJsonArray arr;
    switch (item_map.at(node_id).lock()->data().type) {
        case Mrpa:
            arr.append(item_map.at(node_id).lock()->data().mrpa_data.value());
            return arr;
            break;
        case Zip:
            if (item_map.at(node_id).lock()->data().mrpa_id_size > 0) {
                for (size_t ind = 0;
                     ind < item_map.at(node_id).lock()->data().mrpa_id_size;
                     ++ind) {
                    arr.append(
                        item_map
                            .at(item_map.at(node_id).lock()->data().mrpa_ids.at(
                                ind))
                            .lock()
                            ->data()
                            .mrpa_data.value());
                }
                return arr;
            }
            break;
        case File:
            if (item_map.at(node_id).lock()->data().mrpa_id_size > 0) {
                for (size_t ind = 0;
                     ind < item_map.at(node_id).lock()->data().mrpa_id_size;
                     ++ind) {
                    arr.append(
                        item_map
                            .at(item_map.at(node_id).lock()->data().mrpa_ids.at(
                                ind))
                            .lock()
                            ->data()
                            .mrpa_data.value());
                }
                return arr;
            }
            break;
        case Sig:
            if (item_map.at(node_id).lock()->data().mrpa_id_size > 0) {
                for (size_t ind = 0;
                     ind < item_map.at(node_id).lock()->data().mrpa_id_size;
                     ++ind) {
                    arr.append(
                        item_map
                            .at(item_map.at(node_id).lock()->data().mrpa_ids.at(
                                ind))
                            .lock()
                            ->data()
                            .mrpa_data.value());
                }
                return arr;
            }
            break;
        case Asig:
            if (item_map.at(node_id).lock()->data().mrpa_id_size > 0) {
                for (size_t ind = 0;
                     ind < item_map.at(node_id).lock()->data().mrpa_id_size;
                     ++ind) {
                    arr.append(
                        item_map
                            .at(item_map.at(node_id).lock()->data().mrpa_ids.at(
                                ind))
                            .lock()
                            ->data()
                            .mrpa_data.value());
                }
                return arr;
            }
            break;
        default:
            return {};
            break;
    }
    return {};
}

bool FileTreeModel::addNode(const QVariantList &list) {
    qWarning() << "FileTreeModel::addNode()" << list.size();
    if (!list.empty()) {
        QStringList file_list;
        std::for_each(
            list.cbegin(), list.cend(), [&file_list](const QVariant &item) {
                qWarning() << "FileTreeModel::addNode()"
                           << item.metaType().name();
                file_list.append(
                    qvariant_cast<QUrl>(item.value<QVariant>()).toString());
            });

        file_list.erase(std::remove_if(file_list.begin(), file_list.end(),
                                       [this](const QString &file_name) {
                                           return root_item->contains(
                                               QUrl(file_name).toLocalFile());
                                       }),
                        file_list.end());
        QJsonArray file_array;
        std::for_each(
            file_list.begin(), file_list.end(),
            [this, &file_array](const QString &file_name) {
                if (!root_item->contains(QUrl(file_name).toLocalFile())) {
                    file_array.append(QUrl(file_name).toLocalFile());
                    qWarning() << "FileTreeModel::addNode()"
                               << QUrl(file_name).toLocalFile();
                }
            });
        if (!ctx_available_) {
            std::for_each(
                file_list.begin(), file_list.end(),
                [this](const QString &file_name) {
                    const QString full_path = QUrl(file_name).toLocalFile();
                    if (operation_data_.count(full_path) > 0 &&
                        operation_data_[full_path].operation == Delete) {
                        operation_data_.erase(full_path);
                        return;
                    }
                    operation_data_[full_path].operation = Add;
                    operation_data_[full_path].row = std::nullopt;
                    operation_data_[full_path].file_uid = std::nullopt;
                    operation_data_[full_path].file_id = std::nullopt;
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
    if (row >= 0) {
        if (!ctx_available_) {
            if (operation_data_.count(full_path) > 0 &&
                operation_data_[full_path].operation == Add) {
                operation_data_.erase(full_path);
            } else {
                operation_data_[full_path].operation = Delete;
                operation_data_[full_path].row = row;
                operation_data_[full_path].file_uid = uid;
                operation_data_[full_path].file_id = id;
            }
        }
        QJsonObject obj;
        obj["row"] = row;
        obj["uid"] = uid.toString();
        obj["id"] = id;
        QJsonArray delete_array;
        delete_array.append(obj);
        processDelete(delete_array);
        return true;
    }
    qWarning() << "[DEBUG] "
               << "FileTreeModel::deleteNode(): " << "Incorrect row or id";
    return false;
}

void FileTreeModel::deleteTree() {
    beginResetModel();
    emit dropState();
    root_item->deleteChildren();
    tree_.ResetContext();
    endResetModel();
}

void FileTreeModel::signTree(const QVariantMap &qvparams) {
    sign_helper_.createSigSettings(qvparams);
    auto setting_wrapper = sign_helper_.createWrapper();
    sign_watcher_ = std::make_unique<SignFutureWatcher>();
    QObject::connect(sign_watcher_.get(), &SignFutureWatcher::finished,
                     [this]() { processSignResult(); });
    sign_future_ = std::make_unique<SignFuture>(
        QtConcurrent::run([this, settings = std::move(setting_wrapper)]() {
            return tree_.SignTree(settings->pod_settings);
        }));
    sign_watcher_->setFuture(*sign_future_);
    return;
}

void FileTreeModel::processSignResult() {
    if (sign_future_ && sign_future_->isValid()) {
        bool res = sign_future_->takeResult();
        auto status = tree_.LastSignStatus();
        if (status.has_value()) {
            if (!res) {
                qWarning() << "[DEBUG] " << res;
            }
            emit signDone(QString(status.value().data()), res);
        }
    }
    return;
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
            QString type = obj["type"].toString();
            if (type == "File") {
                fileData.type = File;
            } else if (type == "Dir") {
                fileData.type = Dir;
            } else if (type == "Zip") {
                fileData.type = Zip;
            } else if (type == "Asig") {
                fileData.type = Asig;
            } else if (type == "Sig") {
                fileData.type = Sig;
            } else if (type == "Mrpa") {
                fileData.type = Mrpa;
            }
        }
        if (statArray.contains("encrypted")) {
            fileData.encrypted = statArray["encrypted"].toBool();
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
        if (obj.contains("ref_ids")) {
            QJsonArray refArray = obj["ref_ids"].toArray();
            if (refArray.size() > 0) {
                std::for_each(refArray.begin(), refArray.end(),
                              [&fileData](const auto &ref_id) {
                                  fileData.ref_ids.emplace_back(ref_id.toInt());
                              });
            }
        }
        if (obj.contains("mrpa_ids")) {
            QJsonArray mrpaRefArray = obj["mrpa_ids"].toArray();
            if (mrpaRefArray.size() > 0) {
                std::for_each(
                    mrpaRefArray.begin(), mrpaRefArray.end(),
                    [&fileData](const auto &mrpa_id) {
                        fileData.mrpa_ids.emplace_back(mrpa_id.toInt());
                    });
                fileData.mrpa_id_size = mrpaRefArray.size();
            }
        }
        if (obj.contains("has_check_result")) {
            fileData.has_check_result = obj["has_check_result"].toBool();
            if (fileData.has_check_result && obj.contains("check_results")) {
                QJsonArray checkArray;
                checkArray = obj["check_results"].toArray();
                std::for_each(
                    checkArray.begin(), checkArray.end(),
                    [&fileData](const auto &item) {
                        if (item.isObject()) {
                            QJsonObject item_obj = item.toObject();
                            if (item_obj.contains("file_id") &&
                                item_obj.contains("check_summary")) {
                                CheckResult check_result;
                                check_result.file_id =
                                    item_obj["file_id"].toInt();
                                check_result.check_summary =
                                    item_obj["check_summary"].toBool();
                                fileData.check_results.emplace_back(
                                    check_result);
                            }
                        }
                    });
            }
        }

        if (fileData.type == Mrpa) {
            if (obj.contains("mrpa_json_repr")) {
                fileData.mrpa_data = obj["mrpa_json_repr"].toObject();
            }
        }

        if (obj.contains("time_valid")) {
            fileData.time_valid = obj["time_valid"].toBool();
        }

        QUuid const uid(QUuid::createUuid());
        auto newItem = std::make_shared<TreeItem>(fileData, uid, parent);
        parent->appendChild(std::shared_ptr<TreeItem>(newItem));

        if (obj.contains("id") && ctx_available_) {
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
            for (const auto &item : arr) {
                if (item.isObject()) {
                    QJsonObject obj = item.toObject();
                    int id = obj["id"].toInt();
                    file_array.append(id);
                }
                if (item.isDouble()) {
                    file_array.append(item.toInt());
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
                    is_draft_ = true;
                    emit isDraftChanged();
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
        std::for_each(operation_data_.begin(), operation_data_.end(),
                      [this, &add_array](const auto &pair) {
                          if (pair.second.operation == Add) {
                              add_array.append(pair.first);
                          }
                      });
        std::for_each(add_array.begin(), add_array.end(),
                      [this](const auto &item) {
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
            });
        std::for_each(delete_array_helper.begin(), delete_array_helper.end(),
                      [this](const auto &item) {
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
                qWarning() << "[DEBUG]" << res.value().data();
                if (json_doc.isObject()) {
                    QJsonArray const data_ = json_doc["children"].toArray();
                    beginResetModel();
                    is_draft_ = false;
                    emit isDraftChanged();
                    root_item->deleteChildren();
                    setupModelData(data_, root_item.get());
                    qWarning() << "[DEBUG]"
                               << " FileTreeModel::processSignedTree(): "
                               << "completed and ready";
                    qWarning()
                        << "[DEBUG]"
                        << "root child count: " << root_item->childCount();
                    std::for_each(item_map.begin(), item_map.end(),
                                  [this](auto &item) {
                                      if (!item.second.expired()) {
                                          processChecks(item.first);
                                      }
                                  });
                    endResetModel();
                }
            }
            return;
        }
    }

    QJsonArray add_array;
    std::for_each(operation_data_.begin(), operation_data_.end(),
                  [this, &add_array](const auto &pair) {
                      if (pair.second.operation == Add) {
                          add_array.append(QUrl(pair.first).toLocalFile());
                      }
                  });
    std::for_each(add_array.begin(), add_array.end(), [this](const auto &item) {
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
        });
    std::for_each(delete_array_helper.begin(), delete_array_helper.end(),
                  [this](const auto &item) {
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
                      obj["full_path"] = file;
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

void FileTreeModel::processChecks(int id) {
    auto item = item_map.at(id).lock();
    switch (item->data().type) {
        case Zip:
            item->setSigStats("nosign", "empty");
            item->setMrpaStats("nomrpa", "empty");
            if (item->data().encrypted) {
                item->setSigStats("lock", "file_red");
                return;
            }
            if (item->data().ref_id_size > 0) {
                const auto checks = item->data().ref_ids;
                int valid = 0;
                int invalid = 0;
                std::for_each(
                    checks.cbegin(), checks.cend(),
                    [&item, &valid, &invalid, this](int ref_id) {
                        if (tree_
                                .GetCheckResultForNode(ref_id, item->data().id)
                                ->bres.check_summary) {
                            ++valid;
                        } else {
                            ++invalid;
                        }
                    });
                if (valid > 0 && invalid == 0) {
                    item->setSigStats(QString::number(valid), "file_green");
                }
                if (valid > 0 && invalid > 0) {
                    item->setSigStats(QString::number(valid), "file_mixed");
                }
                if (valid == 0 && invalid > 0) {
                    item->setSigStats(QString::number(invalid), "file_red");
                }
                if (item->data().mrpa_id_size > 0) {
                    if (item->data().mrpa_id_size == item->data().ref_id_size) {
                        item->setMrpaStats(
                            QString::number(item->data().mrpa_id_size),
                            "file_green");
                        return;
                    }
                    item->setMrpaStats(
                        QString::number(item->data().mrpa_id_size), "empty");
                    return;
                } else {
                    item->setMrpaStats("nomrpa", "file_mixed");
                    return;
                }
            }
            return;
        case Dir:
            item->setSigStats("nosign", "empty");
            item->setMrpaStats("nomrpa", "empty");
            return;
        case Sig:
            item->setSigStats("nosign", "empty");
            item->setMrpaStats("nomrpa", "empty");
            if (item->data().has_check_result.has_value() &&
                item->data().has_check_result.value()) {
                if (item->data().check_results.size() > 0) {
                    int valid = 0;
                    int invalid = 0;
                    auto checks = item->data().check_results;
                    std::for_each(checks.cbegin(), checks.cend(),
                                  [&valid, &invalid](const CheckResult &res) {
                                      if (res.check_summary) {
                                          ++valid;
                                      } else {
                                          ++invalid;
                                      }
                                  });
                    if (valid > 0 && invalid == 0) {
                        item->setSigStats("good", "sig_green");
                    } else if (valid > 0 && invalid > 0) {
                        item->setSigStats("mixed", "sig_red");
                    } else {
                        item->setSigStats("bad", "sig_red");
                    }
                    if (item->data().mrpa_id_size > 0) {
                        if (item->data().mrpa_id_size ==
                            item->data().ref_id_size) {
                            item->setMrpaStats(
                                QString::number(item->data().mrpa_id_size),
                                "file_green");
                            return;
                        }
                        item->setMrpaStats(
                            QString::number(item->data().mrpa_id_size),
                            "empty");
                        return;
                    } else {
                        item->setMrpaStats("nomrpa", "empty");
                        return;
                    }
                }
            }
            item->setSigStats("no_file", "sig_mixed");
            return;
        case Asig:
            item->setSigStats("nosign", "empty");
            item->setMrpaStats("nomrpa", "empty");
            if (item->data().has_check_result.has_value() &&
                item->data().has_check_result.value()) {
                if (item->data().check_results.size() > 0) {
                    int valid = 0;
                    int invalid = 0;
                    auto checks = item->data().check_results;
                    std::for_each(checks.cbegin(), checks.cend(),
                                  [&valid, &invalid](const CheckResult &res) {
                                      if (res.check_summary) {
                                          ++valid;
                                      } else {
                                          ++invalid;
                                      }
                                  });
                    if (valid > 0 && invalid == 0) {
                        item->setSigStats("good", "sig_green");
                    } else if (valid > 0 && invalid > 0) {
                        item->setSigStats("mixed", "sig_mixed");
                    } else {
                        item->setSigStats("bad", "sig_red");
                    }
                    if (item->data().mrpa_id_size > 0) {
                        if (item->data().mrpa_id_size ==
                            item->data().ref_id_size) {
                            item->setMrpaStats(
                                QString::number(item->data().mrpa_id_size),
                                "file_green");
                            return;
                        }
                        item->setMrpaStats(
                            QString::number(item->data().mrpa_id_size),
                            "empty");
                        return;
                    } else {
                        item->setMrpaStats("nomrpa", "empty");
                        return;
                    }
                }
            }
            item->setSigStats("not found", "sig_mixed");
            return;
        case File:
            item->setSigStats("nosign", "empty");
            item->setMrpaStats("nomrpa", "empty");
            if (item->data().encrypted) {
                item->setSigStats("lock", "file_red");
                return;
            }
            if (item->data().ref_id_size > 0) {
                const auto checks = item->data().ref_ids;
                int valid = 0;
                int invalid = 0;
                std::for_each(
                    checks.cbegin(), checks.cend(),
                    [&item, &valid, &invalid, this](int ref_id) {
                        if (tree_
                                .GetCheckResultForNode(ref_id, item->data().id)
                                ->bres.check_summary) {
                            ++valid;
                        } else {
                            ++invalid;
                        }
                    });
                if (valid > 0 && invalid == 0) {
                    item->setSigStats(QString::number(valid), "file_green");
                }
                if (valid > 0 && invalid > 0) {
                    item->setSigStats(QString::number(valid), "file_mixed");
                }
                if (valid == 0 && invalid > 0) {
                    item->setSigStats(QString::number(invalid), "file_red");
                }
                if (item->data().mrpa_id_size > 0) {
                    if (item->data().mrpa_id_size == item->data().ref_id_size) {
                        item->setMrpaStats(
                            QString::number(item->data().mrpa_id_size),
                            "file_green");
                        return;
                    }
                    item->setMrpaStats(
                        QString::number(item->data().mrpa_id_size),
                        "file_mixed");
                    return;
                } else {
                    item->setMrpaStats("nomrpa", "empty");
                    return;
                }
            }
            return;
        case Mrpa:
            item->setSigStats("nosign", "empty");
            item->setMrpaStats("nosign", "invalid");
            if (item->data().ref_id_size > 0) {
                if (item->data().time_valid.has_value() &&
                    !item->data().time_valid.value()) {
                    item->setMrpaStats("old", "old");
                    return;
                }
                item->setMrpaStats("ok", "valid");
                return;
            }
            return;
        default:
            item->setSigStats("nosign", "empty");
            item->setMrpaStats("nomrpa", "empty");
            return;
            ;
    }
}

/* File: file_tree_model.cpp
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

namespace {
/// @brief class for parsing json to objects for model
class TreeJsonParser {
    TreeItem::FileData fileData;
    QJsonObject obj;
    QJsonObject statArray;

   public:
    explicit TreeJsonParser(const QJsonObject &obj) : obj(obj) {
        if (obj.contains("stat")) {
            statArray = obj["stat"].toObject();
        }
    };
    TreeItem::FileData getResult() { return fileData; }

    void setName() {
        if (statArray.contains("name")) {
            fileData.name = statArray["name"].toString();
        } else if (obj.contains("name")) {
            fileData.name = obj["name"].toString();
        }
    }

    void setSize() {
        if (statArray.contains("size")) {
            fileData.size = statArray["size"].toInt();
        }
    }

    void setLastModified() {
        if (statArray.contains("modification_time")) {
            fileData.last_modified = statArray["modification_time"].toInt();
        }
    }

    void setType() {
        if (obj.contains("type")) {
            const QString type = obj["type"].toString();
            if (type == "File") {
                fileData.type = TreeItem::File;
            } else if (type == "Dir") {
                fileData.type = TreeItem::Dir;
            } else if (type == "Zip") {
                fileData.type = TreeItem::Zip;
            } else if (type == "Asig") {
                fileData.type = TreeItem::Asig;
            } else if (type == "Sig") {
                fileData.type = TreeItem::Sig;
            } else if (type == "Mrpa") {
                fileData.type = TreeItem::Mrpa;
            }
        }
    }

    void setEncrypted() {
        if (statArray.contains("encrypted")) {
            fileData.encrypted = statArray["encrypted"].toBool();
        }
    }

    void setId() {
        if (obj.contains("id")) {
            fileData.id = obj["id"].toInt();
        }
    }

    void setFullPath() {
        if (obj.contains("full_path")) {
            fileData.full_path = obj["full_path"].toString();
        }
    }

    void setRefIds() {
        if (obj.contains("assoc_refs_number")) {
            fileData.ref_id_size = obj["assoc_refs_number"].toInt();
        }
        if (obj.contains("ref_ids") && obj["ref_ids"].isArray()) {
            const QJsonArray refArray = obj["ref_ids"].toArray();
            fileData.ref_ids.reserve(fileData.ref_ids.size() + refArray.size());
            std::transform(refArray.cbegin(), refArray.cend(),
                           std::back_inserter(fileData.ref_ids),
                           [](const auto &ref_id) { return ref_id.toInt(); });
        }
    }

    void setMrpaIds() {
        if (obj.contains("mrpa_ids") && obj["mrpa_ids"].isArray()) {
            const QJsonArray mrpaRefArray = obj["mrpa_ids"].toArray();
            fileData.mrpa_ids.reserve(fileData.mrpa_ids.size() +
                                      mrpaRefArray.size());
            std::transform(mrpaRefArray.cbegin(), mrpaRefArray.cend(),
                           std::back_inserter(fileData.mrpa_ids),
                           [](const auto &mrpa_id) { return mrpa_id.toInt(); });
            fileData.mrpa_id_size = static_cast<int>(mrpaRefArray.size());
        }
    }

    static std::vector<TreeItem::CheckResult> setupCheckResults(
        const QJsonObject &obj, bool has_check_result) {
        std::vector<TreeItem::CheckResult> results;
        if (!has_check_result || !obj.contains("check_results") ||
            !obj["check_results"].isArray()) {
            return results;
        }

        const QJsonArray checkArray = obj["check_results"].toArray();
        std::for_each(checkArray.cbegin(), checkArray.cend(),
                      [&results](const QJsonValue &item) {
                          if (!item.isObject()) {
                              return;
                          }
                          const QJsonObject item_obj = item.toObject();
                          if (item_obj.contains("file_id") &&
                              item_obj.contains("check_summary")) {
                              TreeItem::CheckResult check_result;
                              check_result.file_id =
                                  item_obj["file_id"].toInt();
                              check_result.check_summary =
                                  item_obj["check_summary"].toBool();
                              results.emplace_back(check_result);
                          }
                      });
        return results;
    }

    void setCheckResults() {
        if (obj.contains("has_check_result")) {
            fileData.has_check_result = obj["has_check_result"].toBool();
            fileData.check_results =
                setupCheckResults(obj, fileData.has_check_result);
        }
    }

    void setMrpaDataIfMrpa() {
        if (fileData.type == TreeItem::Mrpa && obj.contains("mrpa_json_repr") &&
            obj["mrpa_json_repr"].isObject()) {
            fileData.mrpa_data = obj["mrpa_json_repr"].toObject();
        }
    }

    void setTimeValid() {
        if (obj.contains("time_valid")) {
            fileData.time_valid = obj["time_valid"].toBool();
        }
    }

    TreeItem::FileData parseAll() {
        setName();
        setSize();
        setLastModified();
        setType();
        setEncrypted();
        setId();
        setFullPath();
        setRefIds();
        setMrpaIds();
        setCheckResults();
        setMrpaDataIfMrpa();
        setTimeValid();
        return fileData;
    }
};
}  // namespace

FileTreeModel::FileTreeModel(QObject *parent)
    : QAbstractItemModel(parent),
      root_item(std::make_unique<TreeItem>(TreeItem::FileData(), QUuid())) {}

int FileTreeModel::columnCount(const QModelIndex &parent) const {
    if (parent.isValid()) {
        return TreeItem::columnCount();
    }
    return TreeItem::columnCount();
}

QVariant FileTreeModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) {
        return {};
    }

    const auto *item = static_cast<const TreeItem *>(index.internalPointer());

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
            return {};
    }
}

Qt::ItemFlags FileTreeModel::flags(const QModelIndex &index) const {
    return index.isValid() ? QAbstractItemModel::flags(index)
                           : Qt::ItemFlags(Qt::NoItemFlags);
}

QModelIndex FileTreeModel::index(int row, int column,
                                 const QModelIndex &parent) const {
    if (!hasIndex(row, column, parent)) {
        return {};
    }

    TreeItem *parentItem =
        parent.isValid() ? static_cast<TreeItem *>(parent.internalPointer())
                         : root_item.get();

    if (auto *childItem = parentItem->child(row)) {
        return createIndex(row, column, childItem);
    }
    return {};
}

QModelIndex FileTreeModel::parent(const QModelIndex &index) const {
    if (!index.isValid()) {
        return {};
    }

    auto *childItem = static_cast<TreeItem *>(index.internalPointer());
    TreeItem *parentItem = childItem->parentItem();

    return parentItem != root_item.get()
               ? createIndex(parentItem->row(), 0, parentItem)
               : QModelIndex{};
}

int FileTreeModel::rowCount(const QModelIndex &parent) const {
    if (parent.column() > 0) {
        return 0;
    }

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
    if (item_map.count(file_id) == 0 || item_map.at(file_id).expired()) {
        return;
    }

    const auto &item_data = item_map[file_id].lock()->data();
    switch (item_data.type) {
        case TreeItem::File:
            getFileCertList(item_data, file_id);
            break;
        case TreeItem::Zip:
            getFileCertList(item_data, file_id);
        case TreeItem::Sig:  // NOLINT(bugprone-branch-clone)
            getSignatureCertList(item_data, file_id);
            break;
        case TreeItem::Asig:
            getSignatureCertList(item_data, file_id);
            break;
        default:
            break;
    }
}

void FileTreeModel::getFileCertList(const TreeItem::FileData &item_data,
                                    int file_id) {
    emit updateSigCount(item_data.ref_id_size);
    std::vector<std::shared_ptr<core::ValidationResult>> res;
    for (const int ind : item_data.ref_ids) {
        if (!item_map.at(ind).expired()) {
            pdfcsp::c_bridge::CPodResult const *pod =
                tree_.GetCheckResultForNode(ind, file_id);
            core::ValidationResult val_res;
            if (pod == nullptr) {
                continue;
            }
            core::createCSPResponse(val_res, pod);
            res.emplace_back(
                std::move(std::make_shared<core::ValidationResult>(val_res)));
        }
    }
    emit signatureReady(res);
}

void FileTreeModel::getSignatureCertList(const TreeItem::FileData &item_data,
                                         int file_id) {
    emit updateSigCount(item_data.ref_id_size);
    std::vector<std::shared_ptr<core::ValidationResult>> res;
    for (const int ind : item_data.ref_ids) {
        // NOLINTNEXTLINE
        pdfcsp::c_bridge::CPodResult const *pod =
            tree_.GetCheckResultForNode(file_id, ind);
        core::ValidationResult val_res;
        if (pod == nullptr) {
            continue;
        }
        core::createCSPResponse(val_res, pod);
        res.emplace_back(
            std::move(std::make_shared<core::ValidationResult>(val_res)));
    }
    emit signatureReady(res);
}

QJsonArray FileTreeModel::getMrpaData(int node_id) {
    QJsonArray arr;
    if (item_map.count(node_id) == 0 || item_map.at(node_id).expired()) {
        return arr;
    }

    const auto &item_data = item_map[node_id].lock()->data();
    switch (item_data.type) {
        case TreeItem::Mrpa:
            if (item_data.mrpa_data.has_value()) {
                arr.append(item_data.mrpa_data.value());
            }
            return arr;
            break;
        case TreeItem::Zip:
            std::for_each(item_data.mrpa_ids.cbegin(),
                          item_data.mrpa_ids.cend(), [this, &arr](int ind) {
                              const auto mrpa_val =
                                  item_map.at(ind).lock()->data().mrpa_data;
                              if (mrpa_val.has_value()) {
                                  arr.append(mrpa_val.value());
                              }
                          });
            break;
        case TreeItem::File:
            std::for_each(item_data.mrpa_ids.cbegin(),
                          item_data.mrpa_ids.cend(), [this, &arr](int ind) {
                              const auto mrpa_val =
                                  item_map.at(ind).lock()->data().mrpa_data;
                              if (mrpa_val.has_value()) {
                                  arr.append(mrpa_val.value());
                              }
                          });
            break;
        case TreeItem::Sig:
            std::for_each(item_data.mrpa_ids.cbegin(),
                          item_data.mrpa_ids.cend(), [this, &arr](int ind) {
                              const auto mrpa_val =
                                  item_map.at(ind).lock()->data().mrpa_data;
                              if (mrpa_val.has_value()) {
                                  arr.append(mrpa_val.value());
                              }
                          });
            break;
        case TreeItem::Asig:
            std::for_each(item_data.mrpa_ids.cbegin(),
                          item_data.mrpa_ids.cend(), [this, &arr](int ind) {
                              const auto mrpa_val =
                                  item_map.at(ind).lock()->data().mrpa_data;
                              if (mrpa_val.has_value()) {
                                  arr.append(mrpa_val.value());
                              }
                          });
            break;
        default:
            return {};
            break;
    }
    return arr;
}

bool FileTreeModel::addNode(const QVariantList &list) {
    if (!list.empty()) {
        QStringList file_list;
        std::transform(
            list.cbegin(), list.cend(), std::back_inserter(file_list),
            [](const QVariant &item) {
                return qvariant_cast<QUrl>(item.value<QVariant>()).toString();
            });

        /// filter from dirs or files that are already in the tree
        file_list.erase(
            std::remove_if(
                file_list.begin(), file_list.end(),
                [this](const QString &file_name) {
                    return QFileInfo(QUrl(file_name).toLocalFile()).isDir() ||
                           root_item->contains(QUrl(file_name).toLocalFile());
                }),
            file_list.end());
        QJsonArray file_array;
        /// transform filtered list into jsonarray with local filenames
        std::transform(file_list.begin(), file_list.end(),
                       std::back_inserter(file_array),
                       [](const QString &file_name) {
                           return (QUrl(file_name).toLocalFile());
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
        return true;
    }
    return false;
}

bool FileTreeModel::deleteNode(const QString &full_path, int row, QUuid uid,
                               int node_id) {
    if (row >= 0) {
        if (!ctx_available_) {
            if (operation_data_.count(full_path) > 0 &&
                operation_data_[full_path].operation == Add) {
                operation_data_.erase(full_path);
            } else {
                operation_data_[full_path].operation = Delete;
                operation_data_[full_path].row = row;
                operation_data_[full_path].file_uid = uid;
                operation_data_[full_path].file_id = node_id;
            }
        }
        QJsonObject obj;
        obj["row"] = row;
        obj["uid"] = uid.toString();
        obj["id"] = node_id;
        QJsonArray delete_array;
        delete_array.append(obj);
        processDelete(delete_array);
        return true;
    }
    qDebug() << "[DEBUG] "
             << "FileTreeModel::deleteNode(): " << "Incorrect row or id";
    return false;
}

void FileTreeModel::deleteTree() {
    beginResetModel();
    operation_data_.clear();
    emit dropState();
    root_item->deleteChildren();
    if (ctx_available_) {
        tree_.ResetContext();
    } else {
        state_ = NeedReset;
    }
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
}

void FileTreeModel::processSignResult() {
    if (sign_future_ && sign_future_->isValid()) {
        const bool res = sign_future_->takeResult();
        auto status = tree_.LastSignStatus();
        if (status.has_value()) {
            /// if sign fails it also comes with signDone signal and later
            /// handled in QML
            emit signDone(QString(status.value().data()), res);
        }
    }
}

void FileTreeModel::setupModelData(const QJsonArray &doc, TreeItem *parent) {
    for (const auto &value : doc) {
        if (!value.isObject()) {
            continue;
        }

        QJsonObject const obj = value.toObject();
        TreeJsonParser parser(obj);
        const TreeItem::FileData fileData = parser.parseAll();

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

std::vector<TreeItem::CheckResult> FileTreeModel::setupCheckResults(
    const QJsonObject &obj, bool has_check_result) {
    std::vector<TreeItem::CheckResult> results;
    if (!has_check_result || !obj.contains("check_results")) {
        return results;
    }
    const QJsonArray checkArray = obj["check_results"].toArray();
    std::for_each(checkArray.cbegin(), checkArray.cend(),
                  [&results](const QJsonValue &item) {
                      if (!item.isObject()) {
                          return;
                      }
                      const QJsonObject item_obj = item.toObject();
                      if (item_obj.contains("file_id") &&
                          item_obj.contains("check_summary")) {
                          TreeItem::CheckResult check_result;
                          check_result.file_id = item_obj["file_id"].toInt();
                          check_result.check_summary =
                              item_obj["check_summary"].toBool();
                          results.emplace_back(check_result);
                      }
                  });
    return results;
}

void FileTreeModel::processAdd(const QJsonArray &arr) {
    switch (state_) {
        case Done: {
            state_ = RunningDraft;
            ctx_available_ = false;
            tree_watcher_ = std::make_unique<TreeFutureWatcher>();
            QObject::connect(tree_watcher_.get(), &TreeFutureWatcher::finished,
                             [this]() { processDraftTree(); });
            tree_future_ =
                std::make_unique<TreeFuture>(QtConcurrent::run([this, arr]() {
                    return tree_.AddFilesJsonList(
                        QJsonDocument(arr).toJson().toStdString());
                }));
            tree_watcher_->setFuture(*tree_future_);
            return;
        }
        case NeedReset:
            [[fallthrough]];
        case RunningDraft:
            [[fallthrough]];
        case RunningSigns: {
            QStringList file_list;
            for (const auto &item : arr) {
                if (item.isString()) {
                    file_list.append(item.toString());
                }
            }
            addFilesUI(file_list);
            qDebug() << "[DEBUG] " << "FileTreeModel::addNode()"
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
                    const int node_id = obj["id"].toInt();
                    file_array.append(node_id);
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
            return;
        }
        case NeedReset:
            [[fallthrough]];
        case RunningDraft:
            [[fallthrough]];
        case RunningSigns: {
            for (const auto &item : arr) {
                if (item.isObject()) {
                    QJsonObject obj = item.toObject();
                    const int row = obj["row"].toInt();
                    const QString uid_str = obj["uid"].toString();
                    deleteFilesUI(row, QUuid(uid_str));
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
        if (state_ == NeedReset) {
            tree_.ResetContext();
            res = std::nullopt;
            state_ = RunningDraft;
        }
        if (operation_data_.empty()) {
            qDebug() << "[DEBUG]" << " FileTreeModel::processDraftTree(): "
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
                    qDebug() << "[DEBUG]"
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
        processOperationData();
    }
}

void FileTreeModel::processSignedTree() {
    if (tree_future_ && tree_future_->isValid()) {
        std::optional<std::string> res = tree_future_->takeResult();
        ctx_available_ = true;
        if (state_ == NeedReset) {
            tree_.ResetContext();
            res = std::nullopt;
            state_ = RunningSigns;
        }
        state_ = Done;
        if (operation_data_.empty()) {
            if (res.has_value()) {
                const QJsonDocument json_doc =
                    QJsonDocument::fromJson(res.value().data());
                if (json_doc.isObject()) {
                    QJsonArray const data_ = json_doc["children"].toArray();
                    beginResetModel();
                    is_draft_ = false;
                    emit isDraftChanged();
                    root_item->deleteChildren();
                    setupModelData(data_, root_item.get());
                    qDebug() << "[DEBUG]"
                             << " FileTreeModel::processSignedTree(): "
                             << "completed and ready";
                    qDebug() << "[DEBUG]"
                             << "root child count: " << root_item->childCount();
                    std::for_each(item_map.begin(), item_map.end(),
                                  [this](auto &item) {
                                      if (!item.second.expired()) {
                                          parseChecksResults(item.first);
                                      }
                                  });
                    endResetModel();
                }
            }
            return;
        }
    }

    processOperationData();
}

void FileTreeModel::processOperationData() {
    /// add files we need to process from operation_data_ map and erase them
    QJsonArray add_array;
    std::for_each(operation_data_.begin(), operation_data_.end(),
                  [this, &add_array](const auto &pair) {
                      if (pair.second.operation == Add) {
                          add_array.append(pair.first);
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

    /* delete_array holds id of nodes as we delete from tree by id
     delete_paths_array hold node full_path string so opeation_data_ can be
     cleared*/
    QJsonArray delete_array;
    QJsonArray delete_paths_array;
    std::for_each(operation_data_.begin(), operation_data_.end(),
                  [this, &delete_array, &delete_paths_array](const auto &pair) {
                      if (pair.second.operation == Delete) {
                          delete_array.append(pair.second.file_id.value());
                          delete_paths_array.append(pair.first);
                      }
                  });
    std::for_each(delete_paths_array.begin(), delete_paths_array.end(),
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
    beginInsertRows(
        QModelIndex(), root_item->childCount(),
        root_item->childCount() + static_cast<int>(file_list.size()) - 1);
    setupModelData(data_, root_item.get());
    endInsertRows();
}

void FileTreeModel::deleteFilesUI(int row, QUuid uid) {
    beginRemoveRows(QModelIndex(), row, row);
    root_item->deleteItem(uid);
    endRemoveRows();
}

void FileTreeModel::parseChecksResults(int node_id) {
    auto item = item_map.at(node_id).lock();
    switch (item->data().type) {
        case TreeItem::Zip:
            parseFileCheckResults(item);
            return;
        case TreeItem::Dir:
            item->setSigStats("", "empty");
            item->setMrpaStats("", "empty");
            return;
        case TreeItem::Sig:  // NOLINT(bugprone-branch-clone)
            parseSignatureCheckResults(item);
            return;
        case TreeItem::Asig:
            parseSignatureCheckResults(item);
            return;
        case TreeItem::File:
            parseFileCheckResults(item);
            return;
        case TreeItem::Mrpa:
            parseMrpaCheckResults(item);
            return;
        default:
            item->setSigStats("", "empty");
            item->setMrpaStats("", "empty");
            return;
            ;
    }
}

void FileTreeModel::parseFileCheckResults(std::shared_ptr<TreeItem> &item) {
    item->setSigStats("", "empty");
    item->setMrpaStats("", "empty");
    if (item->data().encrypted) {
        item->setSigStats(tr("Encrypted"), "file_red");
        return;
    }
    if (item->data().ref_id_size > 0) {
        const auto checks = item->data().ref_ids;
        int valid = 0;
        int invalid = 0;
        std::for_each(
            checks.cbegin(), checks.cend(),
            [&item, &valid, &invalid, this](int ref_id) {
                if (!item_map.at(ref_id).expired() &&
                    tree_.GetCheckResultForNode(ref_id, item->data().id) &&
                    tree_.GetCheckResultForNode(ref_id, item->data().id)
                        ->bres.check_summary) {
                    ++valid;
                } else {
                    ++invalid;
                }
            });
        if (valid > 0 && invalid == 0) {
            item->setSigStats(tr("Signature valid"), "file_green");
        }
        if (valid > 0 && invalid > 0) {
            item->setSigStats(tr("Ambigious"), "file_mixed");
        }
        if (valid == 0 && invalid > 0) {
            item->setSigStats(tr("Signature invalid"), "file_red");
        }
        if (item->data().mrpa_id_size > 0) {
            if (item->data().mrpa_id_size == item->data().ref_id_size) {
                item->setMrpaStats(tr("MRPA signed"), "file_green");
                return;
            }
            item->setMrpaStats(tr("The quantity does not match"), "file_mixed");
        } else {
            item->setMrpaStats("", "empty");
            return;
        }
    }
}

void FileTreeModel::parseMrpaCheckResults(std::shared_ptr<TreeItem> &item) {
    item->setSigStats("", "empty");
    item->setMrpaStats(tr("MRPA"), "invalid");
    if (item->data().ref_id_size > 0) {
        const auto time_val = item->data().time_valid;
        if (time_val.has_value() && !time_val.value()) {
            item->setMrpaStats(tr("MRPA outdated"), "old");
            return;
        }
        item->setMrpaStats(tr("MRPA valid"), "valid");
        return;
    }
}

void FileTreeModel::parseSignatureCheckResults(
    std::shared_ptr<TreeItem> &item) {
    item->setSigStats("", "empty");
    item->setMrpaStats("", "empty");
    if (item->data().has_check_result) {
        if (!item->data().check_results.empty()) {
            int valid = 0;
            int invalid = 0;
            auto checks = item->data().check_results;
            std::for_each(checks.cbegin(), checks.cend(),
                          [&valid, &invalid](const TreeItem::CheckResult &res) {
                              if (res.check_summary) {
                                  ++valid;
                              } else {
                                  ++invalid;
                              }
                          });
            if (valid > 0 && invalid == 0) {
                item->setSigStats(tr("Signature valid"), "sig_green");
            } else if (valid > 0 && invalid > 0) {
                item->setSigStats(tr("File not found"), "sig_red");
            } else {
                item->setSigStats(tr("Signature invalid"), "sig_red");
            }
            if (item->data().mrpa_id_size > 0) {
                if (item->data().mrpa_id_size == item->data().ref_id_size) {
                    item->setMrpaStats(tr("MRPA signed"), "file_green");
                    return;
                }
                item->setMrpaStats(tr("The quantity does not match"), "empty");
            } else {
                item->setMrpaStats("", "empty");
            }
        }
    } else {
        item->setSigStats(tr("File not found"), "sig_mixed");
    }
}

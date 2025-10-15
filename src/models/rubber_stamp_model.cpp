/* File: rubber_stamp_model.cpp
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

#include "rubber_stamp_model.hpp"

#include <QDir>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>

#include "bridge_utils.hpp"

RubberStampModel::RubberStampModel(QObject* parent)
    : QAbstractListModel(parent),
      approve_stamp_title_{tr("Approve")},
      decline_stamp_title_{tr("Decline")},
      create_stamp_title_{tr("Create mark")} {
    role_names_[TitleRole] = "title";
    role_names_[ValueRole] = "value";
    readRubberStamps();
}

QVariant RubberStampModel::headerData(int /*section*/,
                                      Qt::Orientation /*orientation*/,
                                      int /*role*/) const {
    return {};
}

QHash<int, QByteArray> RubberStampModel::roleNames() const {
    return role_names_;
}

int RubberStampModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) {
        return 0;
    }
    if (rubber_stamps_.count() > std::numeric_limits<int>::max()) {
        qWarning() << "[profilesModel::rowCount] can not cast to integer";
        return 0;
    }
    return static_cast<int>(rubber_stamps_.count());
}

QVariant RubberStampModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() > rubber_stamps_.size() - 1) {
        return {};
    }
    switch (role) {
        case TitleRole: {
            const QString res = rubber_stamps_.at(index.row())
                                    .toObject()
                                    .value("title")
                                    .toString();
            return res;
        }
        case ValueRole: {
            if (rubber_stamps_.at(index.row())
                    .toObject()
                    .value("title")
                    .toString() == create_stamp_title_) {
                return "new";
            }
            const QString res =
                QJsonDocument(rubber_stamps_.at(index.row()).toObject())
                    .toJson();
            return res;
        }
        default:
            return {};
    }

    return {};
}

/// @brief readRubberStamps from JSON file in
/// @details /HOME/USER/.config/pdfcsp/rubber_stamps.json
void RubberStampModel::readRubberStamps() {
    config_path_ =
        QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    if (config_path_.isEmpty()) {
        qWarning() << tr(
            "Standard config location cannot be determined, using home");
        config_path_ = QDir::homePath();
    }
    if (config_path_.isEmpty()) {
        const QStringList config_path_list =
            QStandardPaths::standardLocations(QStandardPaths::ConfigLocation);
        qWarning() << tr("Cannot determine the user's home folder");
        if (!config_path_list.empty()) {
            config_path_ = config_path_list.at(0);
            qWarning() << tr("using the path ") << config_path_;
        }
    }
    if (!config_path_.isEmpty()) {
        config_path_ += "/csppdf";
        const QDir config_dir(config_path_);
        if (!config_dir.exists()) {
            if (!config_dir.mkpath(".")) {
                qWarning() << tr("Can not create folder ") << config_path_;
            }
        }
    }
    if (config_path_.isEmpty()) {
        return;
    }
    rubber_stamps_file_name_ = config_path_ + "/rubber_stamps.json";
    QFile stamps_file(rubber_stamps_file_name_);
    // create empty json array if not exists
    if (!stamps_file.exists()) {
        if (!stamps_file.open(
                QIODeviceBase::WriteOnly,
                QFileDevice::ReadOwner | QFileDevice::WriteOwner)) {
            qWarning() << tr("Can not create file ")
                       << rubber_stamps_file_name_;
            return;
        }
        QTextStream out(&stamps_file);
        //  out << "[{\"id\":1,\"title\":\"" << approve_stamp_title_ << "\"}, "
        // << "{\"id\":2,\"title\":\"" << decline_stamp_title_ << "\"}]";
        out << generateConfig();
        stamps_file.close();
    }
    if (!stamps_file.exists()) {
        return;
    }
    // read the file
    if (!stamps_file.open(QIODevice::ReadOnly)) {
        qWarning() << tr("Can not open file ") << rubber_stamps_file_name_;
        return;
    }
    const QByteArray file_data = stamps_file.readAll();
    stamps_file.close();
    const QJsonDocument json_doc = QJsonDocument::fromJson(file_data);
    if (json_doc.isNull() || !json_doc.isArray()) {
        qWarning() << tr("Error parsing JSON from file ")
                   << rubber_stamps_file_name_;
    }
    const QJsonObject create_profile_field{{"id", 0},
                                           {"title", create_stamp_title_}};
    rubber_stamps_ = json_doc.array();
    rubber_stamps_.append(create_profile_field);
}

/// @brief get a json array with rubber stamps
QString RubberStampModel::getRubberStampsJSON() const {
    const QJsonDocument json_doc(rubber_stamps_);
    return json_doc.toJson();
}

/// @brief save rubber stamp
bool RubberStampModel::saveRubberStamps(const QString& stamp_json) {
    if (stamp_json.isEmpty()) {
        return false;
    }
    const QByteArray stamp_data = stamp_json.toUtf8();
    const QJsonDocument json_doc = QJsonDocument::fromJson(stamp_data);
    if (json_doc.isNull() || !json_doc.isObject()) {
        qWarning()
            << "[RubberStampModel] error parsing JSON,can not save the stamp";
    }
    QJsonObject stamp_object = json_doc.object();
    QString old_img_path;
    // if new profile - create a new id
    if (stamp_object.value("id").toInt() == -1) {
        const bool unique_name =
            stamp_object.contains("title") &&
            uniqueStampName(stamp_object.value("title").toString());
        if (!unique_name) {
            qWarning()
                << "Can't create stamp,stamp with this name already exists";
            return false;
        }

        auto it_max_current = std::max_element(
            rubber_stamps_.cbegin(), rubber_stamps_.cend(),
            [](const QJsonValue& left, const QJsonValue& right) {
                return left.toObject().value("id").toInt() <
                       right.toObject().value("id").toInt();
            });
        const int new_id =
            it_max_current == rubber_stamps_.cend()
                ? 0
                : it_max_current->toObject().value("id").toInt() + 1;
        stamp_object.insert("id", new_id);
    }
    // existing profile
    else {
        auto it_old_value = std::find_if(
            rubber_stamps_.begin(), rubber_stamps_.end(),
            [&stamp_object](const QJsonValue& val) {
                return val.toObject().value("id") == stamp_object.value("id");
            });
        if (it_old_value != rubber_stamps_.cend()) {
            const QJsonObject old_profile = it_old_value->toObject();
            if (old_profile.contains("img_path")) {
                old_img_path = old_profile.value("img_path").toString();
            }
            rubber_stamps_.erase(it_old_value);
        }
    }
    // copy the image
    const QString img_path = stamp_object.value("img_path").toString();
    const QString dest_name =
        "tag_" + QString::number(stamp_object.value("id").toInt()) + "_logo";
    const QString copy_result_name =
        saveLogoImage(img_path, dest_name, old_img_path);
    stamp_object.insert("img_path", copy_result_name);
    rubber_stamps_.push_back(stamp_object);
    // save profiles
    QJsonArray stamps;
    std::copy_if(rubber_stamps_.cbegin(), rubber_stamps_.cend(),
                 std::back_inserter(stamps), [this](const QJsonValue& val) {
                     return val.toObject().value("title").toString() !=
                            create_stamp_title_;
                 });
    QFile file(rubber_stamps_file_name_);
    const QString stamps_data = QJsonDocument(stamps).toJson();
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << stamps_data;
        file.close();
        beginResetModel();
        readRubberStamps();
        endResetModel();
        const QString saved_name = stamp_object.value("title").toString();
        rubberStampSaved(saved_name);
        return true;
    }
    endResetModel();
    qWarning() << "[RubberStampModel] failed to save stamps";
    return false;
}

/**
 * @brief Save logo image
 *
 * @param path source image path
 * @param dest_name destination file name
 * @param old_logo_path old logo to delete
 * @return QString full path to saved logo on success
 */
QString RubberStampModel::saveLogoImage(const QString& path,
                                        const QString& dest_name,
                                        const QString& old_logo_path) {
    if (path.isEmpty()) {
        return {};
    }
    const QString file_path = QUrl(path).toString(QUrl::PreferLocalFile);
    const QFileInfo src_file_info(file_path);
    if (!src_file_info.exists() || !src_file_info.isFile()) {
        qWarning() << "[RubberStampModel] can not save the image, file does "
                      "not exist: "
                   << path;
        return {};
    }
    if (!src_file_info.isReadable()) {
        qWarning() << "[RubberStampModel] the file is not readable :" << path;
        return {};
    }
    if (src_file_info.isExecutable()) {
        qWarning()
            << "[RubberStampModel] the file is executable, will not copy :"
            << path;
        return {};
    }
    if (src_file_info.size() > 100000000) {
        qWarning() << "[RubberStampModel] file size >100 MB , will not copy :"
                   << path;
        return {};
    }

    QString dest =
        config_path_ + "/" + dest_name + "." + src_file_info.completeSuffix();
    // delete old logo
    if (dest != old_logo_path && old_logo_path != file_path) {
        QFile old_logo_file(old_logo_path);
        if (old_logo_file.exists()) {
            std::ignore = old_logo_file.remove();
        }
    }
    QFile dest_file(dest);
    if (dest_file.exists()) {
        if (dest != file_path) {
            std::ignore = dest_file.remove();
        } else {
            return dest;
        }
    }
    if (!QFile::copy(file_path, dest)) {
        qWarning() << "[RubberStampModel] Failed to copy file from " << path
                   << " to " << dest;
        return {};
    }
    return dest;
}

bool RubberStampModel::uniqueStampName(QString stamp_name) {
    return !stamp_name.isEmpty() &&
           std::none_of(rubber_stamps_.begin(), rubber_stamps_.end(),
                        [&stamp_name](const QJsonValue& val) {
                            return stamp_name ==
                                   val.toObject().value("title").toString();
                        });
}

bool RubberStampModel::deleteRubberStamps(int id_stamp) {
    QString stamp_title;
    QJsonArray stamps_new;
    for (qsizetype i = 0; i < rubber_stamps_.count(); ++i) {
        if (!rubber_stamps_[i].isObject()) {
            continue;
        }
        if (rubber_stamps_[i].toObject().value("id").toInt() == id_stamp) {
            stamp_title =
                rubber_stamps_[i].toObject().value("title").toString();
        } else if (rubber_stamps_[i].toObject().value("title").toString() !=
                   create_stamp_title_) {
            stamps_new.append(rubber_stamps_[i]);
        }
    }
    QFile file(rubber_stamps_file_name_);
    const QString stamp_data = QJsonDocument(stamps_new).toJson();
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << stamp_data;
        file.close();
        beginResetModel();
        readRubberStamps();
        endResetModel();
        rubberStampDeleted(stamp_title);
        return true;
    }
    endResetModel();
    qWarning() << "[RubberStampModel] failed to delete stamp";
    return false;
}

QString RubberStampModel::generateConfig() const {
    return QString(R"([
    {
        "B": 168,
        "G": 62,
        "R": 50,
        "bg_transparent": 0,
        "border_radius": 50,
        "border_width": 7,
        "create_from_image": 0,
        "font_family": "Noto Sans",
        "id": 1,
        "img_path": "",
        "stamp_link": "",
        "stamp_text": "%1",
        "tag_width": 30,
        "title": "%1"
    },
    {
        "B": 181,
        "G": 62,
        "R": 50,
        "bg_transparent": 0,
        "border_radius": 50,
        "border_width": 7,
        "create_from_image": 0,
        "font_family": "Noto Sans",
        "id": 2,
        "img_path": "",
        "stamp_link": "",
        "stamp_text": "%2",
        "tag_width": 30,
        "title": "%2"
    }
])")
        .arg(approve_stamp_title_, decline_stamp_title_);
}

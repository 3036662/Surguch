/* File: profiles_model.cpp
Copyright (C) Basealt LLC,  2024
Author: Oleg Proskurin, <proskurinov@basealt.ru>

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

#include "profiles_model.hpp"

#include <QDir>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>

#include "bridge_utils.hpp"

ProfilesModel::ProfilesModel(QObject *parent)
    : QAbstractListModel(parent),
      create_profile_title_{tr("CreateProfile")},
      create_stamp_title_{tr("CreateStamp")} {
    role_names_[TitleRole] = "title";
    role_names_[ValueRole] = "value";
    readProfiles();
    readUserCerts();
    readUserStamps();
}

QVariant ProfilesModel::headerData(int /*section*/,
                                   Qt::Orientation /*orientation*/,
                                   int /*role*/) const {
    return {};
}

QHash<int, QByteArray> ProfilesModel::roleNames() const { return role_names_; }

int ProfilesModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid()) {
        return 0;
    }
    if (profiles_.count() > std::numeric_limits<int>::max()) {
        qWarning() << "[profilesModel::rowCount] can not cast to integer";
        return 0;
    }
    return static_cast<int>(profiles_.count());
}

QVariant ProfilesModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() > profiles_.size() - 1) {
        return {};
    }
    switch (role) {
        case TitleRole: {
            const QString res =
                profiles_.at(index.row()).toObject().value("title").toString();
            return res;
        }
        case ValueRole: {
            if (profiles_.at(index.row())
                    .toObject()
                    .value("title")
                    .toString() == create_profile_title_) {
                return "new";
            }
            // profiles_.at(index.row()).toObject().value("id").toInt();
            const QString res =
                QJsonDocument(profiles_.at(index.row()).toObject()).toJson();
            return res;
        }
        default:
            return {};
    }

    return {};
}

/// @brief readProfiles from JSON file in
/// @details /HOME/USER/.config/pdfcsp/profiles.json
void ProfilesModel::readProfiles() {
    config_path_ =
        QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    if (config_path_.isEmpty()) {
        qWarning() << tr(
            "Standart config location cannot be determined, using home");
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
    profiles_file_name_ = config_path_ + "/profiles.json";
    QFile profile_file(profiles_file_name_);
    // create empty json array if not exists
    if (!profile_file.exists()) {
        if (!profile_file.open(
                QIODeviceBase::WriteOnly,
                QFileDevice::ReadOwner | QFileDevice::WriteOwner)) {
            qWarning() << tr("Can not create file ") << profiles_file_name_;
            return;
        }
        QTextStream out(&profile_file);
        out << "[]";
        profile_file.close();
    }
    if (!profile_file.exists()) {
        return;
    }
    // read the file
    if (!profile_file.open(QIODevice::ReadOnly)) {
        qWarning() << tr("Can not open file ") << profiles_file_name_;
        return;
    }
    const QByteArray file_data = profile_file.readAll();
    profile_file.close();
    const QJsonDocument json_doc = QJsonDocument::fromJson(file_data);
    if (json_doc.isNull() || !json_doc.isArray()) {
        qWarning() << tr("Error parsing JSON from file ")
                   << profiles_file_name_;
    }
    profiles_ = json_doc.array();
    const QJsonObject create_profile_field{{"title", create_profile_title_}};
    profiles_.append(create_profile_field);
}

/// @brief readUserCerts, read certificates for current uset from CryptoApi
void ProfilesModel::readUserCerts() {
    QString certs_json = core::bridge_utils::getCertListJSON();
    if (certs_json == core::bridge_utils::kErrGetCerts ||
        certs_json == core::bridge_utils::kErrNoCSPLib) {
        qWarning() << tr("Failed getting the user's certificates list");
        error_status_ = true;
        if (!certs_json.isEmpty()) {
            err_string_ = certs_json;
        }
        return;
    }
    if (certs_json.isEmpty()) {
        certs_json = "[]";
    }
    const QByteArray certs_json_qb = certs_json.toUtf8();
    const QJsonDocument json_doc = QJsonDocument::fromJson(certs_json_qb);
    if (json_doc.isEmpty() || !json_doc.isArray()) {
        qWarning() << tr(
            "Failed to parse the certificates list from the JSON string");
        return;
    }
    user_certs_ = json_doc.array();
}

/// @brief readUserStamps, read stamps details from JSON file in
/// @details /HOME/USER/.config/pdfscp/stamps.json
void ProfilesModel::readUserStamps() {
    config_path_ =
        QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    if (config_path_.isEmpty()) {
        qWarning() << tr(
            "Standart config location cannot be determined, using home");
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
    stamps_file_name_ = config_path_ + "/stamps.json";
    QFile stamps_file(stamps_file_name_);
    // create empty json array if not exists
    if (!stamps_file.exists()) {
        if (!stamps_file.open(
                QIODeviceBase::WriteOnly,
                QFileDevice::ReadOwner | QFileDevice::WriteOwner)) {
            qWarning() << tr("Can not create file ") << stamps_file_name_;
            return;
        }
        QTextStream out(&stamps_file);
        out << "[{\"id\":1,\"title\": \"ГОСТ\"}]";
        stamps_file.close();
    }
    if (!stamps_file.exists()) {
        return;
    }
    // read the file
    if (!stamps_file.open(QIODevice::ReadOnly)) {
        qWarning() << tr("Can not open file ") << stamps_file_name_;
        return;
    }
    const QByteArray file_data = stamps_file.readAll();
    stamps_file.close();
    const QJsonDocument json_doc = QJsonDocument::fromJson(file_data);
    if (json_doc.isNull() || !json_doc.isArray()) {
        qWarning() << tr("Error parsing JSON from file ") << stamps_file_name_;
    }
    const QJsonObject create_stamp_field{{"id", 0},
                                         {"title", create_stamp_title_}};
    user_stamps_ = json_doc.array();
    user_stamps_.append(create_stamp_field);
}

///@brief get a json array with user certificates
QString ProfilesModel::getUserCertsJSON() const {
    const QJsonDocument json_doc(user_certs_);
    return json_doc.toJson();
}

///@brief get a json array with user stamps
QString ProfilesModel::getUserStampsJSON() const {
    const QJsonDocument json_doc(user_stamps_);
    return json_doc.toJson();
}

/// @brief check if the given stamp name is unique
Q_INVOKABLE bool ProfilesModel::uniqueStampName(QString stamp_name) {
    return !stamp_name.isEmpty() &&
           std::none_of(user_stamps_.begin(), user_stamps_.end(),
                        [&stamp_name](const QJsonValue &val) {
                            return stamp_name ==
                                   val.toObject().value("title").toString();
                        });
}

///@brief save user's stamp
Q_INVOKABLE bool ProfilesModel::saveStamp(const QString &stamp_json) {
    if (stamp_json.isEmpty()) {
        return false;
    }
    const QByteArray stamp_data = stamp_json.toUtf8();
    const QJsonDocument json_doc = QJsonDocument::fromJson(stamp_data);
    if (json_doc.isNull() || !json_doc.isObject()) {
        qWarning()
            << "[ProfilesModel] error parsing JSON,can not save the stamp";
    }
    QJsonObject stamp_object = json_doc.object();
    // if new profile - create a new id
    if (stamp_object.value("id").toInt() == -1) {
        const bool unique_name =
            stamp_object.contains("title") &&
            uniqueName(stamp_object.value("title").toString());
        if (!unique_name) {
            qWarning()
                << "Can't create stamp,stamp with this name already exists";
            return false;
        }

        auto it_max_current = std::max_element(
            user_stamps_.cbegin(), user_stamps_.cend(),
            [](const QJsonValue &left, const QJsonValue &right) {
                return left.toObject().value("id").toInt() <
                       right.toObject().value("id").toInt();
            });
        const int new_id =
            it_max_current == user_stamps_.cend()
                ? 0
                : it_max_current->toObject().value("id").toInt() + 1;
        stamp_object.insert("id", new_id);
    }
    // existing profile
    else {
        auto it_old_value = std::find_if(
            user_stamps_.begin(), user_stamps_.end(),
            [&stamp_object](const QJsonValue &val) {
                return val.toObject().value("id") == stamp_object.value("id");
            });
        if (it_old_value != user_stamps_.cend()) {
            user_stamps_.erase(it_old_value);
        }
    }
    user_stamps_.push_back(stamp_object);
    // save profiles
    QJsonArray stamps;
    std::copy_if(user_stamps_.cbegin(), user_stamps_.cend(),
                 std::back_inserter(stamps), [this](const QJsonValue &val) {
                     return val.toObject().value("title").toString() !=
                            create_stamp_title_;
                 });
    QFile file(stamps_file_name_);
    const QString stamps_data = QJsonDocument(stamps).toJson();
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << stamps_data;
        file.close();
        beginResetModel();
        readUserStamps();
        endResetModel();
        const QString saved_name = stamp_object.value("title").toString();
        stampsSaved(saved_name);
        return true;
    }
    endResetModel();
    qWarning() << "[ProfilesModel] failed to save stamps";
    return false;
}

/// @brief check if the given name is unique
Q_INVOKABLE bool ProfilesModel::uniqueName(QString profile_name) {
    return !profile_name.isEmpty() &&
           std::none_of(profiles_.begin(), profiles_.end(),
                        [&profile_name](const QJsonValue &val) {
                            return profile_name ==
                                   val.toObject().value("title").toString();
                        });
}

/// @brief save user's profile
Q_INVOKABLE bool ProfilesModel::saveProfile(const QString &profile_json) {
    if (profile_json.isEmpty()) {
        return false;
    }
    const QByteArray profile_data = profile_json.toUtf8();
    const QJsonDocument json_doc = QJsonDocument::fromJson(profile_data);
    if (json_doc.isNull() || !json_doc.isObject()) {
        qWarning()
            << "[ProfilesModel] error parsing JSON,can not save the profile";
    }
    QJsonObject profile_object = json_doc.object();
    QString old_logo_path;
    // if new profile - create a new id
    if (profile_object.value("id").toInt() == -1) {
        const bool unique_name =
            profile_object.contains("title") &&
            uniqueName(profile_object.value("title").toString());
        if (!unique_name) {
            qWarning()
                << "Can't create profile,profile with this name already exists";
            return false;
        }

        auto it_max_current = std::max_element(
            profiles_.cbegin(), profiles_.cend(),
            [](const QJsonValue &left, const QJsonValue &right) {
                return left.toObject().value("id").toInt() <
                       right.toObject().value("id").toInt();
            });
        const int new_id =
            it_max_current == profiles_.cend()
                ? 0
                : it_max_current->toObject().value("id").toInt() + 1;
        profile_object.insert("id", new_id);
    }
    // existing profile
    else {
        auto it_old_value = std::find_if(
            profiles_.begin(), profiles_.end(),
            [&profile_object](const QJsonValue &val) {
                return val.toObject().value("id") == profile_object.value("id");
            });
        if (it_old_value != profiles_.cend()) {
            const QJsonObject old_profile = it_old_value->toObject();
            if (old_profile.contains("logo_path")) {
                old_logo_path = old_profile.value("logo_path").toString();
            }
            profiles_.erase(it_old_value);
        }
    }
    // copy the image
    const QString img_path = profile_object.value("logo_path").toString();
    const QString dest_name =
        "profile_" + QString::number(profile_object.value("id").toInt()) +
        "_logo";
    const QString copy_result_name =
        saveLogoImage(img_path, dest_name, old_logo_path);
    profile_object.insert("logo_path", copy_result_name);
    // if new profile will be used as default, disable default use for other
    // profiles
    if (profile_object.value("use_as_default").toBool()) {
        for (qsizetype i = 0; i < profiles_.count(); ++i) {
            QJsonObject tmp = profiles_[i].toObject();
            tmp.insert("use_as_default", false);
            profiles_.replace(i, tmp);
        }
    }
    profiles_.push_back(profile_object);
    // save profiles
    QJsonArray profiles;
    std::copy_if(profiles_.cbegin(), profiles_.cend(),
                 std::back_inserter(profiles), [this](const QJsonValue &val) {
                     return val.toObject().value("title").toString() !=
                            create_profile_title_;
                 });
    QFile file(profiles_file_name_);
    const QString profiles_data = QJsonDocument(profiles).toJson();
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << profiles_data;
        file.close();
        beginResetModel();
        readProfiles();
        endResetModel();
        const QJsonDocument saved_val(profile_object);
        profileSaved(saved_val.toJson());
        return true;
    }
    endResetModel();
    qWarning() << "[ProfilesModel] failed to save profiles";
    return false;
}

/// @brief Update profiles if their stamp model was deleted
Q_INVOKABLE void ProfilesModel::updateProfiles(const QString &stamp_name) {
    qsizetype create_title_pos = 0;
    for (int i = 0; i < profiles_.size(); ++i) {
        const QJsonObject profile_obj = profiles_.at(i).toObject();
        if (profile_obj.value("title").toString() == create_profile_title_) {
            create_title_pos = i;
        }
        if (profile_obj.contains("stamp_type") &&
            profile_obj.value("stamp_type").toString() == stamp_name) {
            QJsonObject updated_obj = profile_obj;
            updated_obj["stamp_type"] = "ГОСТ";
            profiles_[i] = updated_obj;
        }
    }
    profiles_.removeAt(create_title_pos);
    QFile file(profiles_file_name_);
    const QString profiles_data = QJsonDocument(profiles_).toJson();
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << profiles_data;
        file.close();
        beginResetModel();
        readProfiles();
        endResetModel();
        profilesUpdated();
    }
}

/**
 * @brief Save logo image
 *
 * @param path source image path
 * @param dest_name destanation file name
 * @param old_logo_path old logo to delete
 * @return QString full path to saved logo on success
 */
QString ProfilesModel::saveLogoImage(const QString &path,
                                     const QString &dest_name,
                                     const QString &old_logo_path) {
    if (path.isEmpty()) {
        return {};
    }
    const QString file_path = QUrl(path).toString(QUrl::PreferLocalFile);
    const QFileInfo src_file_info(file_path);
    if (!src_file_info.exists() || !src_file_info.isFile()) {
        qWarning()
            << "[ProfilesModel] can not save the image, file does not exist: "
            << path;
        return {};
    }
    if (!src_file_info.isReadable()) {
        qWarning() << "[ProfilesModel] the file is not readable :" << path;
        return {};
    }
    if (src_file_info.isExecutable()) {
        qWarning() << "[ProfilesModel] the file is executable, will not copy :"
                   << path;
        return {};
    }
    if (src_file_info.size() > 100000000) {
        qWarning() << "[ProfilesModel] file size >100 MB , will not copy :"
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
            dest_file.remove();
        } else {
            return dest;
        }
    }
    if (!QFile::copy(file_path, dest)) {
        qWarning() << "[ProfilesModel] Failed to copy file from " << path
                   << " to " << dest;
        return {};
    }
    return dest;
}

/// @brief find the default profile and return it as JSON string
QString ProfilesModel::getDetDefaultProfileVal() {
    for (const auto &profile : profiles_) {
        if (profile.toObject().value("use_as_default").toBool()) {
            const QJsonDocument tmp_doc(profile.toObject());
            return tmp_doc.toJson();
        }
    }
    return {};
}

/// @brief delete the user's stamp
bool ProfilesModel::deleteStamp(int id_stamp) {
    QString stamp_title;
    QJsonArray stamps_new;
    for (qsizetype i = 0; i < user_stamps_.count(); ++i) {
        if (!user_stamps_[i].isObject()) {
            continue;
        }
        if (user_stamps_[i].toObject().value("id").toInt() == id_stamp) {
            stamp_title = user_stamps_[i].toObject().value("title").toString();
        } else if (user_stamps_[i].toObject().value("title").toString() !=
                   create_stamp_title_) {
            stamps_new.append(user_stamps_[i]);
        }
    }
    QFile file(stamps_file_name_);
    const QString stamp_data = QJsonDocument(stamps_new).toJson();
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << stamp_data;
        file.close();
        beginResetModel();
        readUserStamps();
        endResetModel();
        stampDeleted(stamp_title);
        return true;
    }
    endResetModel();
    qWarning() << "[ProfilesModel] failed to delete stamp";
    return false;
}

/// @brief delete the user's profile
bool ProfilesModel::deleteProfile(int id_profile) {
    QString profile_title;
    QJsonArray profiles_new;
    for (qsizetype i = 0; i < profiles_.count(); ++i) {
        if (!profiles_[i].isObject()) {
            continue;
        }
        if (profiles_[i].toObject().value("id").toInt() == id_profile) {
            profile_title = profiles_[i].toObject().value("title").toString();
            deleteLogoImage(
                profiles_[i].toObject().value("logo_path").toString());
        } else if (profiles_[i].toObject().value("title").toString() !=
                   create_profile_title_) {
            profiles_new.append(profiles_[i]);
        }
    }
    QFile file(profiles_file_name_);
    const QString profiles_data = QJsonDocument(profiles_new).toJson();
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << profiles_data;
        file.close();
        beginResetModel();
        readProfiles();
        endResetModel();
        profileDeleted(profile_title);
        return true;
    }
    endResetModel();
    qWarning() << "[ProfilesModel] failed to save profiles";
    return false;
}

/// @brief delete the given file
bool ProfilesModel::deleteLogoImage(const QString &path) {
    const QFileInfo finfo_tmp(path);
    // use only filename and extension
    const QString path_to_delete = config_path_ + "/" + finfo_tmp.fileName();
    const QFileInfo finfo(path_to_delete);
    if (finfo.exists() && finfo.isFile() && !QFile(path_to_delete).remove()) {
        qWarning() << "[ProfilesModel] can't delete file: " << path_to_delete;
    }
    return true;
}

/// @brief path to config directory (/home/$user/.config)
QString ProfilesModel::getConfigPath() const { return config_path_; }

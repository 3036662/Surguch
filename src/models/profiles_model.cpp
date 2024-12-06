#include "profiles_model.hpp"
#include "bridge_utils.hpp"
#include <QDir>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>

ProfilesModel::ProfilesModel(QObject *parent)
    : QAbstractListModel(parent), create_profile_title_{tr("CreateProfile")} {
  role_names_[TitleRole] = "title";
  role_names_[ValueRole] = "value";
  readProfiles();
  readUserCerts();
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
  return profiles_.count();
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
  case ValueRole:
    if (profiles_.at(index.row()).toObject().value("title").toString() ==
        create_profile_title_) {
      return "new";
    }
    // profiles_.at(index.row()).toObject().value("id").toInt();
    const QString res =
        QJsonDocument(profiles_.at(index.row()).toObject()).toJson();
    return res;
  }
  return {};
}

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
    if (!profile_file.open(QIODeviceBase::WriteOnly,
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
    qWarning() << tr("Error parsing JSON from file ") << profiles_file_name_;
  }
  profiles_ = json_doc.array();
  const QJsonObject create_profile_field{{"title", create_profile_title_}};
  profiles_.append(create_profile_field);
}

void ProfilesModel::readUserCerts() {
  const QString certs_json = core::bridge_utils::getCertListJSON();
  if (certs_json.isEmpty()) {
    qWarning() << tr("Failed getting the user's certificates list");
    return;
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

QString ProfilesModel::getUserCertsJSON() const {
  const QJsonDocument json_doc(user_certs_);
  return json_doc.toJson();
}

Q_INVOKABLE bool ProfilesModel::uniqueName(QString profile_name) {
  return !profile_name.isEmpty() &&
         std::none_of(profiles_.begin(), profiles_.end(),
                      [&profile_name](const QJsonValue &val) {
                        return profile_name ==
                               val.toObject().value("title").toString();
                      });
}

Q_INVOKABLE bool ProfilesModel::saveProfile(QString profile_json) {
  if (profile_json.isEmpty()) {
    return false;
  }
  const QByteArray profile_data = profile_json.toUtf8();
  const QJsonDocument json_doc = QJsonDocument::fromJson(profile_data);
  if (json_doc.isNull() || !json_doc.isObject()) {
    qWarning() << "[ProfilesModel] error parsing JSON,can not save the profile";
  }
  QJsonObject profile_object = json_doc.object();
  QString old_logo_path;
  // if new profile - create a new id
  if (profile_object.value("id").toInt() == -1) {
    bool unique_name = profile_object.contains("title") &&
                       uniqueName(profile_object.value("title").toString());
    if (!unique_name) {
      qWarning()
          << "Can't create profile,profile with this name already exists";
      return false;
    }

    auto it_max_current =
        std::max_element(profiles_.cbegin(), profiles_.cend(),
                         [](const QJsonValue &left, const QJsonValue &right) {
                           return left.toObject().value("id").toInt() <
                                  right.toObject().value("id").toInt();
                         });
    const int new_id = it_max_current == profiles_.cend()
                           ? 0
                           : it_max_current->toObject().value("id").toInt() + 1;
    profile_object.insert("id", new_id);
  }
  // existing profile
  else {
    auto it_old_value = std::find_if(profiles_.begin(), profiles_.end(),
                                     [&profile_object](const QJsonValue &val) {
                                       return val.toObject().value("id") ==
                                              profile_object.value("id");
                                     });
    if (it_old_value != profiles_.cend()) {
      QJsonObject old_profile = it_old_value->toObject();
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

QString ProfilesModel::saveLogoImage(const QString &path,
                                     const QString &dest_name,
                                     const QString &old_logo_path) {
  if (path.isEmpty()) {
    return {};
  }
  QString file_path = QUrl(path).toString(QUrl::PreferLocalFile);
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
    qWarning() << "[ProfilesModel] file size >100 MB , will not copy :" << path;
    return {};
  }

  QString dest =
      config_path_ + "/" + dest_name + "." + src_file_info.completeSuffix();
  // delete old logo
  if (dest != old_logo_path && old_logo_path != file_path) {
    QFile old_logo_file(old_logo_path);
    if (old_logo_file.exists()) {
      old_logo_file.remove();
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
    qWarning() << "[ProfilesModel] Failed to copy file from " << path << " to "
               << dest;
    return {};
  }
  return dest;
}

QString ProfilesModel::getDetDefaultProfileVal() {
  for (const auto &profile : profiles_) {
    if (profile.toObject().value("use_as_default").toBool()) {
      const QJsonDocument tmp_doc(profile.toObject());
      return tmp_doc.toJson();
    }
  }
  return {};
}

bool ProfilesModel::deleteProfile(int id_profile) {

  QString profile_title;
  QJsonArray profiles_new;
  for (qsizetype i = 0; i < profiles_.count(); ++i) {
    if (!profiles_[i].isObject()) {
      continue;
    }
    if (profiles_[i].toObject().value("id").toInt() == id_profile) {
      profile_title = profiles_[i].toObject().value("title").toString();
      deleteLogoImage(profiles_[i].toObject().value("logo_path").toString());
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

QString ProfilesModel::getConfigPath() const { return config_path_; }

#include "profiles_model.hpp"
#include "bridge_utils.hpp"
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>

ProfilesModel::ProfilesModel(QObject *parent) : QAbstractListModel(parent) {
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
    return "new";
  }
  return {};
}

void ProfilesModel::readProfiles() {
  QString config_path =
      QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
  if (config_path.isEmpty()) {
    qWarning() << tr(
        "Standart config location cannot be determined, using home");
    config_path = QDir::homePath();
  }
  if (config_path.isEmpty()) {
    const QStringList config_path_list =
        QStandardPaths::standardLocations(QStandardPaths::ConfigLocation);
    qWarning() << tr("Cannot determine the user's home folder");
    if (!config_path_list.empty()) {
      config_path = config_path_list.at(0);
      qWarning() << tr("using the path ") << config_path;
    }
  }
  if (!config_path.isEmpty()) {
    config_path += "/csppdf";
    const QDir config_dir(config_path);
    if (!config_dir.exists()) {
      if (!config_dir.mkpath(".")) {
        qWarning() << tr("Con not create folder ") << config_path;
      }
    }
  }
  if (config_path.isEmpty()) {
    return;
  }
  const QString profiles_file_name = config_path + "/profiles.json";
  QFile profile_file(profiles_file_name);
  // create empty json array if not exists
  if (!profile_file.exists()) {
    if (!profile_file.open(QIODeviceBase::WriteOnly,
                           QFileDevice::ReadOwner | QFileDevice::WriteOwner)) {
      qWarning() << tr("Can not create file ") << profiles_file_name;
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
    qWarning() << tr("Can not open file ") << profiles_file_name;
    return;
  }
  const QByteArray file_data = profile_file.readAll();
  profile_file.close();
  const QJsonDocument json_doc = QJsonDocument::fromJson(file_data);
  if (json_doc.isNull() || !json_doc.isArray()) {
    qWarning() << tr("Error parsing JSON from file ") << profiles_file_name;
  }
  profiles_ = json_doc.array();
  const QJsonObject create_profile_field{{"title", "CreateProfile"}};
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

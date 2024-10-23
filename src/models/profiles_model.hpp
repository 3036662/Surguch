#ifndef PROFILES_MODEL_HPP
#define PROFILES_MODEL_HPP

#include <QAbstractListModel>
#include <QHash>
#include <QJsonArray>

class ProfilesModel : public QAbstractListModel {
  Q_OBJECT

  enum RoleNames { TitleRole = Qt::UserRole, ValueRole = Qt::UserRole + 1 };

public:
  explicit ProfilesModel(QObject *parent = nullptr);

  // Header:
  [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation,
                                    int role) const override;

  // Basic functionality:
  [[nodiscard]] int rowCount(const QModelIndex &parent) const override;

  [[nodiscard]] QVariant data(const QModelIndex &index,
                              int role) const override;

  [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

  Q_INVOKABLE QString getUserCertsJSON() const;

  Q_INVOKABLE bool saveProfile(QString profile_json);

signals:
  void profileSaved(QString);

private:
  /// @brief readProfiles from JSON file in
  /// /HOME/USER/.config/pdfcsp/profiles.json
  void readProfiles();

  /// @brief readUserCerts, read certificates for current uset from CryptoApi
  void readUserCerts();

  QString saveLogoImage(const QString &path, const QString &dest_name);

  QHash<int, QByteArray> role_names_;
  QJsonArray profiles_;
  QJsonArray user_certs_;
  QString config_path_;
  QString profiles_file_name_;
};

#endif // PROFILES_MODEL_HPP

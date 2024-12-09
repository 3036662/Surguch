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

  [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation,
                                    int role) const override;

  [[nodiscard]] int rowCount(const QModelIndex &parent) const override;

  [[nodiscard]] QVariant data(const QModelIndex &index,
                              int role) const override;

  [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

  ///@brief get a json array with user certificates
  [[nodiscard]] Q_INVOKABLE QString getUserCertsJSON() const;

  [[nodiscard]] Q_INVOKABLE bool saveProfile(const QString &profile_json);

  [[nodiscard]] Q_INVOKABLE QString getDetDefaultProfileVal();

  [[nodiscard]] Q_INVOKABLE bool deleteProfile(int id_profile);

  [[nodiscard]] Q_INVOKABLE bool uniqueName(QString profile_name);

  [[nodiscard]] Q_INVOKABLE QString getConfigPath() const;

  Q_PROPERTY(bool errStatus MEMBER error_status_)
  Q_PROPERTY(QString errString MEMBER err_string_)

signals:
  void profileSaved(QString);   // value of saved profile
  void profileDeleted(QString); // title of deleted profile
  void errNoCspLib();           // no cryptoPro lib error

private:
  /// @brief readProfiles from JSON file in
  /// /HOME/USER/.config/pdfcsp/profiles.json
  void readProfiles();

  /// @brief readUserCerts, read certificates for current uset from CryptoApi
  void readUserCerts();

  QString saveLogoImage(const QString &path, const QString &dest_name,
                        const QString &old_logo_path);

  bool deleteLogoImage(const QString &path);

  const QString create_profile_title_;
  QHash<int, QByteArray> role_names_;
  QJsonArray profiles_;
  QJsonArray user_certs_;
  QString config_path_;
  QString profiles_file_name_;

  bool error_status_ = false;
  QString err_string_;
};

#endif // PROFILES_MODEL_HPP

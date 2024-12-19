/* File: profiles_model.hpp
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

#ifndef PROFILES_MODEL_HPP
#define PROFILES_MODEL_HPP

#include <QAbstractListModel>
#include <QHash>
#include <QJsonArray>

/**
 * @brief Model for user profiles
 */
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

    /// @brief save user's profile
    [[nodiscard]] Q_INVOKABLE bool saveProfile(const QString &profile_json);

    /// @brief find the default profile and return it as JSON string
    [[nodiscard]] Q_INVOKABLE QString getDetDefaultProfileVal();

    /// @brief delete the user's profile
    [[nodiscard]] Q_INVOKABLE bool deleteProfile(int id_profile);

    /// @brief check if the given name is unique
    [[nodiscard]] Q_INVOKABLE bool uniqueName(QString profile_name);

    /// @brief path to config directory (/home/$user/.config)
    [[nodiscard]] Q_INVOKABLE QString getConfigPath() const;

    /// @brief TRUE if some global error ocurred
    /// @details for now, TRUE if no CSP library was found
    Q_PROPERTY(bool errStatus MEMBER error_status_)

    /// @brief error code string for errStatus
    Q_PROPERTY(QString errString MEMBER err_string_)

   signals:
    void profileSaved(QString);    // value of saved profile
    void profileDeleted(QString);  // title of deleted profile
    void errNoCspLib();            // no cryptoPro lib error

   private:
    /// @brief readProfiles from JSON file in
    /// @details /HOME/USER/.config/pdfcsp/profiles.json
    void readProfiles();

    /// @brief readUserCerts, read certificates for current uset from CryptoApi
    void readUserCerts();

    /**
     * @brief Save logo image
     *
     * @param path source image path
     * @param dest_name destanation file name
     * @param old_logo_path old logo to delete
     * @return QString full path to saved logo on success
     */
    QString saveLogoImage(const QString &path, const QString &dest_name,
                          const QString &old_logo_path);

    /// @brief delete the given file
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

#endif  // PROFILES_MODEL_HPP

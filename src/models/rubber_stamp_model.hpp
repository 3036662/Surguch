#ifndef RUBBER_STAMP_MODEL_HPP
#define RUBBER_STAMP_MODEL_HPP

#include <QAbstractListModel>
#include <QJsonArray>

/**
 * @brief Model for rubber stamps
 */
class RubberStampModel : public QAbstractListModel {
    Q_OBJECT

    enum RoleNames { TitleRole = Qt::UserRole, ValueRole = Qt::UserRole + 1 };

   public:
    explicit RubberStampModel(QObject* parent = nullptr);

    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation,
                                      int role) const override;

    [[nodiscard]] int rowCount(const QModelIndex& parent) const override;

    [[nodiscard]] QVariant data(const QModelIndex& index,
                                int role) const override;

    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    /// @brief get a json array with rubber stamps
    [[nodiscard]] Q_INVOKABLE QString getRubberStampsJSON() const;

    /// @brief save rubber stamp
    [[nodiscard]] Q_INVOKABLE bool saveRubberStamps(const QString& stamp_json);

    /// @brief delete rubber stamp
    [[nodiscard]] Q_INVOKABLE bool deleteRubberStamps(int id_stamp);

    /// @brief check if the given rubber stamp name is unique
    [[nodiscard]] Q_INVOKABLE bool uniqueStampName(QString stamp_name);

   signals:
    void rubberStampSaved(QString);    // value of saved stamp
    void rubberStampDeleted(QString);  // value of deleted stamp

   private:
    /// @brief generate config for basic rubber stamps
    [[nodiscard]] QString generateConfig() const;

    /// @brief readRubberStamps from JSON file in
    /// @details /HOME/USER/.config/pdfcsp/rubber_stamps.json
    void readRubberStamps();

    /**
     * @brief Save logo image
     *
     * @param path source image path
     * @param dest_name destination file name
     * @param old_logo_path old logo to delete
     * @return QString full path to saved logo on success
     */
    QString saveLogoImage(const QString& path, const QString& dest_name,
                          const QString& old_logo_path);

    const QString approve_stamp_title_;
    const QString decline_stamp_title_;
    const QString create_stamp_title_;
    QHash<int, QByteArray> role_names_;
    QJsonArray rubber_stamps_;
    QString config_path_;
    QString rubber_stamps_file_name_;
};

#endif  // RUBBER_STAMP_MODEL_HPP

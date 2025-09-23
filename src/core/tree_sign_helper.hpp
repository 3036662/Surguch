#ifndef TREE_SIGN_HELPER_HPP
#define TREE_SIGN_HELPER_HPP

#include <QObject>
#include <QVariant>
#include <c_bridge.hpp>

namespace core {

class TreeSignHelper {
   public:
    /// @brief utility structure for storing parameters for library
    struct CBatchSigSettingsWrapper {
        QByteArray qb_cert_serial;
        QByteArray qb_cert_subject;
        QByteArray qb_cades_type;
        QByteArray qb_tsp_link;
        QByteArray qb_sig_extension;
        QByteArray qb_dest_dir_path;
        pdfcsp::c_bridge::BatchSignatureSettings pod_settings;
    };

    struct SigSettings {
        QString cert_serial;
        QString cert_subject;
        QString cades_type;
        QString tsp_link;
        QString sig_extension;
        QString dest_dir_path;
        bool create_attached;
        bool create_base_64_encoded;
        bool pack_to_zip;
        bool pack_separate_zips;
    };

    using SharedSettingsWrapper = std::shared_ptr<CBatchSigSettingsWrapper>;

    explicit TreeSignHelper();

    void createSigSettings(const QVariantMap &qvparams);
    SharedSettingsWrapper createWrapper();

   private:
    SigSettings settings_;
};
}  // namespace core

#endif  // TREE_SIGN_HELPER_HPP

/* File: tree_sign_helper.hpp
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

#ifndef TREE_SIGN_HELPER_HPP
#define TREE_SIGN_HELPER_HPP

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
        bool create_attached = false;
        bool create_base_64_encoded = true;
        bool pack_to_zip = true;
        bool pack_separate_zips = false;
    };

    using SharedSettingsWrapper = std::shared_ptr<CBatchSigSettingsWrapper>;

    explicit TreeSignHelper() = default;

    void createSigSettings(const QVariantMap &qvparams);
    [[nodiscard]] SharedSettingsWrapper createWrapper() const;

   private:
    SigSettings settings_;
};
}  // namespace core

#endif  // TREE_SIGN_HELPER_HPP

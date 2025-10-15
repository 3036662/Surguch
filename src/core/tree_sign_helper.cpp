/* File: tree_sign_helper.cpp
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

#include "tree_sign_helper.hpp"

#include <QUrl>

namespace core {

void TreeSignHelper::createSigSettings(const QVariantMap &qvparams) {
    if (qvparams.contains("cert_serial")) {
        settings_.cert_serial = qvparams.value("cert_serial").toString();
    }
    if (qvparams.contains("cert_subject")) {
        settings_.cert_subject = qvparams.value("cert_subject").toString();
    }
    if (qvparams.contains("cades_type")) {
        settings_.cades_type = qvparams.value("cades_type").toString();
    }
    if (qvparams.contains("tsp_link")) {
        settings_.tsp_link = qvparams.value("tsp_link").toString();
    }
    if (qvparams.contains("sig_ext")) {
        settings_.sig_extension = qvparams.value("sig_ext").toString();
    }
    if (qvparams.contains("dest_dir_path")) {
        settings_.dest_dir_path =
            qvparams.value("dest_dir_path").toUrl().toLocalFile();
    }
    if (qvparams.contains("create_attached")) {
        settings_.create_attached = qvparams.value("create_attached").toBool();
    }
    if (qvparams.contains("create_base_64_encoded")) {
        settings_.create_base_64_encoded =
            qvparams.value("create_base_64_encoded").toBool();
    }
    if (qvparams.contains("pack_to_zip")) {
        settings_.pack_to_zip = qvparams.value("pack_to_zip").toBool();
    }
    if (qvparams.contains("pack_separate_zips")) {
        settings_.pack_separate_zips =
            qvparams.value("pack_separate_zips").toBool();
    }
}

TreeSignHelper::SharedSettingsWrapper TreeSignHelper::createWrapper() const {
    auto settings_wrapper = std::make_shared<CBatchSigSettingsWrapper>();
    pdfcsp::c_bridge::BatchSignatureSettings &pod_settings =
        settings_wrapper->pod_settings;
    settings_wrapper->qb_cert_serial = settings_.cert_serial.toUtf8();
    pod_settings.cert_serial = settings_wrapper->qb_cert_serial.data();
    settings_wrapper->qb_cert_subject = settings_.cert_subject.toUtf8();
    pod_settings.cert_subject = settings_wrapper->qb_cert_subject.data();
    settings_wrapper->qb_cades_type = settings_.cades_type.toUtf8();
    pod_settings.cades_type = settings_wrapper->qb_cades_type.data();
    settings_wrapper->qb_tsp_link = settings_.tsp_link.toUtf8();
    pod_settings.tsp_link = settings_wrapper->qb_tsp_link.data();
    settings_wrapper->qb_sig_extension = settings_.sig_extension.toUtf8();
    pod_settings.sig_extension = settings_wrapper->qb_sig_extension.data();
    settings_wrapper->qb_dest_dir_path = settings_.dest_dir_path.toUtf8();
    pod_settings.dest_dir_path = settings_wrapper->qb_dest_dir_path.data();
    pod_settings.create_attached = settings_.create_attached;
    pod_settings.create_base_64_encoded = settings_.create_base_64_encoded;
    pod_settings.pack_to_zip = settings_.pack_to_zip;
    pod_settings.pack_separate_zips = settings_.pack_separate_zips;
    return settings_wrapper;
}
}  // namespace core

/* File: bridge_utils.cpp
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

#include "bridge_utils.hpp"

#include "c_bridge.hpp"
// #include "pod_structs.hpp"
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTimeZone>

namespace core::bridge_utils {

QString timeToString(time_t val) noexcept {
    const QDateTime date_time =
        QDateTime::fromSecsSinceEpoch(val, QTimeZone(0));
    return date_time.toString("dd.MM.yyyy hh:mm:ss UTC");
}

/// @brief get list of certificates from CSP
QString getCertListJSON() noexcept {
    const pdfcsp::c_bridge::CPodParam params{};
    pdfcsp::c_bridge::CPodResult *const pod =
        pdfcsp::c_bridge::CGetCertList(params);
    QString res;
    if (pod != nullptr && pod->user_certifitate_list_json != nullptr &&
        pod->common_execution_status) {
        res = pod->user_certifitate_list_json;
    }
    if (pod != nullptr && !pod->common_execution_status) {
        qWarning() << "ERR STRING" << pod->err_string;
        res = kErrNoCSPLib;
    }
    if (pod == nullptr) {
        res = kErrGetCerts;
    }
    pdfcsp::c_bridge::CFreeResult(pod);
    return res;
}

}  // namespace core::bridge_utils

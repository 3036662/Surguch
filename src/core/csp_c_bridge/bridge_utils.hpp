#ifndef BRIGED_UTILS_HPP
#define BRIGED_UTILS_HPP
#include <QString>
#include <QJsonArray>

namespace core::bridge_utils {

QString timeToString(time_t val) noexcept;

// QJsonArray convertTimeInTspResult(const QString& tsp_info_json) noexcept;

// QJsonArray convertTimeInChain(const QJsonArray& chain_arr) noexcept;

// QJsonObject convertTimeInChainObject(const QJsonObject& chain_obj) noexcept;

// QJsonArray convertTimeInCertsArray(const QJsonArray& certs_arr) noexcept;

// QJsonObject convertTimeInCertObj(const QJsonObject& cert_obj) noexcept;

// QJsonObject convertTimeInTstObj(const QJsonObject& tst_obj) noexcept;

} // namespace core::bridge_utils

#endif // BRIGED_UTILS_HPP

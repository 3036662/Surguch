#ifndef BRIGED_UTILS_HPP
#define BRIGED_UTILS_HPP
#include <QJsonArray>
#include <QString>

namespace core::bridge_utils {

constexpr const char *const kErrNoCSPLib = "ERR_NO_CSP_LIB";
constexpr const char *const kErrGetCerts = "ERR_GET_CERTS";

QString timeToString(time_t val) noexcept;

QString getCertListJSON() noexcept;

} // namespace core::bridge_utils

#endif // BRIGED_UTILS_HPP

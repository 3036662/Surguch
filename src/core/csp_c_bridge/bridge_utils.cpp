#include "bridge_utils.hpp"
#include "c_bridge.hpp"
// #include "pod_structs.hpp"
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTimeZone>

namespace core::bridge_utils {

QString timeToString(time_t val) noexcept {
  const QDateTime date_time = QDateTime::fromSecsSinceEpoch(val, QTimeZone(0));
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

} // namespace core::bridge_utils

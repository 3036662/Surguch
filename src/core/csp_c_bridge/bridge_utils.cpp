#include "bridge_utils.hpp"
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTimeZone>

namespace core::bridge_utils {

QString timeToString(time_t val) noexcept {
  const QDateTime date_time = QDateTime::fromSecsSinceEpoch(val, QTimeZone(0));
  return date_time.toString("dd.MM.yyyy hh:mm:ss UTC");
}



} // namespace core::bridge_utils

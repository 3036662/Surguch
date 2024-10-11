#include "bridge_utils.hpp"
#include <QDateTime>
#include <QTimeZone>
#include <QJsonDocument>
#include <QJsonObject>

namespace core::bridge_utils {

QString timeToString(time_t val) noexcept{
    QDateTime date_time = QDateTime::fromSecsSinceEpoch(val,QTimeZone(0));
    return date_time.toString("dd.MM.yyyy hh:mm:ss UTC");
}

// QJsonArray convertTimeInTspResult(const QString& tsp_info_json) noexcept{
//   QJsonDocument tsp_info= QJsonDocument::fromJson(tsp_info_json.toUtf8());
//   if (tsp_info.isNull() || !tsp_info.isArray()){
//       return {};
//   }
//   const QJsonArray tsp_arr=tsp_info.array();
//   QJsonArray modified_tsp_arr;
//   for (const auto& tsp_val:tsp_arr){
//         if (tsp_val.isNull() || !tsp_val.isObject()){continue;}
//         QJsonObject tsp_obj=tsp_val.toObject();
//         QJsonObject fixed_tsp_obj;
//         fixed_tsp_obj["result"]=tsp_obj["result"];
//         if (tsp_obj.contains("chains") && tsp_obj["chains"].isArray()){
//             fixed_tsp_obj["chains"]=convertTimeInChain(tsp_obj["chains"].toArray());
//         }
//         if (tsp_obj.contains("tst_content") && tsp_obj["tst_content"].isObject()){
//             fixed_tsp_obj["tst_content"]=convertTimeInTstObj(tsp_obj["tst_content"].toObject());
//         }
//         modified_tsp_arr.append(tsp_obj);
//   }
//   return modified_tsp_arr;
// }

// QJsonArray convertTimeInChain(const QJsonArray& chain_arr) noexcept{
//       QJsonArray fixed_array;
//       for (const auto& chain:chain_arr){
//          if (chain.isArray()){
//             fixed_array.append(convertTimeInChain(chain.toArray()));
//             continue;
//          }
//          if (!chain.isObject()) {continue;}
//          const QJsonObject chain_obj=chain.toObject();
//          fixed_array.append(convertTimeInChainObject(chain_obj));
//       }
//       return fixed_array;
// }

// QJsonObject convertTimeInChainObject(const QJsonObject& chain_obj) noexcept{
//     QJsonObject fixed_chain_obj;
//     fixed_chain_obj["trust_status"]=chain_obj["trust_status"];
//     if (chain_obj.contains("certs") && chain_obj["certs"].isArray()){
//         fixed_chain_obj["certs"]=convertTimeInCertsArray(chain_obj["certs"].toArray());
//     }

//     return fixed_chain_obj;
// }

// QJsonArray convertTimeInCertsArray(const QJsonArray& certs_arr) noexcept{
//     QJsonArray fixed_certs_array;
//     for (const auto& cert_val:certs_arr){
//         if (!cert_val.isObject()){
//             continue;
//         }
//         fixed_certs_array.append(convertTimeInCertObj(cert_val.toObject()));
//     }
//     return fixed_certs_array;
// }

// QJsonObject convertTimeInCertObj(const QJsonObject& cert_obj) noexcept{
//     QJsonObject fixed_cert_obj=cert_obj;
//     fixed_cert_obj["not_before"]=timeToString(cert_obj["not_before"].toInteger());
//     fixed_cert_obj["not_after"]=timeToString(cert_obj["not_after"].toInteger());
//     return fixed_cert_obj;
// }

// QJsonObject convertTimeInTstObj(const QJsonObject& tst_obj) noexcept{
//     QJsonObject fixed_tst_obj=tst_obj;
//     fixed_tst_obj["gen_time"]=timeToString(tst_obj["gen_time"].toInteger());
//     return fixed_tst_obj;
// }



} //namespace core::bridge_utils

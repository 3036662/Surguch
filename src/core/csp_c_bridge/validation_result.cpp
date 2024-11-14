#include "validation_result.hpp"
#include "bridge_utils.hpp"
#include "raw_signature.hpp"
#include <QJsonArray>
#include <QJsonDocument>
#include <algorithm>

namespace core {

ValidationResult::ValidationResult(const core::RawSignature &raw_signature,
                         const std::string &path) {
  // create CPodParams
  pdfcsp::c_bridge::CPodParam params{};
  // Put the byteranges into the flat memory.
  std::vector<uint64_t> flat_ranges;
  std::for_each(raw_signature.getByteRanges().cbegin(),
                raw_signature.getByteRanges().cend(),
                [&flat_ranges](const auto &range_pair) {
                  flat_ranges.emplace_back(range_pair.first);
                  flat_ranges.emplace_back(range_pair.second);
                });
  params.byte_range_arr = flat_ranges.data();
  params.byte_ranges_size = flat_ranges.size();
  // raw signature
  params.raw_signature_data = raw_signature.getSigData().data();
  params.raw_signature_size = raw_signature.getSigData().size();
  // file path
  params.file_path = path.c_str();
  params.file_path_size = path.size() + 1;
  // call the library get CPodResult
  pdfcsp::c_bridge::CPodResult *const pod = CGetCheckResult(params);
  if (pod == nullptr || !pod->common_execution_status) {
    throw std::runtime_error("[CSPResponse] error getting pod result");
  }
  // create CSPResponse
  bres = pod->bres;
  cades_type = pod->cades_type;
  cades_t_str = QString(pod->cades_t_str);
  hashing_oid = QString(pod->hashing_oid);
  if (pod->encrypted_digest != nullptr && pod->encrypted_digest_size > 0) {
    std::copy(pod->encrypted_digest,
              pod->encrypted_digest + pod->encrypted_digest_size,
              std::back_inserter(encrypted_digest));
  }
  if (pod->times_collection != nullptr && pod->times_collection_size > 0) {
    std::copy(pod->times_collection,
              pod->times_collection + pod->times_collection_size,
              std::back_inserter(times_collection));
  }
  if (pod->x_times_collection != nullptr && pod->x_times_collection_size > 0) {
    std::copy(pod->x_times_collection,
              pod->x_times_collection + pod->times_collection_size,
              std::back_inserter(x_times_collection));
  }
  if (pod->cert_issuer_dname != nullptr) {
    cert_issuer_dname = QString(pod->cert_issuer_dname);
  }
  if (pod->cert_subject_dname != nullptr) {
    cert_subject_dname = QString(pod->cert_subject_dname);
  }
  if (pod->issuer_common_name != nullptr) {
    issuer_common_name = QString(pod->issuer_common_name);
  }
  if (pod->issuer_email != nullptr) {
    issuer_email = QString(pod->issuer_email);
  }
  if (pod->issuer_organization != nullptr) {
    issuer_organization = QString(pod->issuer_organization);
  }
  if (pod->subj_common_name != nullptr) {
    subj_common_name = QString(pod->subj_common_name);
  }
  if (pod->subj_email != nullptr) {
    subj_email = QString(pod->subj_email);
  }
  if (pod->subj_organization != nullptr) {
    subj_organization = QString(pod->subj_organization);
  }
  if (pod->cert_chain_json != nullptr) {
    cert_chain_json = QString(pod->cert_chain_json);
  }
  if (pod->tsp_json_info != nullptr) {
    tsp_info_json = QString(pod->tsp_json_info);
  }
  if (pod->signers_cert_ocsp_json_info != nullptr) {
    signers_cert_ocsp_json_info = QString(pod->signers_cert_ocsp_json_info);
  }

  if (pod->cert_public_key != nullptr && pod->cert_public_key_size > 0) {
    std::copy(pod->cert_public_key,
              pod->cert_public_key + pod->cert_public_key_size,
              std::back_inserter(cert_public_key));
  }
  if (pod->cert_serial != nullptr && pod->cert_serial_size > 0) {
    std::copy(pod->cert_serial, pod->cert_serial + pod->cert_serial_size,
              std::back_inserter(cert_serial));
  }
  signers_time = pod->signers_time;
  cert_not_before = pod->cert_not_before;
  cert_not_after = pod->cert_not_after;
  // Free CPodResult
  pdfcsp::c_bridge::CFreeResult(pod);
}

QJsonObject ValidationResult::toJson() const {
  // signature
  const char *ok = "ok";
  const char *bad = "bad";
  const char *no_field = "no_field";
  const char *no_check = "no_check";
  const char *failed = "failed";

  QJsonObject signature;
  signature["status"] = bres.check_summary;
  signature["integrity"] =
      (bres.data_hash_ok && bres.computed_hash_ok && bres.certificate_hash_ok)
          ? ok
          : bad;
  if (cades_type == pdfcsp::csp::CadesType::kPkcs7) {
    signature["integrity"] = bres.msg_signature_ok ? ok : bad;
  }
  signature["math_correct"] = bres.msg_signature_ok ? ok : bad;
  signature["certificate_ok"] = bres.certificate_ok ? ok : bad;
  if (cades_type == pdfcsp::csp::CadesType::kCadesT) {
    signature["timestamp_ok"] = bres.t_all_ok ? ok : bad;
  } else if (cades_type >= pdfcsp::csp::CadesType::kCadesXLong1) {
    signature["timestamp_ok"] = bres.t_all_ok && bres.x_esc_tsp_ok ? ok : bad;
  } else {
    signature["timestamp_ok"] = no_field;
  }
  if (bres.certificate_ocsp_check_failed && !bres.certificate_ocsp_ok) {
    signature["ocsp_ok"] = failed;
  } else {
    signature["ocsp_ok"] = bres.certificate_ocsp_ok ? ok : bad;
  }
  // signing time
  {
    time_t signing_time = 0;
    std::vector<time_t> tmp;
    std::copy(times_collection.cbegin(), times_collection.cend(),
              std::back_inserter(tmp));
    std::copy(x_times_collection.cbegin(), x_times_collection.cend(),
              std::back_inserter(tmp));
    auto max_el = std::max_element(tmp.cbegin(), tmp.cend());
    if (max_el != tmp.cend()) {
      signing_time = *max_el;
    } else {
      signing_time = signers_time;
    }
    if (signing_time != 0) {
      signature["signing_time"] = bridge_utils::timeToString(signing_time);
    } else {
      signature["signing_time"] = "?";
    }
  }
  // cades type
  signature["cades_type"] = cades_t_str;
  // result struct
  QJsonObject res;
  res["signature"] = signature;
  // signers sertificate chain
  const QJsonDocument json_chain = QJsonDocument::fromJson(cert_chain_json.toUtf8());
  if (json_chain.isNull()) {
    qDebug() << "Failed to create JSON doc.";
  } else {
    res["signers_chain"] = json_chain.array();
  }
  // TSP stamp
  if (cades_type >= pdfcsp::csp::CadesType::kCadesT) {
    const QJsonDocument tsp_info = QJsonDocument::fromJson(tsp_info_json.toUtf8());
    if (!tsp_info.isArray()) {
      qDebug() << "Failed to parse JSON TSP info";
    } else {
      res["tsp_info"] = tsp_info.array();
    }
  }
  if (!signers_cert_ocsp_json_info.isEmpty()) {
    const QJsonDocument ocsp_info =
        QJsonDocument::fromJson(signers_cert_ocsp_json_info.toUtf8());
    if (!ocsp_info.isArray()) {
      qDebug() << "Failed to parse JSON OCSP info";
    } else {
      res["ocsp_info"] = ocsp_info.array();
    }
  }

  // byterange analasys results
  res["full_coverage"]=full_coverage;
  res["can_be_casted_to_full_coverage"]=can_be_casted_to_full_coverage;
  return res;
}

} // namespace core

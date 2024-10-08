#include "csp_response.hpp"

#include "raw_signature.hpp"
#include <algorithm>

namespace core {

CSPResponse::CSPResponse(const core::RawSignature &raw_signature,
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
  if (pod == nullptr) {
    throw std::runtime_error("[CSPResponse] error getting pod result");
  }
  // create CSPResponse
  bres = pod->bres;
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

} // namespace core

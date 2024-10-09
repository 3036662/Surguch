#ifndef CSP_RESPONSE_HPP
#define CSP_RESPONSE_HPP

#include "c_bridge.hpp"
#include "raw_signature.hpp"
#include <QString>
#include <string>
#include <vector>
#include <QJsonObject>

namespace core {

struct CSPResponse {
public:
  CSPResponse(const core::RawSignature &raw_signature, const std::string &path);

  QJsonObject toJson() const;

  pdfcsp::csp::checks::BoolResults bres;
  pdfcsp::csp::CadesType cades_type = pdfcsp::csp::CadesType::kUnknown;
  QString cades_t_str;
  QString hashing_oid;
  std::vector<unsigned char> encrypted_digest;
  std::vector<time_t> times_collection;
  std::vector<time_t> x_times_collection;
  QString cert_issuer_dname;
  QString cert_subject_dname;

  QString issuer_common_name;
  QString issuer_email;
  QString issuer_organization;
  QString subj_common_name;
  QString subj_email;
  QString subj_organization;

  std::vector<unsigned char> cert_public_key;
  std::vector<unsigned char> cert_serial;

  time_t signers_time = 0;
  time_t cert_not_before = 0;
  time_t cert_not_after = 0;


};

} // namespace core




#endif // CSP_RESPONSE_HPP

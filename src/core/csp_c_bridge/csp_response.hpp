#ifndef CSP_RESPONSE_HPP
#define CSP_RESPONSE_HPP

#include "c_brigde/c_bridge.hpp"
#include "raw_signature.hpp"
#include <QString>
#include <string>
#include <vector>
namespace core {

class CSPResponse {
public:
  CSPResponse(const core::RawSignature &raw_signature, const std::string &path);

  pdfcsp::csp::checks::BoolResults bres;
  pdfcsp::csp::CadesType cades_type = pdfcsp::csp::CadesType::kUnknown;
  QString cades_t_str;
  QString hashing_oid;
  std::vector<unsigned char> encrypted_digest;
  std::vector<time_t> times_collection;
  std::vector<time_t> x_times_collection;
  QString cert_issuer_dname;
  QString cert_subject_dname;
  std::vector<unsigned char> cert_public_key;
  std::vector<unsigned char> cert_serial;

  time_t signers_time = 0;
  time_t cert_not_before = 0;
  time_t cert_not_after = 0;
};

} // namespace core
#endif // CSP_RESPONSE_HPP

/* File: validation_result.hpp
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

#ifndef CSP_RESPONSE_HPP
#define CSP_RESPONSE_HPP

#include "c_bridge.hpp"
#include "raw_signature.hpp"
#include <QJsonObject>
#include <QString>
#include <string>
#include <vector>

namespace core {

struct ValidationResult {
public:
  ValidationResult(const core::RawSignature &raw_signature,
                   const std::string &path);
  ValidationResult() = default;

  [[nodiscard]] QJsonObject toJson() const;

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
  QString cert_chain_json;
  QString tsp_info_json;
  QString signers_cert_ocsp_json_info;

  std::vector<unsigned char> cert_public_key;
  std::vector<unsigned char> cert_serial;

  time_t signers_time = 0;
  time_t cert_not_before = 0;
  time_t cert_not_after = 0;

  // byterange analasys results
  bool full_coverage = false;
  bool can_be_casted_to_full_coverage = false;
  std::optional<size_t> sig_curr_index;
  RangesVector byteranges;
  QString file_path;
};

} // namespace core

#endif // CSP_RESPONSE_HPP

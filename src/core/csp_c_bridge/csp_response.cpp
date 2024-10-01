#include "csp_response.hpp"

#include "c_brigde/c_bridge.hpp"
#include "raw_signature.hpp"
#include <algorithm>

namespace core{

CSPResponse::CSPResponse(const core::RawSignature& raw_signature,const std::string& path)
{
    // create CPodParams
    pdfcsp::c_bridge::CPodParam params{};
    // Put the byteranges into the flat memory.
    std::vector<uint64_t> flat_ranges;
    std::for_each(raw_signature.getByteRanges().cbegin(), raw_signature.getByteRanges().cend(),
                   [&flat_ranges](const auto &range_pair) {
                     flat_ranges.emplace_back(range_pair.first);
                     flat_ranges.emplace_back(range_pair.second);
                   });
    params.byte_range_arr=flat_ranges.data();
    params.byte_ranges_size=flat_ranges.size();
    // raw signature
    params.raw_signature_data=raw_signature.getSigData().data();
    params.raw_signature_size=raw_signature.getSigData().size();
    // file path
    params.file_path=path.c_str();
    params.file_path_size=path.size()+1;
    // call the library get CPodResult
    pdfcsp::c_bridge::CPodResult * const pod=CGetCheckResult(params);
    if (pod==nullptr){
        throw std::runtime_error("[CSPResponse] error getting pod result");
    }
    // create CSPResponse
    bres=pod->bres;
    cades_t_str=QString(pod->cades_t_str);
    hashing_oid=QString(pod->hashing_oid);
    std::copy(pod->encrypted_digest,pod->encrypted_digest+pod->encrypted_digest_size,std::back_inserter(encrypted_digest));
    std::copy(pod->times_collection,pod->times_collection+pod->times_collection_size,std::back_inserter(times_collection));
    std::copy(pod->x_times_collection,pod->x_times_collection+pod->times_collection_size,std::back_inserter(x_times_collection));
    cert_issuer_dname=QString(pod->cert_issuer_dname);
    cert_subject_dname=QString(pod->cert_subject_dname);
    std::copy(pod->cert_public_key,pod->cert_public_key+pod->cert_public_key_size,std::back_inserter(cert_public_key));
    std::copy(pod->cert_serial,pod->cert_serial+pod->cert_serial_size,std::back_inserter(cert_serial));
    signers_time=pod->signers_time;
    cert_not_before=pod->cert_not_before;
    cert_not_after=pod->cert_not_after;
    // Free CPodResult
    pdfcsp::c_bridge::CFreeResult(pod);
}

} // namespace core

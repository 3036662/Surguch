#include "signature_processor.hpp"
#include <stdexcept>

namespace core {

SignatureProcessor::SignatureProcessor(pdf_document* pdfdoc):pdfdoc_(pdfdoc){
  if (pdfdoc_==nullptr){
      throw std::runtime_error("[SignatureProcessor] document context is null");
  }
}

} //namespace core

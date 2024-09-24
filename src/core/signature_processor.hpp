#ifndef SIGNATURE_PROCESSOR_HPP
#define SIGNATURE_PROCESSOR_HPP

#include "mupdf/pdf.h"

namespace core{

/*!
 * \brief The SignatureProcessor class
 * \throws runtime_error on construct
 */
class SignatureProcessor
{
public:
    SignatureProcessor(pdf_document* pdfdoc);

private:
    pdf_document* pdfdoc_=nullptr;
};


}//namespace core

#endif // SIGNATURE_PROCESSOR_HPP

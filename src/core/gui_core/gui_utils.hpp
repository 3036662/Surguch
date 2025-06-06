#pragma once


#include "rubber_structs.hpp"
#include  "QVariant"
#include "QUrl"

namespace core::gui {

std::unique_ptr<BakeRubberResult> prepareImage(const SharedParamWrapper &params);

inline std::vector<unsigned char>* glueImageWithMask(
    const unsigned char* img, size_t img_size, const unsigned char* img_mask,
    size_t mask_size);

RubberParams prepareParams(const QVariantMap &qvparams);

SharedParamWrapper createParams(const RubberParams& params);

std::vector<pdfcsp::pdf::CAnnotParams> createAnnotParams(const std::vector<std::shared_ptr<RubberStamp>>& params);

}  // namespace core::gui


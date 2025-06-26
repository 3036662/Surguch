#pragma once

#include "QUrl"
#include "QVariant"
#include "rubber_structs.hpp"

namespace core::gui {

/// @brief concurrent function to make QImage
std::unique_ptr<BakeResult> prepareStampImage(
    const SharedSignParamWrapper& params);

/// @brief prepare preview params for later use
SignParams preparePreviewParams(const QVariantMap& qvparams);

/// @brief Gather all parameters (pdfcsp::pdf::CSignParam)
SharedSignParamWrapper createParams(const SignParams& params);

std::unique_ptr<BakeRubberResult> prepareImage(
    const SharedRubberParamWrapper& params);

inline std::vector<unsigned char>* glueImageWithMask(
    const unsigned char* img, size_t img_size, const unsigned char* img_mask,
    size_t mask_size);

RubberParams prepareParams(const QVariantMap& qvparams);

SharedRubberParamWrapper createParams(const RubberParams& params);

std::vector<pdfcsp::pdf::CAnnotParams> createAnnotParams(
    const std::vector<std::shared_ptr<RubberStamp>>& params);

}  // namespace core::gui

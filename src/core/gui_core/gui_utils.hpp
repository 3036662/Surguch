#pragma once

#include "QUrl"
#include "QVariant"
#include "rubber_structs.hpp"

namespace core::gui {

/// @brief concurrent function to make QImage for sign stamp
std::unique_ptr<BakeResult> prepareStampImage(
    const SharedSignParamWrapper& params);

/// @brief prepare sign stamp preview params for later use
SignParams preparePreviewParams(const QVariantMap& qvparams);

/// @brief Gather all sign stamp parameters (pdfcsp::pdf::CSignParam)
SharedSignParamWrapper createParams(const SignParams& params);

/// @brief concurrent function to make QImage for rubber stamp
std::unique_ptr<BakeRubberResult> prepareImage(
    const SharedRubberParamWrapper& params);

/// @brief prepare rubber stamp preview params for later use
RubberParams prepareParams(const QVariantMap& qvparams);

/// @brief Gather all rubber stamp parameters (pdfcsp::pdf::CAnnotParam)
SharedRubberParamWrapper createParams(const RubberParams& params);


/// @brief create rubber stamps params for embedding into pdf
std::vector<pdfcsp::pdf::CAnnotParams> createAnnotParams(
    const std::vector<std::shared_ptr<RubberStamp>>& params);

}  // namespace core::gui

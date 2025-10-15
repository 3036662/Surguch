/* File: gui_utils.hpp
Copyright (C) Basealt LLC,  2025
Author: Daniil-Viktor Ratkin, <ratkinda@basealt.ru>

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

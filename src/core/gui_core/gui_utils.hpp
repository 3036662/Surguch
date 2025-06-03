#pragma once


#include "rubber_structs.hpp"

namespace core::gui {

std::unique_ptr<BakeRubberResult> prepareImage(const SharedParamWrapper &params);

inline std::vector<unsigned char> glueImageWithMask(
    const unsigned char* img, size_t img_size, const unsigned char* img_mask,
    size_t mask_size);

}  // namespace core::gui


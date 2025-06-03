#include <QDebug>
#include <QImage>

#include "gui_utils.hpp"

namespace core::gui {

inline std::vector<unsigned char> glueImageWithMask(
    const unsigned char* img, size_t img_size, const unsigned char* img_mask,
    size_t mask_size) {
    if (img_size == 0 || img == nullptr) {
        return {};
    }
    std::vector<unsigned char> result;
    result.reserve(img_size + mask_size);
    for (size_t i = 0; i < img_size; ++i) {
        result.push_back(img[i]);
        if (i >= 2 && (i - 2) % 3 == 0) {
            const size_t mask_index = (i - 2) / 3;
            if (img_mask != nullptr && mask_index < mask_size) {
                // result.push_back(img_mask[mask_index] > 0 ? 0xff : 0x00);
                result.push_back(img_mask[mask_index]);
            } else {
                result.push_back(0xff);
            }
        }
    }
    return result;
}

std::unique_ptr<BakeRubberResult> prepareImage(
    const SharedParamWrapper &params) {
    auto result = std::make_unique<BakeRubberResult>(BakeRubberResult{
        std::unique_ptr<pdfcsp::pdf::BakeRubberStamResult,
                        void (*)(pdfcsp::pdf::BakeRubberStamResult *)>(
            pdfcsp::pdf::BakeRubberStamp(params->pod_params),
            pdfcsp::pdf::FreeRubberStampResult),
        std::unique_ptr<QImage>()});
    // qWarning() << "result pointer:" << result.get();
    if (result && result->data_ && result->data_->img != nullptr &&
        result->data_->img_size > 0) {
        result->image_ = std::make_unique<QImage>(result->data_->img,
            //glueImageWithMask(result->data_->img, result->data_->img_size,
                //result->data_->img_mask, result->data_->img_mask_size).data(),
                result->data_->resolution_x,
            result->data_->resolution_y, result->data_->resolution_x * 3,
            QImage::Format_RGB888);
        qWarning() << "resolution_x = " << result->data_->resolution_x;
        qWarning() << "resolution_y = " << result->data_->resolution_y;
        }
    return result;
}

}  //  namespace core::gui
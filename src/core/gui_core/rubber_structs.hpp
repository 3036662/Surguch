#ifndef RUBBER_STRUCTS_HPP
#define RUBBER_STRUCTS_HPP

#include <QImage>

#include <pdf_csp_c.hpp>

namespace core::gui {

struct RGBColor {
    quint8 R = 0;
    quint8 G = 0;
    quint8 B = 0;
};

// utility structure for storing parameters for library
struct CRubberParamsWrapper {
    QByteArray qb_img_path;
    QByteArray qb_annotation_text;
    QByteArray qb_font_family;
    pdfcsp::pdf::RubberStampParams pod_params;
};

using SharedParamWrapper = std::shared_ptr<CRubberParamsWrapper>;

struct RubberParams {
    bool bg_transparent = false;
    bool create_from_image = false;
    bool stamp_preserve_ratio = true;
    quint8 bg_opacity = 0;
    quint64 border_width = 10;
    quint32 border_radius = 10;
    quint64 stamp_width = 0;
    quint64 stamp_height = 0;
    quint64 annotation_width = 900;
    quint64 font_size = 0;
    quint64 font_weight = 0;
    QString img_path;
    QString annotation_text;
    QString font_family;
    RGBColor text_color;
    RGBColor border_color;
    RGBColor bg_color;
};

/// @brief structure for holding image data and constructed image for rendering
struct BakeRubberResult {
    std::unique_ptr<pdfcsp::pdf::BakeRubberStamResult,
                    void (*)(pdfcsp::pdf::BakeRubberStamResult *)>
        data_;
    std::unique_ptr<QImage> image_;
};

/// @brief structure for holding all needed for render data
struct RubberStamp {
    size_t page_index;
    double position_x;
    double position_y;
    double qml_width;
    double qml_height;
    std::unique_ptr<BakeRubberResult> res;
};


} // namespace core::gui

#endif //RUBBER_STRUCTS_HPP

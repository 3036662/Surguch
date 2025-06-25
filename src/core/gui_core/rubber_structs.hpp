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

/// @brief strucute for holding image data and contructed image for renderring
struct BakeResult {
    std::unique_ptr<pdfcsp::pdf::BakeSignatureStampResult,
                    void (*)(pdfcsp::pdf::BakeSignatureStampResult *)>
        data_;
    std::unique_ptr<QImage> image_;
};

// utility structure for storing parameters for library
struct CSignParamsWrapper {
    QByteArray qb_logo_path;
    QByteArray qb_config_path;
    QByteArray qb_cert_serial;
    QByteArray qb_cert_serial_prefix;
    QByteArray qb_cert_subject;
    QByteArray qb_cert_subject_prefix;
    QByteArray qb_cert_time_validity;
    QByteArray qb_stamp_type;
    QByteArray qb_cades_type;
    QByteArray qb_file_to_sign_path;
    QString temp_dir;
    QByteArray qb_temp_dir;
    QByteArray qb_stamp_title;
    QByteArray qb_tsp_url;
    pdfcsp::pdf::CSignParams pod_params;
};

using SharedSignParamWrapper = std::shared_ptr<CSignParamsWrapper>;

struct SignParams {
    bool bg_transparent = true;
    quint8 bg_opacity = 0;
    int page_index = 0;
    quint32 border_width = 10;
    quint32 border_radius = 10;
    qreal page_width = 0;
    qreal page_height = 0;
    qreal stamp_x = 0;
    qreal stamp_y = 0;
    qreal stamp_width = 0;
    qreal stamp_height = 0;
    QString logo_path;
    QString config_path;
    QString cert_serial;
    QString cert_subject;
    QString cert_time_validity;
    QString stamp_type;
    QString cades_type;
    QString file_to_sign_path;
    QString tsp_url;
    QString cert_serial_prefix;
    QString cert_subject_prefix;
    QString stamp_title;
    RGBColor text_color;
    RGBColor border_color;
};

// utility structure for storing parameters for library
struct CRubberParamsWrapper {
    QByteArray qb_img_path;
    QByteArray qb_annotation_text;
    QByteArray qb_font_family;
    QByteArray qb_link;
    pdfcsp::pdf::RubberStampParams pod_params;
};

using SharedRubberParamWrapper = std::shared_ptr<CRubberParamsWrapper>;

struct RubberParams {
    bool bg_transparent = false;
    bool create_from_image = false;
    bool stamp_preserve_ratio = true;
    int page_index = 0;
    double position_x = 0;
    double position_y = 0;
    double page_width = 0;
    double page_height = 0;
    quint8 bg_opacity = 0;
    quint64 border_width = 10;
    quint32 border_radius = 10;
    double stamp_width = 900;
    double stamp_height = 300;
    double real_stamp_width = 900;
    quint64 annotation_width = 900;
    quint64 font_size = 0;
    quint64 font_weight = 0;
    QString img_path;
    QString annotation_text;
    QString font_family;
    std::string link;
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
    int page_index;
    double position_x;
    double position_y;
    double qml_width;
    double real_stamp_qml_width;
    double qml_height;
    double stamp_width;
    double stamp_height;
    std::string link;
    std::unique_ptr<BakeRubberResult> res;
};

}  // namespace core::gui

#endif  // RUBBER_STRUCTS_HPP

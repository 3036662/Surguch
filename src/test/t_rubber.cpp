#include "t_rubber.hpp"

#include <QFile>
#include <QFuture>
#include <QFutureWatcher>
#include <QImage>
#include <QJsonDocument>
#include <QJsonObject>
#include <QQuickItem>
#include <QSignalSpy>
#include <QStandardPaths>
#include <QTest>
#include <QVariant>
#include <fstream>

#include "core/csp_c_bridge/bridge_utils.hpp"
#include "cpp_views/rubber_preview_render.hpp"
#include "models/rubber_stamp_model.hpp"
#include "pdf_csp_c.hpp"

void SavePPM(const unsigned char *data, size_t data_size, size_t width,
             size_t height, const std::string &dest, bool gray) {
    std::ofstream file(dest, std::ios_base::binary);
    assert(file.is_open());
    file << (gray ? "P5\n" : "P6\n");
    file << width << " " << height << " " << 255 << "\n";
    if (data != nullptr && data_size > 0) {
        file.write(reinterpret_cast<const char *>(data), data_size);
    }
    file.close();
}

TRubber::TRubber(QObject *parent) : QObject{parent} {}

void TRubber::createImages(){
    pdfcsp::pdf::RubberStampParams params{};
    params.annotation_text = "ANNOTATION TEXT";
    params.create_from_image = false;
    params.bg_color = pdfcsp::pdf::RGBColor{0xff, 0xff, 0xff};
    params.font_color = pdfcsp::pdf::RGBColor{0x00, 0x00, 0xff};
    params.border_color = pdfcsp::pdf::RGBColor{0x00, 0x00, 0xff};
    params.border_radius = 50;
    params.bg_opacity = 0xff;
    params.bg_transparent = false;
    for (params.annotation_width = 200; params.annotation_width < 1000; params.annotation_width += 10) {
        auto *result_ = BakeRubberStamp(params);
        QVERIFY(result_ != nullptr);
        QVERIFY(result_->img_size > 0);
        QVERIFY(result_->img_mask == nullptr);
        QVERIFY(result_->img_mask_size == 0);

        const std::string dest = test_files_dir_.toStdString() + "testBake" + std::to_string(params.annotation_width) + ".ppm";

        if (result_->resolution_x > 400) {
            auto image_ = std::make_unique<QImage>(
            result_->img, result_->resolution_x,
            result_->resolution_y, result_->resolution_x * 3,
            QImage::Format_RGB888);
            auto image = image_->scaled(400,
                400 * (static_cast<double>(result_->resolution_y / static_cast<double>(result_->resolution_x))) ,
                Qt::KeepAspectRatio);
            QVERIFY(image.save(dest.c_str()));
            FreeRubberStampResult(result_);
            continue;
        }

        if (result_->resolution_y > 400) {
            auto image_ = std::make_unique<QImage>(
            result_->img, result_->resolution_x,
            result_->resolution_y, result_->resolution_x * 3,
            QImage::Format_RGB888);
            auto image = image_->scaled(
                400 * (static_cast<double>(result_->resolution_x) / static_cast<double>(result_->resolution_y)),
            400 ,
            Qt::KeepAspectRatio);
            QVERIFY(image.save(dest.c_str()));
            FreeRubberStampResult(result_);
            continue;
        }

        if (result_->resolution_x <= 400 && result_->resolution_y <= 400) {
            auto image_ = std::make_unique<QImage>(
            result_->img, result_->resolution_x,
            result_->resolution_y, result_->resolution_x * 3,
            QImage::Format_RGB888);
            QVERIFY(image_->save(dest.c_str()));
            FreeRubberStampResult(result_);
            continue;
        }
        FreeRubberStampResult(result_);
    }

    for (params.annotation_width = 200; params.annotation_width < 1000; params.annotation_width += 10) {
        for (params.border_width = 0; params.border_width < 10; params.border_width += 1) {
            auto *result_ = BakeRubberStamp(params);
            QVERIFY(result_ != nullptr);
            QVERIFY(result_->img_size > 0);
            QVERIFY(result_->img_mask == nullptr);
            QVERIFY(result_->img_mask_size == 0);

            const std::string dest = test_files_dir_.toStdString() + std::to_string(params.border_width) + "testBake" + std::to_string(params.annotation_width) + ".ppm";

            if (result_->resolution_x > 400) {
                auto image_ = std::make_unique<QImage>(
                result_->img, result_->resolution_x,
                result_->resolution_y, result_->resolution_x * 3,
                QImage::Format_RGB888);
                auto image = image_->scaled(400,
                    400 * (static_cast<double>(result_->resolution_y / static_cast<double>(result_->resolution_x))) ,
                    Qt::KeepAspectRatio);
                QVERIFY(image.save(dest.c_str()));
                FreeRubberStampResult(result_);
                continue;
            }

            if (result_->resolution_y > 400) {
                auto image_ = std::make_unique<QImage>(
                result_->img, result_->resolution_x,
                result_->resolution_y, result_->resolution_x * 3,
                QImage::Format_RGB888);
                auto image = image_->scaled(
                    400 * (static_cast<double>(result_->resolution_x) / static_cast<double>(result_->resolution_y)),
                400 ,
                Qt::KeepAspectRatio);
                QVERIFY(image.save(dest.c_str()));
                FreeRubberStampResult(result_);
                continue;
            }

            if (result_->resolution_x <= 400 && result_->resolution_y <= 400) {
                auto image_ = std::make_unique<QImage>(
                result_->img, result_->resolution_x,
                result_->resolution_y, result_->resolution_x * 3,
                QImage::Format_RGB888);
                QVERIFY(image_->save(dest.c_str()));
                FreeRubberStampResult(result_);
                continue;
            }
            FreeRubberStampResult(result_);
        }
    }
}

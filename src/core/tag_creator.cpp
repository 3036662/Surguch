#include "tag_creator.hpp"

#include <QDebug>
#include <QStandardPaths>
#include <memory>

namespace core {

TagCreator::TagCreator(QObject *parent) : QObject{parent} {}

/// @brief create rubber stamps params for embedding into pdf
QString TagCreator::embedAnnot(
    const std::vector<pdfcsp::pdf::CAnnotParams> &params,
    const QString &file_path) {
    if (!params.empty()) {
        auto qb_tmp_path =
            QStandardPaths::writableLocation(QStandardPaths::TempLocation)
                .toUtf8();
        auto qb_file_path = file_path.toUtf8();
        std::unique_ptr<pdfcsp::pdf::CEmbedAnnotResult,
                        decltype(&pdfcsp::pdf::CFreeEmbedAnnotResult)>
            res(pdfcsp::pdf::PerformAnnotEmbedding(params.data(), params.size(),
                                                   qb_tmp_path, qb_file_path),
                &pdfcsp::pdf::CFreeEmbedAnnotResult);
        if (res != nullptr && res->status) {
            return res->tmp_file_path;
        }
    }
    return {};
}

}  //  namespace core
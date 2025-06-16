#include "tag_creator.hpp"

#include <QStandardPaths>
#include <QDebug>

#include <memory>

namespace core {

TagCreator::TagCreator(QObject *parent) : QObject{parent} {}

QString TagCreator::embedAnnot(const std::vector<pdfcsp::pdf::CAnnotParams> &params, const QString& file_path){
    if (!params.empty()) {
        qWarning() << "start embedding annots";
        std::unique_ptr<pdfcsp::pdf::CEmbedAnnotResult, decltype(&pdfcsp::pdf::CFreeEmbedAnnotResult)> res(
           pdfcsp::pdf::PerfomAnnotEmbeddign(
               params.data(), params.size(),
               QStandardPaths::writableLocation(QStandardPaths::TempLocation).toUtf8(),
               file_path.toUtf8()), &pdfcsp::pdf::CFreeEmbedAnnotResult);
        if (res != nullptr && res->status) {
            return res->tmp_file_path;
        }
    }
    return {};
}


}  //  namespace core
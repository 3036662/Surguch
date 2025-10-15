/* File: tag_creator.cpp
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

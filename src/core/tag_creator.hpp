/* File: tag_creator.hpp
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

#ifndef TAG_CREATOR_HPP
#define TAG_CREATOR_HPP

#include <QObject>
#include <pdf_csp_c.hpp>

namespace core {

/**
 * @brief Create a tag  stamp
 */
class TagCreator : public QObject {
    Q_OBJECT
   public:
    explicit TagCreator(QObject *parent = nullptr);

    /// @brief embed annotations into pdf
    static Q_INVOKABLE QString
    embedAnnot(const std::vector<pdfcsp::pdf::CAnnotParams> &params,
               const QString &file_path);
};

}  //  namespace core

#endif  // TAG_CREATOR_HPP

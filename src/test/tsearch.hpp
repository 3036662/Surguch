/* File: tsearch.hpp
Copyright (C) Basealt LLC,  2025
Author: Oleg Proskurin, <proskurinov@basealt.ru>

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

#ifndef TSEARCH_HPP
#define TSEARCH_HPP

#include <QObject>
#include <QString>

#include "mupdf/fitz.h"

class TSearch : public QObject {
    Q_OBJECT
   public:
    explicit TSearch(QObject *parent = nullptr);

   private slots:
    void BaseTest();
    void CacheText();
    void SearchTest1();
    void TextExtractorClass();

   private:
    const std::string test_files_dir_ = TEST_FILES_DIR;
};

#endif  // TSEARCH_HPP

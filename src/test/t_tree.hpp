/* File: t_tree.hpp
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

#pragma once

#include <QObject>

// @brief tests if backand send us null everywhere and if json we get is broken
class TTree : public QObject {
    Q_OBJECT
   public:
    explicit TTree(QObject *parent = nullptr);

   signals:

   private slots:

    void testEmpty();

   private:
    const QString test_files_dir_ = SENSITIVE_DIR;
    const QString test_json_file = test_files_dir_ + "test_data.txt";
    const QString test_broken_json_file = test_files_dir_ + "broken_data.txt";
    const QString file1_ = test_files_dir_ + "1.zip";
    const QString file2_ = test_files_dir_ + "2.zip";
    const QString file3_ = test_files_dir_ + "3.zip";
};

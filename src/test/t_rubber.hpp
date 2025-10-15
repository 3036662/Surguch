/* File: t_rubber.hpp
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

#ifndef T_RUBBER_HPP
#define T_RUBBER_HPP

#include <QDir>
#include <QObject>

class TRubber : public QObject {
    Q_OBJECT
   public:
    explicit TRubber(QObject *parent = nullptr);

   signals:
   private slots:

    void createRubber1();

    void createRubber2();

    void createRubber3();

   private:
    const QString test_files_dir_ = TEST_FILES_DIR;
    const QString file1_ = test_files_dir_ + "13_cam_CADES-XLT1_1sig.pdf";
    const QString file2_ = test_files_dir_ + "14_acrob_CADES-XLT1.pdf";
    const QString file3_ = test_files_dir_ + "21_cam_CADES-XLT1_5signs.pdf";
    const QString logo_ = test_files_dir_ + "profile_1_logo.jpg";
    const QString tag_ = test_files_dir_ + "tag_5_logo.jpg";
    const QString home_dir = TEST_DIR;
    const QString config_dir_ = home_dir + "/.config/csppdf";
    const QString profile_file_ = QString(TEST_FILES_DIR) + "/profiles.json";
    const QString stamps_file_ = QString(TEST_FILES_DIR) + "/stamps.json";
    const QString rubber_stamps_file_ =
        QString(TEST_FILES_DIR) + "/rubber_stamps.json";
};

#endif  // T_RUBBER_HPP

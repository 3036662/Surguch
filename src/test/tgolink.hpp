/* File: tgolink.hpp
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

#ifndef TGOLINK_HPP
#define TGOLINK_HPP

#include <QObject>

class TGolink : public QObject {
    Q_OBJECT
   public:
    explicit TGolink(QObject *parent = nullptr);

   private slots:
    void BaseTest();
    void ExtractAllUriPage();
    void ExtractAllUriPageTest1();
    void CacheUri();
    void RemoveAllCoveredUri();
    void RemoveAllCoveredUriTest();
    void FindAllUriPage();

    void ExtractAllUriPageTest1_data();
    void CacheUri_data();
    void RemoveAllCoveredUriTest_data();

   private:
    const std::string test_files_dir_ = TEST_FILES_DIR;
};

#endif  // TGOLINK_HPP

/* File: font_helper.hpp
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

#pragma once

#include <QFontDatabase>
#include <QObject>
#include <QStringList>

namespace core {

/// @brief utility class to get list of fonts which support Cyrillic
class FontHelper : public QObject {
    Q_OBJECT
   public:
    explicit FontHelper(QObject *parent = nullptr) : QObject(parent) {}

    static Q_INVOKABLE QStringList cyrillicFamilies() {
        return QFontDatabase::families(QFontDatabase::Cyrillic);
    }
};

}  // namespace core

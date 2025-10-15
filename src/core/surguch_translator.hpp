/* File: surguch_translator.hpp
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

#ifndef SURGUCH_TRANSLATOR_HPP
#define SURGUCH_TRANSLATOR_HPP

#include <QObject>

namespace core {

class SurguchTranslator : public QObject {
    Q_OBJECT
   public:
    explicit SurguchTranslator(QObject *parent = nullptr);

    [[nodiscard]] static Q_INVOKABLE QString
    surguchTranslate(const QString &str);
};

}  // namespace core

#endif  // SURGUCH_TRANSLATOR_HPP

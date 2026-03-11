/* File: surguch_launcher.cpp
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

#include "surguch_launcher.hpp"

#include <QDebug>
#include <QProcess>
#include <QUrl>

namespace core {

constexpr auto surguch_executable_ = "surguch";

SurguchLauncher::SurguchLauncher(QObject *parent) : QObject{parent} {}

void SurguchLauncher::launchSurguch(const QUrl &file) {
    QProcess::startDetached(surguch_executable_, {file.toLocalFile()});
}

}  // namespace core

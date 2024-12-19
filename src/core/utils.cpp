/* File: utils.cpp
Copyright (C) Basealt LLC,  2024
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

#include "utils.hpp"
#include <QDebug>

namespace core::utils {

/**
 * @brief Read a PDF HEX string to simple vector
 * @param str pointer to data
 * @param size size of the data
 * @return std::vector<unsigned char>
 */
std::vector<unsigned char> hexStringToByteArray(const char *str,
                                                size_t size) noexcept {
  std::vector<unsigned char> res;
  if (size == 0 || str == nullptr) {
    return res;
  }
  std::string copy(str, str + size);
  if (copy.empty()) {
    return res;
  }
  if (copy.front() == '<') {
    copy.erase(copy.begin());
  }
  if (copy.back() == '>') {
    copy.pop_back();
  }
  if (copy.size() % 2 != 0) {
    copy.push_back('0');
  }
  for (size_t i = 0; i < copy.size(); i += 2) {
    const std::string tmp = copy.substr(i, 2);
    size_t pos = 0;
    try {
      int val = std::stoi(tmp, &pos, 16);
      if (pos != tmp.size()) {
        throw std::runtime_error("parse error");
      }
      res.push_back(static_cast<uint8_t>(val));
    } catch (const std::exception &ex) {
      qWarning() << "[hexStringToByteArray] error parsing hexstring "
                 << ex.what();
      return {};
    }
  }
  return res;
}

} // namespace core::utils

/* File: utils.hpp
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

#ifndef UTILS_HPP
#define UTILS_HPP
#include <cstddef>
#include <vector>


namespace core::utils {

/**
 * @brief Read a PDF HEXstring to simple vector
 *
 * @param str pointer to data
 * @param size size of the data
 * @return std::vector<unsigned char>
 */
std::vector<unsigned char> hexStringToByteArray(const char *str,
                                                size_t size) noexcept;

}  // namespace core::utils

#endif  // UTILS_HPP

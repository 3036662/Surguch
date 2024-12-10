#ifndef UTILS_HPP
#define UTILS_HPP
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

} // namespace core::utils

#endif // UTILS_HPP

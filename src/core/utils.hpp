#ifndef UTILS_HPP
#define UTILS_HPP
#include <vector>

namespace core::utils {

std::vector<unsigned char> hexStringToByteArray(const char *str,
                                                size_t size) noexcept;

} // namespace core::utils

#endif // UTILS_HPP

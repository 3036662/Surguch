#include "utils.hpp"
#include <QDebug>
#include <iostream>

namespace core::utils {

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

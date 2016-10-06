#ifndef SOFTWARE_RENDERER_UTILS_H
#define SOFTWARE_RENDERER_UTILS_H

#include <string>
#include <vector>

template <class T>
constexpr const T& clamp(const T &v, const T &low, const T &hi) {
  return std::max(low, std::min(hi, v));
}

std::vector<std::string> split(const std::string &s, char delim);

#endif
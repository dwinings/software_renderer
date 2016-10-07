#ifndef SOFTWARE_RENDERER_UTILS_H
#define SOFTWARE_RENDERER_UTILS_H

#include <string>
#include <vector>
#include <streambuf>
#include <istream>

void dump_stream(std::istream &stream, std::string path);

struct vector_streambuf : std::streambuf {
  vector_streambuf(std::vector<uint8_t> &vec);
};

template <class T>
constexpr const T& clamp(const T &v, const T &low, const T &hi) {
  return std::max(low, std::min(hi, v));
}

std::vector<std::string> split(const std::string &s, char delim);

#endif
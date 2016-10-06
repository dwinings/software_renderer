#include "Utils.h"

#include <string>
#include <sstream>
#include <vector>
#include <algorithm>

vector_streambuf::vector_streambuf(std::vector<uint8_t> &vec) {
  setg((char*)(&vec[0]), (char*)(&vec[0]), (char*)(&vec[0]+vec.size()));
}

static void split(const std::string &s, char delim, std::vector<std::string> &elems) {
  std::stringstream stream;
  stream.str(s);
  std::string item;
  while (std::getline(stream, item, delim)) {
    elems.push_back(item);
  }
}

std::vector<std::string> split(const std::string &s, char delim) {
  std::vector<std::string> elems;
  split(s, delim, elems);
  return elems;
}
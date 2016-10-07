#include "Utils.h"

#include "Model.h"
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <fstream>

void dump_stream(std::istream &stream, std::string path) {
  std::ofstream output;
  int size, pos;
  char* buffer;

  output.open(path);
  pos = stream.tellg();
  stream.seekg(0, std::ios::end);
  size = stream.tellg();
  stream.seekg(0, std::ios::beg);
  buffer = new char[size];

  stream.read(buffer, size);
  output.write(buffer, size);
  output.close();
  stream.seekg(pos, std::ios::beg);
  delete[] buffer;
}

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
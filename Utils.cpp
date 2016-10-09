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

Vector3f Barycentric(const Vector3f &a, const Vector3f &b, const Vector3f &c, const Vector3f &point) {
  // We get these by solving for P = (1 - u - v)A + uAB + vAC
  // 0 = (1 - u - v)PA + uAB + vAC
  // actually taking the cross product of the x and y vectors of AC, AB and PA is enough.
  Vector3f xCoords;
  xCoords <<
      c.x() - a.x(), // AC
      b.x() - a.x(), // AB
      a.x() - point.x(); // PA

  Vector3f yCoords;
  yCoords <<
      c.y() - a.y(),
      b.y() - a.y(),
      a.y() - point.y();

  // (u, v, area)
  Vector3f orthoVector = xCoords.cross(yCoords);

  // Actually the area of the parallelogram, not the triangle.
  float double_area = orthoVector.z();

  if (std::abs(double_area) < 1e-2) {return Vector3f(-1.0f, 1.0f, 1.0f);}

  return Vector3f(
      1.0f - (orthoVector.x() + orthoVector.y())/double_area, // (1 - u - v)
      orthoVector.y() / double_area, // v
      orthoVector.x() / double_area  // u
  );
}
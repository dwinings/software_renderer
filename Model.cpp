#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "Model.h"
#include "Color.h"

// This weird syntax initializes vertices and faces
// It's called an "Initializer List"
Model::Model(const char *filename) : vertices(), faces(), textures(), normals(), texture_image() {
  std::ifstream file_stream;
  file_stream.open(MODELS_DIR + std::string(filename), std::ifstream::in);
  if (file_stream.fail()) return;

  // This var refers to our current input line.
  std::string line;
  while (!file_stream.eof()) {
    // We've now opened a stream to the file, so we need a line reader
    std::getline(file_stream, line);

    // This thing is turning our line string object into a "stream" that
    // we can use C++'s janky stream syntax on.
    std::istringstream line_stream(line.c_str());
    std::string tag;

    // Lol it's /dev/null
    char trash;

    // Read the tag char on the line.
    line_stream >> tag;
    line_stream >> std::ws;
    if (tag == "v") {

      Vector3f vertex;
      // If it's a vector, we trash tag element and put the other 3 in a vector3.
      for (int i=0;i<3;i++) line_stream >> vertex.raw[i];
      vertices.push_back(vertex);

    } else if (tag == "f") {
      std::vector<Vector3i> face;
      int32_t idx, tex, normal;

      // Lines come in like x/y/z, this stores x in the idx variable, and
      // discards the other two values (using trash and itrash to tell
      // it to trash chars and ints specifically).
      //
      // What a gross syntax.
      while (line_stream >> idx >> trash >> tex >> trash >> normal) {
        idx--; // in wavefront obj all indices start at 1, not zero
        tex--;
        normal--;
        face.push_back(Vector3i(idx, tex, normal));
      }
      faces.push_back(face);
    } else if (tag == "vt") {
      float u, v, ftrash;
      line_stream >> u >> v >> ftrash;
      textures.push_back(Vector2f(u, v));
    } else if (tag == "vn") {
      float x, y , z;
      line_stream >> x >> y >> z;
      normals.push_back(Vector3f(x, y, z));
    }
  }
#ifdef DEBUG
  std::cerr << "Model Counts -- \n  v: " << vertices.size() << " f: "  << faces.size() << " vt: " << textures.size() << " vn: " << normals.size() << std::endl;
#endif
}

Model::~Model() {
}

bool Model::load_texture(std::string path) {
  bool result = texture_image.read_tga_file((MODELS_DIR + path).c_str());
  if (!result) {
    std::cerr << "Failed to read head texture." << std::endl;
    return false;
  }
  texture_image.flip_vertically();
  return true;
}

Color Model::diffuse_color(float u, float v) const {
  return Color(texture_image.get(
      (uint32_t) (u * texture_image.get_width()),
      (uint32_t) (v * texture_image.get_height())
  ));
}

int Model::vertex_count() {
  return (int)vertices.size();
}

int Model::face_count() {
  return (int)faces.size();
}

int Model::tex_count() {
  return (int)textures.size();
}

int Model::normals_count() {
  return (int)normals.size();
}

std::vector<Vector3i> Model::face(int idx) {
  return faces[idx];
}

Vector3f Model::vertex(int idx) {
  return vertices[idx];
}

Vector2f Model::texture(int idx) {
  return textures[idx];
}

Vector3f Model::normal(int idx) {
  return normals[idx];
}

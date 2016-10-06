#include "Model.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "Color.h"
#include "Utils.h"

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
      for (int i=0;i<3;i++) line_stream >> vertex[i];
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

bool Model::load_image(std::string path, TGAImage &dst) {
  uint32_t chunk_size = 1024 * 256;
  bool result = false;
  std::string basename, ext;
  auto parts = split(path, '.');
  basename = parts[0];
  ext = parts[1];

  if (ext == "gz") {
    // Do zip stuff in the future.

    // Panic badly.
    assert(false);
    /*
    auto file = fopen(path.c_str(), "r");
    fseek(file, 0, SEEK_END);
    auto file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    if (file < 0) {
      std::cerr << "Error opening file (I think).\n";
      return false;
    }
    // Not planning to decompress a large file really.
    uint8_t *compressed = new uint8_t[file_size];
    uint8_t *decompressed = new uint8_t[chunk_size];
    std::ostringstream uncompressed_data;
    z_stream decomp_stream;
    decomp_stream.zalloc = Z_NULL;
    decomp_stream.zfree = Z_NULL;
    decomp_stream.opaque = Z_NULL;
    decomp_stream.avail_in = 0;
    decomp_stream.next_in = Z_NULL;
    auto ret = inflateInit(&decomp_stream);
    if (ret != Z_OK) {
      std::cerr << "Failed to read zip.\n";
    }

    decomp_stream.avail_in = fread(compressed, 1, file_size, file);
    if (ferror(file)) {
      std::cerr << "Error reading zip.\n";
    }
    if (decomp_stream.avail_in == 0) {
      std::cerr << "Successfully read zip.\n";
    }
    decomp_stream.next_in = compressed;

    do {
      decomp_stream.avail_out = file_size;
      decomp_stream.next_out = decompressed;
      ret = inflate(&decomp_stream, Z_NO_FLUSH);
      uncompressed_data << decompressed;
      assert(ret != Z_STREAM_ERROR);
    } while (decomp_stream.avail_out == 0);


    fclose(file);
    delete compressed;

    auto uncompressed_istream = std::istringstream(uncompressed_data.str());
    dst.read_tga_data(uncompressed_istream);
    */
  }
  else {
    result = dst.read_tga_file((MODELS_DIR + path).c_str());
  }

  if (result) {
    dst.flip_vertically();
  }
  return result;
}

bool Model::load_texture(std::string path) {
  return load_image(path, texture_image);
}

bool Model::load_normal_texture(std::string path) {
  return load_image(path, normal_image);
}

Color Model::diffuse_color(const Vector2f &uv) const {
  return Color(texture_image.get(
      (uint32_t) (uv[0] * texture_image.get_width()),
      (uint32_t) (uv[1] * texture_image.get_height())
  ));
}

Vector3f Model::normal(Vector2f &uv) const {
  TGAColor col = normal_image.get(uv[0] * normal_image.get_width(), uv[1] * normal_image.get_height());
  Vector3f result;
  for (int i = 0; i < 3; i++) {
    result[2 - i] = (float)col[i] / (255.0f * 2.0f) - 1.0f;
  }
  return (-1 * result.normalized());
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

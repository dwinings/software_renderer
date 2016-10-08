#include "Model.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "Color.h"
#include "Utils.h"
#include <zlib.h>

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
  uint32_t        uncompressed_size = 0;
  const uint32_t  chunk_size = 1024 * 256;
  bool            load_successful = true;
  std::string     basename, ext;

  // Find the file extension and decompress based on that.
  uint32_t pivot = path.rfind(".", path.length());
  basename = path.substr(0, pivot);
  ext = path.substr(pivot, path.length() - pivot);

  if (ext == ".gz") {
    FILE*                 file;
    uint32_t              file_size;
    int32_t               zlib_result;
    uint8_t*              zlib_input = 0;
    uint8_t               zlib_output[chunk_size];
    std::stringstream*    decompressed = new std::stringstream;
    z_stream              zstream;

    /////////// Read the file into 'compressed'
    file = fopen((MODELS_DIR + path).c_str(), "r");
    if (!file || ferror(file)) {
      std::cerr << "Error opening file.\n";
      load_successful = false;
      goto cleanup;
    }
    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    rewind(file);
    zlib_input = new uint8_t[file_size];
    fread(zlib_input, 1, file_size, file);
    fclose(file);
    /////////////////////////////////////////////////////

    /////////// Prep zlib for decompression loop
    zstream.zalloc = Z_NULL;
    zstream.zfree = Z_NULL;
    zstream.opaque = Z_NULL;
    zstream.next_in = zlib_input;
    zstream.avail_in = file_size;

    zlib_result = inflateInit2(&zstream, 16 + MAX_WBITS);
    if (zlib_result != Z_OK) {
      std::cerr << "Failed to read zip.\n";
      load_successful = false;
      goto cleanup;
    }

    if (ferror(file)) {
      std::cerr << "Error reading gzip.\n";
      load_successful = false;
      goto cleanup;
    }

    if (zstream.avail_in == file_size) {
      std::cerr << "successfully read valid gzip.\n";
    }

    zstream.next_in = zlib_input;
    //////////////////////////////////////////////////////

    ////////////// Main decompression spin loop.
    do {
      zstream.avail_out = chunk_size;
      zstream.next_out = zlib_output;
      zlib_result = inflate(&zstream, Z_NO_FLUSH);
      if (zlib_result == Z_MEM_ERROR) {
        std::cerr << "Ran out of memory decompressing. Load failed.";
        load_successful = false;
        goto cleanup;
      }
      if (zlib_result == Z_DATA_ERROR) {
        std::cerr << "Bad zip file.\n";
        load_successful = false;
        goto cleanup;
      }
      if (zlib_result == Z_BUF_ERROR) {
        continue;
      }
      // Here is where we copy from zlib_output buffer into our vector that will hold the decompressed file.
      decompressed->write((const char *)zlib_output, chunk_size - zstream.avail_out);
      uncompressed_size += (chunk_size - zstream.avail_out);
      assert(zlib_result != Z_STREAM_ERROR);
    } while (zstream.avail_out == 0);
    inflateEnd(&zstream);
    ////////////////////////////////////////////////////////

    dump_stream(*decompressed, MODELS_DIR + "test.out");
    load_successful = dst.read_tga_data(*decompressed);

cleanup:
    if (decompressed != NULL) delete(decompressed);
    if (zlib_input   != NULL) delete[](zlib_input);
  }
  else {
    load_successful = dst.read_tga_file((MODELS_DIR + path).c_str());
  }

  if (load_successful) {
    dst.flip_vertically();
  }
  return load_successful;
}

bool Model::load_texture(std::string path) {
  return load_image(path, texture_image);
}

bool Model::load_normal_texture(std::string path) {
  return load_image(path, normal_image);
}

bool Model::load_specular_texture(std::string path) {
  return load_image(path, specular_image);
}

Color Model::diffuse_color(const Vector2f &uv) const {
  return Color(texture_image.get(
      (uint32_t) (uv[0] * texture_image.get_width()),
      (uint32_t) (uv[1] * texture_image.get_height())
  ));
}

Vector3f Model::normal(const Vector2f &uv) const {
  TGAColor col = normal_image.get(uv[0] * normal_image.get_width(), uv[1] * normal_image.get_height());
  Vector3f result;
  for (int i = 0; i < 3; i++) {
    // RGB -> XYZ, but we have the order as BGR, so BGR -> ZYX.
    // Also we want to normalize to the range (-1, +1)
    result[2 - i] = col[i] / 255.0f - 0.5f;
  }
  return result;
}

float Model::specular(const Vector2f &uv) const {
  TGAColor col = normal_image.get(uv[0] * specular_image.get_width(), uv[1] * specular_image.get_height());
  return (col[0] / 1.0f);
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

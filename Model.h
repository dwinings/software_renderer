//
// Created by David Winings on 9/29/16.
//

#ifndef SOFTWARE_RENDERER_MODEL_H
#define SOFTWARE_RENDERER_MODEL_H

#include <string>
#include <vector>
#include "TGAImage.h"
#include "Color.h"
#include "Definitions.h"

class Model {
private:
  std::vector<Vector3f> vertices;
  std::vector<std::vector<Vector3i>> faces;
  std::vector<Vector2f> textures; // uv coordinates for the texture.
  std::vector<Vector3f> normals;
  TGAImage texture_image;
  TGAImage normal_image;
  TGAImage specular_image;
  bool load_image(std::string path, TGAImage &dest);
public:
  Model(const char *filename);
  ~Model();
  bool load_texture(std::string path);
  bool load_normal_texture(std::string path);
  bool load_specular_texture(std::string path);
  int vertex_count();
  int face_count();
  int tex_count();
  int normals_count();
  Color diffuse_color(const Vector2f &uv) const;
  Vector3f vertex(int idx);
  Vector2f texture(int idx);
  Vector3f normal(int idx);
  Vector3f normal(const Vector2f & uv) const;
  float    specular(const Vector2f &uv) const;
  std::vector<Vector3i> face(int idx);
};


#endif //SOFTWARE_RENDERER_MODEL_H

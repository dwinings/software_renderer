//
// Created by David Winings on 10/4/16.
//

#ifndef SOFTWARE_RENDERER_SHADERS_H
#define SOFTWARE_RENDERER_SHADERS_H

#include "Definitions.h"
#include "Utils.h"

struct IShader {
  virtual ~IShader();
  virtual Vector3f vertex(int face_idx, int vertex_idx) = 0;
  virtual bool   fragment(Vector3f bary_coords, Color &color) = 0;
};

struct GouraudShader : public IShader {
  GouraudShader(Model &_model) : model(_model), varying_tex_coords(MatrixXf(2, 3)) {};
  virtual Vector3f vertex(int face_idx, int face_vertex_idx) {
    Vector3i vertex_vals = model.face(face_idx)[face_vertex_idx];
    Vector3f vertex      = model.vertex(vertex_vals[0]);
    Vector3f normal      = model.normal(vertex_vals[2]);

    vertex = project_3d(augmented_multiply(camera_matrix,    vertex, 1));
    normal =       chop(augmented_multiply(camera_inv_trans, normal, 0)).normalized();

    varying_tex_coords.col(face_vertex_idx) = model.texture(vertex_vals[1]);
    varying_intensity[face_vertex_idx]      = normal.dot(projected_light_direction);
    return vertex;
  }

  virtual bool fragment(Vector3f bary_coords, Color &color) {
    float intensity = varying_intensity.dot(bary_coords);
    color = Color(1, 1, 1, 1) * intensity;
    return false;
  }
protected:
  Vector3f varying_intensity;
  MatrixXf varying_tex_coords;
  Model model;
};

struct DiffuseShader : public GouraudShader {
  DiffuseShader(Model &_model) : GouraudShader(_model) {};
  virtual bool fragment(Vector3f bary_coords, Color &color) {
    Vector2f uv     = varying_tex_coords * bary_coords;
    float intensity = varying_intensity.dot(bary_coords);
    color           = model.diffuse_color(uv) * intensity;
    return false;
  }
};

struct NormalMapDiffuseShader : public GouraudShader {
  NormalMapDiffuseShader(Model &_model) : GouraudShader(_model) {};
  virtual bool fragment(Vector3f bary_coords, Color &color) {
    Vector2f uv     = varying_tex_coords * bary_coords;
    Vector3f normal = chop(augmented_multiply(camera_inv_trans, model.normal(uv), 0)).normalized();
    float intensity = normal.dot(projected_light_direction);
    intensity       = clamp(intensity, 0.0f, 1.0f);
    color           = model.diffuse_color(uv) * intensity;
    return false;
  }
};

struct PhongShader : public GouraudShader {
  PhongShader(Model &_model) : GouraudShader(_model) {};
  virtual bool fragment(Vector3f bary_coords, Color &color) {
    float ambient_coeff  = 0.02f;
    float diffuse_coeff  = 1.0f;
    float specular_coeff = 0.6f;
    float specular_intensity;
    float diffuse_intensity;
    Vector2f uv     = varying_tex_coords * bary_coords;
    Vector3f normal = chop(augmented_multiply(camera_inv_trans, model.normal(uv), 0)).normalized();
    Vector3f reflection = (2 * normal * (normal.dot(projected_light_direction)) - projected_light_direction);
    specular_intensity = pow(std::max<float>(reflection.z(), 0.0f), model.specular(uv));
    diffuse_intensity = clamp(normal.dot(projected_light_direction), 0.0f, 1.0f);
    color = model.diffuse_color(uv);

    color.R = clamp(ambient_coeff + color.R * (diffuse_coeff * diffuse_intensity + specular_coeff * specular_intensity), 0.0f, 1.0f);
    color.G = clamp(ambient_coeff + color.G * (diffuse_coeff * diffuse_intensity + specular_coeff * specular_intensity), 0.0f, 1.0f);
    color.B = clamp(ambient_coeff + color.B * (diffuse_coeff * diffuse_intensity + specular_coeff * specular_intensity), 0.0f, 1.0f);
    return false;
  }
};

#endif //SOFTWARE_RENDERER_SHADERS_H

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

struct WireframeShader : IShader{
  WireframeShader(Model &_model) : model(_model), varying_poly() {};
  virtual Vector3f vertex(int face_idx, int face_vertex_idx) {
    Vector3i vertex_vals = model.face(face_idx)[face_vertex_idx];
    Vector3f vertex      = model.vertex(vertex_vals[0]);
    vertex = project_3d(augmented_multiply(camera_matrix,    vertex, 1));
    varying_poly.col(face_vertex_idx) = vertex;
    if (face_vertex_idx == 2) {
      // I don't know if this approach of "finalizing" the polygon would work in real GLSL, but it works here.
      Vector3f plus_x(vertex.x() + 1, vertex.y(), vertex.z());
      Vector3f plus_y(vertex.x(), vertex.y() + 1, vertex.z());
      Vector3f bary   = Barycentric(varying_poly.col(0), varying_poly.col(1), varying_poly.col(2), vertex);
      Vector3f bary_x = Barycentric(varying_poly.col(0), varying_poly.col(1), varying_poly.col(2), plus_x);
      Vector3f bary_y = Barycentric(varying_poly.col(0), varying_poly.col(1), varying_poly.col(2), plus_y);

      // Calculate rate of change of bary coords so we can adjust the threshold for the "wire" pixels vs the discard pixles.
      // In GLSL I could use https://www.khronos.org/registry/webgl/extensions/OES_standard_derivatives/ to do this for me.
      float dx = bary_x.x() - bary.x();
      float dy = bary_y.y() - bary.y();
      d = std::fabsf(dx) + std::fabsf(dy);
    }
    return vertex;
  }

  virtual bool fragment(Vector3f bary_coords, Color &color) {
    float t = d * thickness;
    if (bary_coords[0] < t || bary_coords[1] < t || bary_coords[2] < t) {
      color = Color(0, 1, 0, 1);
      return false;
    }
    return true;
  }
private:
  float thickness = 1.0f;
  float d;
  float varying_area;
  Matrix3f varying_poly;
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

//
// Created by David Winings on 10/4/16.
//

#ifndef SOFTWARE_RENDERER_SHADERS_H
#define SOFTWARE_RENDERER_SHADERS_H

#include "Rasterizer.h"

// 4 Core rendering matrices
static Matrix4f projection_matrix = projection((CAMERA_POSITION - MODEL_POSITION).norm());
static Matrix4f viewport_matrix = viewport(WINDOW_WIDTH, WINDOW_HEIGHT, RENDER_SCALE);
static Matrix4f view_matrix = look_at(CAMERA_POSITION, MODEL_POSITION, Vector3f(0, 1, 0));
static Matrix4f model_matrix = translate(MODEL_POSITION) * rotate(MODEL_ROTATION);

// Put 'em together
static Matrix4f camera_matrix =
    viewport_matrix * projection_matrix *
    view_matrix     * model_matrix;

// And use this to calculate the normals later.
static Matrix4f inv_trans_camera = camera_matrix.inverse().transpose();

struct GouraudShader : public IShader {
  GouraudShader(Model &_model) : model(_model) {};
  virtual Vector3f vertex(int face_idx, int face_vertex_idx) {
    Vector3i vertex_vals = model.face(face_idx)[face_vertex_idx];
    Vector3f vertex = model.vertex(vertex_vals[0]);
    Vector3f normal = model.normal(vertex_vals[2]);

    vertex = project_3d(augmented_multiply(camera_matrix,    vertex, 1));
    normal =       chop(augmented_multiply(inv_trans_camera, normal, 0)).normalized();
    varying_intensity[face_vertex_idx] = normal.dot(LIGHT_DIRECTION);
    return vertex;
  }

  virtual bool fragment(Vector3f bary_coords, Color &color) {
    float intensity = varying_intensity.dot(bary_coords);
    color = Color(1, 1, 1, 1) * intensity;
    return false;
  }
private:
  Vector3f varying_intensity;
  Model model;
};

#endif //SOFTWARE_RENDERER_SHADERS_H

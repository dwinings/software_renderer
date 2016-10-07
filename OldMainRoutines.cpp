//
// Created by David Winings on 10/4/16.
//

#include "OldMainRoutines.h"
#include <iostream>
#include "AffineTransforms.h"

Color black(0, 0, 0, 1);
Color white(1, 1, 1, 1);
Color red(1, 0, 0, 1);
Color green(0, 1, 0, 1);
Color blue(0, 0, 1, 1);
Color cyan(0, 1, 1, 1);
Color yellow(1, 1, 0, 1);
Color magenta(1, 0, 1, 1);

void drawTestLines(Rasterizer &rasterizer) {
  rasterizer.Line(175, 100, 0, 0, cyan);
  rasterizer.Line(0, 100, 175, 200, cyan);
  rasterizer.Line(0, WINDOW_HEIGHT, 50, 0, black, green);
  rasterizer.Line(0, 0, 175, 100, black, green);
}

void drawWireframeHead(Rasterizer &rasterizer) {
  Color colors[]{black, white, red, green, blue, cyan, yellow};

  Model model("african_head.obj");

  for (uint32_t face_idx = 0; face_idx < model.face_count(); face_idx++) {
    for (uint32_t face_vertex = 0; face_vertex < 3; face_vertex++) {
      int32_t face_vertex_num = model.face(face_idx)[face_vertex][0];
      Vector3f vec_0 = model.vertex(model.face(face_idx)[face_vertex][0]);
      Vector3f vec_1 = model.vertex(model.face(face_idx)[(face_vertex + 1) % 3][0]);
      int start_x = (int) ((vec_0.x() + 1.0f) * rasterizer.width() / 2.0f);
      int start_y = (int) ((vec_0.y() + 1.0f) * rasterizer.height() / 2.0f);
      int end_x = (int) ((vec_1.x() + 1.0f) * rasterizer.width() / 2.0f);
      int end_y = (int) ((vec_1.y() + 1.0f) * rasterizer.height() / 2.0f);
      rasterizer.Line(start_x, start_y, end_x, end_y, colors[std::rand() % 7]);
    }
#ifdef DEBUG
    std::cerr << "-----------------------------------------------------------\n";
#endif
  }
}

void drawTriangles(Rasterizer &rasterizer) {
  Vector3f t0[] {Vector3f {10, 70, 0 }, Vector3f {50, 160, 0}, Vector3f{70, 80, 0}};
  Vector3f t1[] {Vector3f {180, 50, 0 },  Vector3f{150, 1, 0},   Vector3f{70, 180, 0}};
  Vector3f t2[] {Vector3f {180, 150, 0 }, Vector3f{120, 160, 0}, Vector3f{130, 180, 0}};
  Vector3f intensities(1, 1, 1);
  rasterizer.Triangle(t0, intensities, cyan);
  rasterizer.Triangle(t1, intensities, yellow);
  rasterizer.Triangle(t2, intensities, magenta);
}

void drawTriangleHead(Rasterizer &rasterizer) {
  Model model("african_head.obj");
  model.load_texture("african_head_diffuse.tga");

  // 4 Core rendering matrices
  Matrix4f projection_matrix = projection((camera_position - model_position).norm());
  Matrix4f viewport_matrix = viewport(WINDOW_WIDTH, WINDOW_HEIGHT, RENDER_SCALE);
  Matrix4f view_matrix = look_at(camera_position, model_position, Vector3f(0, 1, 0));
  Matrix4f model_matrix = translate(model_position);

  // Put 'em together
  Matrix4f camera_matrix =
      viewport_matrix * projection_matrix *
      view_matrix     * model_matrix;

  // And use this to calculate the normals later.
  // Eval avoids some weird Eigen matrix aliasing behavior.
  Matrix4f inv_trans_camera = camera_matrix.inverse().transpose().eval();

#ifdef DEBUG
  std::cout << "Viewport Matrix: " << std::endl;
  std::cout << viewport_matrix << std::endl;
  std::cout << "Projection Matrix: " << std::endl;
  std::cout << projection_matrix << std::endl;
  std::cout << "View Matrix: " << std::endl;
  std::cout << view_matrix << std::endl;
  std::cout << "Model Matrix: " << std::endl;
  std::cout << model_matrix << std::endl;
  std::cout << "Camera Matrix: " << std::endl;
  std::cout << camera_matrix << std::endl;
  std::cout << "Normal xForm Matrix: " << std::endl;
  std::cout << inv_trans_camera << std::endl;
#endif

  Vector3f vertex;
  Vector3f screen_coords[3];
  Vector2f tex_coords[3];
  Vector3f normals[3];
  Vector3f intensities;

  for (uint32_t face_idx = 0; face_idx < model.face_count(); face_idx++) {
    std::vector<Vector3i> face = model.face(face_idx);

    for (uint32_t face_vertex_idx = 0; face_vertex_idx < 3; face_vertex_idx++) {
      vertex = model.vertex(face[face_vertex_idx][0]);
      Vector2f texture_uv = model.texture(face[face_vertex_idx][1]);
      normals[face_vertex_idx] = model.normal(face[face_vertex_idx][2]);

      screen_coords[face_vertex_idx] = project_3d(augmented_multiply(camera_matrix, vertex, 1));
      tex_coords[face_vertex_idx] = texture_uv;
    }

    for (uint32_t i = 0; i < 3; i++) {
      // Normal has to be deformed due to account for 3D projection
      Vector3f newNormal = chop(augmented_multiply(inv_trans_camera, normals[i], 0)).normalized();
      intensities[i] = newNormal.dot(light_direction);
      if (intensities[i] > 1) intensities[i] = 1;
      if (intensities[i] < 0) intensities[i] = 0;
    }
    rasterizer.Triangle(screen_coords[0], screen_coords[1], screen_coords[2], intensities, tex_coords, model);
  }
}

#include "AffineTransforms.h"

Matrix4f scale(const Vector3f &size) {
  Matrix4f xform = Matrix4f::Identity();
  xform(0, 0) = size.x();
  xform(1, 1) = size.y();
  xform(2, 2) = size.z();
  return xform;
}

Matrix4f translate(const Vector3f &distance) {
  Matrix4f xform = Matrix4f::Identity();
  xform(0, 3) = distance.x();
  xform(1, 3) = distance.y();
  xform(2, 3) = distance.z();
  return xform;
}

Matrix4f rotate_x(float radians) {
  Matrix4f xform = Matrix4f::Identity();
  if (radians == 0) return xform;
  xform(1, 1) = cos(radians);
  xform(1, 2) = -1 * sin(radians);
  xform(2, 1) = sin(radians);
  xform(2, 2) = cos(radians);
  return xform;
}

Matrix4f rotate_y(float radians) {
  Matrix4f xform = Matrix4f::Identity();
  if (radians == 0) return xform;
  xform(0, 0) = cos(radians);
  xform(2, 0) = -1 * sin(radians);
  xform(0, 2) = sin(radians);
  xform(2, 2) = cos(radians);
  return xform;
}

Matrix4f rotate_z(float radians) {
  Matrix4f xform = Matrix4f::Identity();
  if (radians == 0) return xform;
  xform(0, 0) = cos(radians);
  xform(0, 1) = -1 * sin(radians);
  xform(1, 0) = sin(radians);
  xform(1, 1) = cos(radians);
  return xform;
}

Matrix4f rotate(const Vector3f &eulerAngles) {
  return rotate_z(eulerAngles.z()) * rotate_y(eulerAngles.y()) * rotate_x(eulerAngles.x());
}

// Mostly used to create the "view matrix"
Matrix4f look_at(const Vector3f &camera_pos, const Vector3f &point, const Vector3f &up) {
  // These are the x' y' z' axes for our new coordinate basis
  Vector3f z = (camera_pos - point).normalized();
  Vector3f x = up.cross(z).normalized();
  Vector3f y = z.cross(x).normalized();

  Matrix4f view_matrix;
  view_matrix << x[0], x[1], x[2], 0,
                 y[0], y[1], y[2], 0,
                 z[0], z[1], z[2], 0,
                    0,    0,    0, 1;


  return translate(point * -1) * view_matrix;
}

Matrix4f projection(float camera_distance) {
  Matrix4f xform = Matrix4f::Identity();
  xform(3, 2) = -1.0f / camera_distance;
  return xform;
}

Matrix4f viewport(uint32_t width, uint32_t height, float scale_size) {
  return translate(Vector3f(
    width / 2.0f,
    height / 2.0f,
    DEPTH_RESOLUTION / 2.0f
  ))
  *
  scale(Vector3f(
    width * scale_size / 2.0f,
    height * scale_size / 2.0f,
    DEPTH_RESOLUTION / 2.0f
  ));
}

Vector4f augmented_multiply(const Matrix4f &mat, const Vector3f &point, float tail) {
  return mat * Vector4f(point.x(), point.y(), point.z(), tail);
}

// Project 4D vector back to 3D space for projection.
Vector3f project_3d(const Vector4f &v) {
  return Vector3f(
    v.x() / v.w(),
    v.y() / v.w(),
    v.z() / v.w()
  );
}

// Chop a 4D vector into a 3D one.
Vector3f chop(const Vector4f &vec4) {
  return Vector3f(vec4.x(), vec4.y(), vec4.z());
}

Vector3f world2screen_ortho(const Vector3f &p) {
  return Vector3f {
      (p.x() + 1.0f) * WINDOW_WIDTH / 2,
      (p.y() + 1.0f) * WINDOW_HEIGHT / 2,
      p.z()
  };
}


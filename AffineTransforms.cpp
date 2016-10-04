#include "AffineTransforms.h"

Matrix4f scale(Vector3f size) {
  Matrix4f xform = Matrix4f::Identity();
  xform(0, 0) = size.x();
  xform(1, 1) = size.y();
  xform(2, 2) = size.z();
  return xform;
}

Matrix4f translate(Vector3f distance) {
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

Matrix4f rotate(Vector3f eulerAngles) {
  return rotate_z(eulerAngles.z()) * rotate_y(eulerAngles.y()) * rotate_x(eulerAngles.x());
}

Matrix4f look_at(Vector3f camera_pos, Vector3f point, Vector3f up) {
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

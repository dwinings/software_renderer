//
// Created by David Winings on 10/3/16.
//

#ifndef SOFTWARE_RENDERER_AFFINETRANSFORMS_H
#define SOFTWARE_RENDERER_AFFINETRANSFORMS_H

#include "Definitions.h"

// Basic Affine xforms
Matrix4f scale(const Vector3f &size);
Matrix4f translate(const Vector3f &distance);
Matrix4f rotate(const Vector3f &eulerAngles);
Matrix4f rotate_x(float radians);
Matrix4f rotate_y(float radians);
Matrix4f rotate_z(float radians);

// Commonly used 3D matrix transforms
Matrix4f look_at(const Vector3f &camera_pos, const Vector3f &point, const Vector3f &up);
Matrix4f projection(float camera_distance);
Matrix4f viewport(uint32_t width, uint32_t height, float scale);

// Useful extras

Vector4f augmented_multiply(const Matrix4f &mat,
                            const Vector3f &point,
                            float tail = 0);
Vector3f project_3d(const Vector4f &v);
Vector3f chop(const Vector4f &vec4);
Vector3f world2screen_ortho(const Vector3f &p);

#endif //SOFTWARE_RENDERER_AFFINETRANSFORMS_H

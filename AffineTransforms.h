//
// Created by David Winings on 10/3/16.
//

#ifndef SOFTWARE_RENDERER_AFFINETRANSFORMS_H
#define SOFTWARE_RENDERER_AFFINETRANSFORMS_H

#include "Definitions.h"

Matrix4f scale(Vector3f size);
Matrix4f translate(Vector3f distance);
Matrix4f rotate(Vector3f eulerAngles);
Matrix4f rotate_x(float radians);
Matrix4f rotate_y(float radians);
Matrix4f rotate_z(float radians);
Matrix4f look_at(Vector3f camera_pos, Vector3f point, Vector3f up);


#endif //SOFTWARE_RENDERER_AFFINETRANSFORMS_H

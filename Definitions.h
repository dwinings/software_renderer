#ifndef SOFTWARE_RENDERER_DEFINITIONS_H
#define SOFTWARE_RENDERER_DEFINITIONS_H

#include <eigen3/Eigen/Dense>
#include "Color.h"


typedef Eigen::Matrix4f Matrix4f;
typedef Eigen::Vector2i Vector2i;
typedef Eigen::Vector2f Vector2f;
typedef Eigen::Vector3i Vector3i;
typedef Eigen::Vector3f Vector3f;
typedef Eigen::Vector4i Vector4i;
typedef Eigen::Vector4f Vector4f;

// Amount of the window the render should take up.
#define RENDER_SCALE 0.9
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800

#define DEPTH_RESOLUTION 255
#define CAMERA_DISTANCE 3

// Wow, we can move and look at things in worldspace now!
#define MODEL_POSITION Vector3f(5, 5, 0)
#define CAMERA_POSITION Vector3f(5, 5, 3)
#define LIGHT_DIRECTION Vector3f(0, 1, 0.4).normalized()


#endif //SOFTWARE_RENDERER_DEFINITIONS_H

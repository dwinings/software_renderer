#ifndef SOFTWARE_RENDERER_DEFINITIONS_H
#define SOFTWARE_RENDERER_DEFINITIONS_H

// Hack to work around win/mac directory structure differences.
#ifdef _MSC_VER
#include <Eigen/Dense>
#else
#include <eigen3/Eigen/Dense>
#endif

#include <stdint.h>

typedef Eigen::Matrix4f Matrix4f;
typedef Eigen::Vector2i Vector2i;
typedef Eigen::Vector2f Vector2f;
typedef Eigen::Vector3i Vector3i;
typedef Eigen::Vector3f Vector3f;
typedef Eigen::Vector4i Vector4i;
typedef Eigen::Vector4f Vector4f;

// Amount of the window the render should take up.
const float RENDER_SCALE = 0.9f;
const uint32_t WINDOW_WIDTH = 800;
const uint32_t WINDOW_HEIGHT = 800;

#define DEPTH_RESOLUTION 255

// Wow, we can move and look at things in worldspace now!
#define MODEL_POSITION Vector3f(0, 0, 0)
#define MODEL_ROTATION Vector3f(0, -1 * M_PI / 8.0f, 0)
#define CAMERA_POSITION Vector3f(0, 1, 3)
#define LIGHT_DIRECTION Vector3f(1, 1, 1).normalized()


#include "AffineTransforms.h"


#endif //SOFTWARE_RENDERER_DEFINITIONS_H

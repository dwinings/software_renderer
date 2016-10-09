#ifndef SOFTWARE_RENDERER_DEFINITIONS_H
#define SOFTWARE_RENDERER_DEFINITIONS_H

// Hack to work around win/mac directory structure differences.
#ifdef _MSC_VER
#include <Eigen/Dense>
#else
#include <eigen3/Eigen/Dense>
#endif

#include <stdint.h>

typedef Eigen::Matrix3f Matrix3f;
typedef Eigen::Matrix4f Matrix4f;
typedef Eigen::MatrixXf MatrixXf;
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

#define MODELS_DIR std::string("/Users/wisp/Code/software_renderer/models/")

#define DEPTH_RESOLUTION 255

extern Vector3f model_position;
extern Vector3f model_rotation;
extern Vector3f camera_position;
extern Vector3f projected_light_direction;
extern Vector3f light_direction;

extern Matrix4f projection_matrix;
extern Matrix4f viewport_matrix;
extern Matrix4f view_matrix;
extern Matrix4f model_matrix;
extern Matrix4f camera_matrix;
extern Matrix4f camera_inv_trans;

#include "AffineTransforms.h"


#endif //SOFTWARE_RENDERER_DEFINITIONS_H

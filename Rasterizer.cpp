//
// Created by David Winings on 9/29/16.
//

#include <cstdlib>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>
#include <cfloat>
#include "Rasterizer.h"
#include "TGAImage.h"
#include "Model.h"

void Rasterizer::SetFrameBuffer(uint32_t *framebuffer, unsigned width, unsigned height) {
  this->framebuffer = framebuffer;
  this->screen_width = width;
  this->screen_height = height;
  this->z_buffer = new float[screen_width * screen_height];
  std::fill_n(z_buffer, screen_width * screen_height, -INFINITY);
}

void Rasterizer::NextFrame() {
  if (drawn_to) {
    drawn_to = false;
    std::fill_n(z_buffer, screen_width * screen_height, -INFINITY);
  }
}

void Rasterizer::dump_zbuffer() {
  for(int i = 0; i < screen_width * screen_height; i++) {
    float z = z_buffer[i] / 255.0f;
    SetPixel(i % screen_width, i / screen_height, Color(z, z, z, 1));
  }
}

void Rasterizer::SetPixel(uint32_t x, uint32_t y, const Color &color) {
  drawn_to = true;
#ifdef DEBUG
    if (x > this->screen_width || x < 0 || y > this->screen_height || y < 0) {
        printf("Invalid point set: (%d, %d)", x, y);
    }
#endif
    uint32_t address = x + (screen_height - y) * this->screen_width;
    framebuffer[address] = color.ToUInt32();
}

// This one chooses an arbitrary granularity and draws that many pixels. Not correct, not efficient.
void
Rasterizer::BresenhamDrawLine(float granularity, int32_t start_x, int32_t start_y, int32_t end_x, int32_t end_y, const Color &color) {
    for (float time = 0; time < 1; time += granularity) {
        int32_t x = (int32_t)(start_x * (1.0 - time) + end_x * time);
        int32_t y = (int32_t)(start_y * (1.0 - time) + end_y * time);
        SetPixel((uint32_t)x, (uint32_t)y, color);
    }
}

// This one just draws a pixel for each X crossed, and interpolates a Y value for it. Efficient, but not correct.
void
Rasterizer::BresenhamDrawLine2(int32_t start_x, int32_t start_y, int32_t end_x, int32_t end_y, const Color &color) {
    // Only need to rearrange X, since that's what we're walking over.
    if (start_x > end_x) {
        std::swap(start_x, end_x);
        std::swap(start_y, end_y);
    }

    // Our granularity is actually the slope.
    for (int32_t x = start_x; x <= end_x; x += 1) {
        float percent_traversed = (x - start_x) / (float)(end_x - start_x);
        int32_t y = (uint32_t)(start_y * (1.0 - percent_traversed) + end_y * percent_traversed);
        SetPixel((uint32_t)x, (uint32_t)y, color);
    }
}

// Same as the last one, but we transpose x/y if we're a steep line to ensure density of vertical lines.
void
Rasterizer::BresenhamDrawLine3(int32_t start_x, int32_t start_y, int32_t end_x, int32_t end_y, const Color &color) {
    bool steep = false;
    uint32_t x_delta = (uint32_t) abs((int)end_x - (int)start_x);
    uint32_t y_delta = (uint32_t) abs((int)end_y - (int)start_y);

    if (y_delta > x_delta) {
        std::swap(start_x, start_y);
        std::swap(end_x, end_y);
        steep = true;
    }

    if (start_x > end_x) {
        std::swap(start_x, end_x);
        std::swap(start_y, end_y);
    }


    float x_extent = (int)end_x - start_x;

    for (uint32_t x = start_x; x <= end_x; x += 1) {
        float percent_traversed = (x - start_x) / x_extent;
        uint32_t y = (uint32_t)(start_y * (1.0 - percent_traversed) + end_y * percent_traversed);

        if (steep) {
            SetPixel(y, x, color);
        } else {
            SetPixel(x, y, color);
        }
    }
}

void
Rasterizer::BresenhamDrawLine4(int32_t start_x, int32_t start_y, int32_t end_x, int32_t end_y, const Color &start_color, const Color &end_color) {
    bool steep = false;
    uint32_t x_delta = (uint32_t) abs((int)end_x - (int)start_x);
    uint32_t y_delta = (uint32_t) abs((int)end_y - (int)start_y);

    Color real_start_color = start_color;
    Color real_end_color = end_color;

    if (y_delta > x_delta) {
        std::swap(start_x, start_y);
        std::swap(end_x, end_y);
        real_start_color = end_color;
        real_end_color = start_color;
        steep = true;
    }

    if (start_x > end_x) {
        std::swap(start_x, end_x);
        std::swap(start_y, end_y);
    }


    int delta_x = (int)end_x - start_x;
    int delta_y = (int)end_y - start_y;

    float slope = std::fabs(delta_y);

    // How far away we are from the correct y value;
    float error = 0;
    uint32_t y = start_y;
    float extent = (end_x - start_x);
    if (!extent) {extent = 1;}

    for (uint32_t x = start_x; x <= end_x; x += 1) {
        auto real_color = (real_end_color - real_start_color)*((x - start_x) / (extent)) + real_start_color;
        if (steep) {
            SetPixel(y, x, real_color);
        } else {
            SetPixel(x, y, real_color);
        }

        error += slope;

        if (error > 0.5) {
            y += (end_y > start_y ? 1 : -1);

            error -= delta_x;
        }
    }
}

void
Rasterizer::BresenhamDrawLine4(int32_t start_x, int32_t start_y, int32_t end_x, int32_t end_y, const Color &color) {
    bool steep = false;
    uint32_t x_delta = (uint32_t) abs((int)end_x - (int)start_x);
    uint32_t y_delta = (uint32_t) abs((int)end_y - (int)start_y);

    if (y_delta > x_delta) {
        std::swap(start_x, start_y);
        std::swap(end_x, end_y);
        steep = true;
    }

    if (start_x > end_x) {
        std::swap(start_x, end_x);
        std::swap(start_y, end_y);
    }


    int delta_x = (int)end_x - start_x;
    int delta_y = (int)end_y - start_y;

    float slope = std::fabs(delta_y);

// How far away we are from the correct y value;
    float error = 0;
    uint32_t y = start_y;

    for (uint32_t x = start_x; x <= end_x; x += 1) {
        if (steep) {
            SetPixel(y, x, color);
        } else {
            SetPixel(x, y, color);
        }

        error += slope;

        if (error > 0.5) {
            y += (end_y > start_y ? 1 : -1);
            error -= delta_x;
        }
    }
}

void Rasterizer::Line(int32_t start_x, int32_t start_y, int32_t end_x, int32_t end_y, const Color &color) {
#ifdef DEBUG
  std::cerr << "Line: (" << start_x << ", " << start_y << ") -> (" << end_x << ", " << end_y << ")" << std::endl;
#endif
  BresenhamDrawLine4(start_x, start_y, end_x, end_y, color);
}

void Rasterizer::Line(int32_t start_x, int32_t start_y, int32_t end_x, int32_t end_y, const Color &start_color, const Color &end_color) {
#ifdef DEBUG
  std::cerr << "Line: (" << start_x << ", " << start_y << ") -> (" << end_x << ", " << end_y << ")" << std::endl;
#endif
  BresenhamDrawLine4(start_x, start_y, end_x, end_y, start_color, end_color);
}

void Rasterizer::TriangleLineMethod(Vector2i p0, Vector2i p1, Vector2i p2, const Color &color) {
  // Sort the points so that p0 is the highest.
  // Thus the p0->p2 line covers the total y volume of the triangle.
  if (p0.y() > p1.y()) std::swap(p0, p1);
  if (p0.y() > p2.y()) std::swap(p0, p2);
  if (p1.y() > p2.y()) std::swap(p1, p2);

  uint32_t total_height = p2.y() - p0.y();
  uint32_t segment_height = p1.y() - p0.y();

  if (!segment_height) segment_height++;

  int32_t y = p0.y();
  for (; y < p1.y(); y++) {
    // Progress of primary line segment
    float alpha = (float)(y - p0.y()) / total_height;
    // Progress of secondary line segment
    float beta  = (float)(y - p0.y()) / segment_height;

    int32_t ax = p0.x() + (p2.x() - p0.x()) * alpha;
    int32_t bx = p0.x() + (p1.x() - p0.x()) * beta;

    Line(ax, y, bx, y, color);
  }

  segment_height = p2.y() - p1.y();
  if (!segment_height) segment_height++;

  for (; y <= p2.y(); y++) {
    float alpha = (float)(y - p0.y()) / total_height;
    float beta  = (float)(y - p1.y()) / segment_height;

    int32_t ax = p0.x() + (p2.x() - p0.x()) * alpha;
    int32_t bx = p1.x() + (p2.x() - p1.x()) * beta;
    Line(ax, y, bx, y, color);
  }
}

void Rasterizer::TrianglePixelMethod(Vector3f p0, Vector3f p1, Vector3f p2, Vector3f intensities, const Color &color) {
  Vector2i bounding_min(screen_width, screen_height);
  Vector2i bounding_max(0, 0);

  // Generate a bounding box, loop fully unrolled.
  if (p0.x() < bounding_min.x()) bounding_min.x() = p0.x();
  if (p0.y() < bounding_min.y()) bounding_min.y() = p0.y();
  if (p1.x() < bounding_min.x()) bounding_min.x() = p1.x();
  if (p1.y() < bounding_min.y()) bounding_min.y() = p1.y();
  if (p2.x() < bounding_min.x()) bounding_min.x() = p2.x();
  if (p2.y() < bounding_min.y()) bounding_min.y() = p2.y();
  if (p0.x() > bounding_max.x()) bounding_max.x() = p0.x();
  if (p0.y() > bounding_max.y()) bounding_max.y() = p0.y();
  if (p1.x() > bounding_max.x()) bounding_max.x() = p1.x();
  if (p1.y() > bounding_max.y()) bounding_max.y() = p1.y();
  if (p2.x() > bounding_max.x()) bounding_max.x() = p2.x();
  if (p2.y() > bounding_max.y()) bounding_max.y() = p2.y();
  if (0 > bounding_min.x()) bounding_min.x() = 0;
  if (0 > bounding_min.y()) bounding_min.y() = 0;
  if (screen_width < bounding_max.x()) bounding_max.x() = screen_width;
  if (screen_height < bounding_max.y()) bounding_max.y() = screen_height;

  for (int32_t y = bounding_min.y(); y <= bounding_max.y(); y++) {
    for (int32_t x = bounding_min.x(); x <= bounding_max.x(); x++) {
      Vector3f pixel(x, y, 0);
      auto bary = Barycentric(p0, p1, p2, pixel);
      if (bary.x() < 0 || bary.y() < 0 || bary.z() < 0 || x < 0 || y < 0) continue;

      int32_t zbuf_idx = (int32_t) round(pixel.x() + pixel.y() * screen_width);
      pixel.z() += bary.x() * p0.z();
      pixel.z() += bary.y() * p1.z();
      pixel.z() += bary.z() * p2.z();

      if (z_buffer[zbuf_idx] < pixel.z()) {
        float intensity = intensities[0] * bary.x() + intensities[1] * bary.y() + intensities[2] * bary.z();
        SetPixel((int)pixel.x(), (int)pixel.y(), color * intensity);
        z_buffer[zbuf_idx] = pixel.z();
      }
    }
  }
}

void Rasterizer::TrianglePixelMethodTextured(Vector3f p0, Vector3f p1, Vector3f p2, Vector3f intensities, const Vector2f *texture_coords,
                                             const Model &model) {
  Vector2i bounding_min(screen_width, screen_height);
  Vector2i bounding_max(0, 0);

  // Generate a bounding box, loop fully unrolled.
  if (p0.x() < bounding_min.x()) bounding_min.x() = p0.x();
  if (p0.y() < bounding_min.y()) bounding_min.y() = p0.y();
  if (p1.x() < bounding_min.x()) bounding_min.x() = p1.x();
  if (p1.y() < bounding_min.y()) bounding_min.y() = p1.y();
  if (p2.x() < bounding_min.x()) bounding_min.x() = p2.x();
  if (p2.y() < bounding_min.y()) bounding_min.y() = p2.y();
  if (p0.x() > bounding_max.x()) bounding_max.x() = p0.x();
  if (p0.y() > bounding_max.y()) bounding_max.y() = p0.y();
  if (p1.x() > bounding_max.x()) bounding_max.x() = p1.x();
  if (p1.y() > bounding_max.y()) bounding_max.y() = p1.y();
  if (p2.x() > bounding_max.x()) bounding_max.x() = p2.x();
  if (p2.y() > bounding_max.y()) bounding_max.y() = p2.y();
  if (0 > bounding_min.x()) bounding_min.x() = 0;
  if (0 > bounding_min.y()) bounding_min.y() = 0;
  if (screen_width < bounding_max.x()) bounding_max.x() = screen_width;
  if (screen_height < bounding_max.y()) bounding_max.y() = screen_height;

  for (int32_t y = bounding_min.y(); y <= bounding_max.y(); y++) {
    for (int32_t x = bounding_min.x(); x <= bounding_max.x(); x++) {
      Vector3f pixel(x, y, 0);
      auto bary = Barycentric(p0, p1, p2, pixel);
      if (bary.x() < 0 || bary.y() < 0 || bary.z() < 0 || x < 0 || y < 0) continue;

      int32_t zbuf_idx = (int32_t) round(pixel.x() + pixel.y() * screen_width);
      pixel.z() += bary.x() * p0.z();
      pixel.z() += bary.y() * p1.z();
      pixel.z() += bary.z() * p2.z();

      if (z_buffer[zbuf_idx] < pixel.z()) {
        Vector2f uv = texture_coords[0] * bary.x() + texture_coords[1] * bary.y() + texture_coords[2] * bary.z();
        float intensity = intensities[0] * bary.x() + intensities[1] * bary.y() + intensities[2] * bary.z();
        Color color = model.diffuse_color(uv) * intensity;
        color.A = 1;


        SetPixel((int)pixel.x(), (int)pixel.y(), color);
        z_buffer[zbuf_idx] = pixel.z();
      }
    }
  }
}

void Rasterizer::Triangle(Vector3f* points, IShader &shader) {
  TriangleShader(points, shader);
}

void Rasterizer::TriangleShader(Vector3f* points, IShader &shader) {
  Vector2i bounding_min(screen_width, screen_height);
  Vector2i bounding_max(0, 0);
  Color color_holder;
  bool skip_current_pixel;

  // Generate a bounding box

  for (uint32_t i = 0; i < 3; i++) {
    for (uint32_t j = 0; j < 2; j++) {
      if (points[i][j] < bounding_min[j]) bounding_min[j] = points[i][j];
      if (points[i][j] > bounding_max[j]) bounding_max[j] = points[i][j];
    }
  }

  if (0 > bounding_min.x()) bounding_min.x() = 0;
  if (0 > bounding_min.y()) bounding_min.y() = 0;
  if (screen_width < bounding_max.x()) bounding_max.x() = screen_width;
  if (screen_height < bounding_max.y()) bounding_max.y() = screen_height;

  for (int32_t y = bounding_min.y(); y <= bounding_max.y(); y++) {
    for (int32_t x = bounding_min.x(); x <= bounding_max.x(); x++) {
      Vector3f pixel(x, y, 0);
      auto bary = Barycentric(points[0], points[1], points[2], pixel);
      if (bary.x() < 0 || bary.y() < 0 || bary.z() < 0 || x < 0 || y < 0) continue;

      int32_t zbuf_idx = (int32_t) round(pixel.x() + pixel.y() * screen_width);

      for (uint32_t i = 0; i < 3; i++) pixel.z() += bary[i] * points[i].z();

      if (z_buffer[zbuf_idx] < pixel.z()) {
        // Sets the color in color_holder
        skip_current_pixel = shader.fragment(bary, color_holder);
        if (!skip_current_pixel) {
          SetPixel((int)pixel.x(), (int)pixel.y(), color_holder);
          z_buffer[zbuf_idx] = pixel.z();
        }
      }
    }
  }
}

void Rasterizer::Triangle(Vector3f p0, Vector3f p1, Vector3f p2, Vector3f intensities, const Color &color) {
  TrianglePixelMethod(p0, p1, p2, intensities, color);
}

void Rasterizer::Triangle(Vector3f p0, Vector3f p1, Vector3f p2, Vector3f intensities,
                          const Vector2f *texture_coords, const Model &texture) {

  TrianglePixelMethodTextured(p0, p1, p2, intensities, texture_coords, texture);
}

void Rasterizer::Triangle(Vector3f points[], Vector3f intensities, const Color &color) {
  Triangle(points[0], points[1], points[2], intensities, color);
}

uint32_t Rasterizer::width()const {
    return this->screen_width;
}

uint32_t Rasterizer::height()const {
    return this->screen_height;
}

IShader::~IShader() {}


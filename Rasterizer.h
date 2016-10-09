//
// Created by David Winings on 9/29/16.
//

#ifndef SOFTWARE_RENDERER_RASTERIZER_H
#define SOFTWARE_RENDERER_RASTERIZER_H


#include <cstdint>
#include "Definitions.h"
#include "Color.h"
#include "Model.h"
#include "Shaders.h"

class Rasterizer {
public:
  void SetFrameBuffer(uint32_t* framebuffer, unsigned width, unsigned height);
  void SetPixel(uint32_t x, uint32_t y, const Color &color);
  void NextFrame();
  void Line(int32_t start_x, int32_t start_y, int32_t end_x, int32_t end_y, const Color &color);
  void Line(int32_t start_x, int32_t start_y, int32_t end_x, int32_t end_y, const Color &start_color, const Color &end_color);
  void Triangle(Vector3f* points, IShader& shader);
  void Triangle(Vector3f p0, Vector3f p1, Vector3f p2,
      Vector3f intensities, const Vector2f *texture_coords, const Model &texture);
  void Triangle(Vector3f p0, Vector3f p1, Vector3f p2,
      Vector3f intensities, const Color &color);
  void Triangle(Vector3f points[], Vector3f intensities, const Color &color);
  uint32_t width()const;
  uint32_t height()const;
  void dump_zbuffer();
private:
  uint32_t* framebuffer;
  float *z_buffer;
  uint32_t screen_width;
  uint32_t screen_height;
  bool drawn_to;

  void TriangleShader(Vector3f* points, IShader& shader);

  // Lesson 1
  void BresenhamDrawLine(float granularity, int32_t start_x, int32_t start_y, int32_t end_x, int32_t end_y, const Color &color);
  void BresenhamDrawLine2(int32_t start_x, int32_t start_y, int32_t end_x, int32_t end_y, const Color &color);
  void BresenhamDrawLine3(int32_t start_x, int32_t start_y, int32_t end_x, int32_t end_y, const Color &color);
  void BresenhamDrawLine4(int32_t start_x, int32_t start_y, int32_t end_x, int32_t end_y, const Color &color);
  void BresenhamDrawLine4(int32_t start_x, int32_t start_y, int32_t end_x, int32_t end_y, const Color &start_color, const Color &end_color);

  // Lesson 2
  void TriangleLineMethod(Vector2i p0, Vector2i p1, Vector2i p2, const Color &color);
  void TrianglePixelMethod(Vector3f p0, Vector3f p1, Vector3f p2, Vector3f intensities, const Color &color);
  void TrianglePixelMethodTextured(Vector3f p0, Vector3f p1, Vector3f p2, Vector3f intensities, const Vector2f *texture_coords,
                                   const Model &texture);
};


#endif //SOFTWARE_RENDERER_RASTERIZER_H

#ifndef SOFTWARE_RENDERER_COLOR_H
#define SOFTWARE_RENDERER_COLOR_H

#include <stdint.h>
#include <string>
#include "TGAImage.h"

#define RANDOM_COLOR Color(((float)rand()/RAND_MAX), ((float)rand()/RAND_MAX), ((float)rand()/RAND_MAX), 1)

class Color
{
public:

  float R, G, B, A;

  Color(float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f);
  Color(TGAColor tcol);

  uint32_t ToUInt32() const;

  Color operator + (const Color &c) const;
  Color operator - (const Color &c) const;
  Color operator * (float f) const;
  std::string str();
private:
  static const float constexpr tcol_denom = 1.0f / 255.0f;
};

#endif

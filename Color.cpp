#include <iostream>
#include <algorithm>
#include "Color.h"

Color::Color(float r, float g, float b, float a)
{
	R = r;
	G = g;
	B = b;
	A = a;
}

Color::Color(TGAColor tcol) {
  R =  tcol.bgra[2] * tcol_denom;
  G =  tcol.bgra[1] * tcol_denom;
  B =  tcol.bgra[0] * tcol_denom;
  A =  1.0f;
}

uint32_t
Color::ToUInt32() const
{
	int32_t r = (int32_t)(std::min(1.0f, std::max(R, 0.0f)) * 255.0f);
	int32_t g = (int32_t)(std::min(1.0f, std::max(G, 0.0f)) * 255.0f);
	int32_t b = (int32_t)(std::min(1.0f, std::max(B, 0.0f)) * 255.0f);
	int32_t a = (int32_t)(std::min(1.0f, std::max(A, 0.0f)) * 255.0f);

  // TODO: Have this method parse the SDL_PixelFormat to determine color order.
#ifdef _MSC_VER
	return (a << 24) | (r << 16) | (g << 8) | b;
#else
  return (b << 24) | (g << 16) | (r << 8) | a;
#endif
}

Color
Color::operator + (const Color &c) const
{
	return Color(R + c.R, G + c.G, B + c.B, A + c.A);
}

Color
Color::operator - (const Color &c) const
{
	return Color(R - c.R, G - c.G, B - c.B, A - c.A);
}

Color
Color::operator * (float f) const
{
	return Color(R * f, G * f, B * f, 1);
}

std::string
Color::str() {
    char buf[100];
	sprintf(buf, "#%02X%02X%02X%02X)",
            (uint8_t)(R*255.0f),
            (uint8_t)(G*255.0f),
            (uint8_t)(B*255.0f),
            (uint8_t)(A*255.0f));
    return std::string(buf);
}

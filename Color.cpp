#include <iostream>
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
	uint32_t r = (uint32_t)(R * 255.0f);
	uint32_t g = (uint32_t)(G * 255.0f);
	uint32_t b = (uint32_t)(B * 255.0f);
	uint32_t a = (uint32_t)(A * 255.0f);

	return (b << 24) | (g << 16) | (r << 8) | a;
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
	return Color(R * f, G * f, B * f, A * f);
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

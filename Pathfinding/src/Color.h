#pragma once
#include <cstdint>

struct ColorBGR
{
	uint8_t B;
	uint8_t G;
	uint8_t R;

	ColorBGR() : B(0), G(0), R(0) { }
	ColorBGR(uint8_t b, uint8_t g, uint8_t r) : B(b), G(g), R(r)
	{
	}

	bool operator==(const ColorBGR& other)
	{
		return (B == other.B && G == other.G && R == other.R);
	}

	bool operator!=(const ColorBGR& other)
	{
		return (B != other.B || G != other.G || R != other.R);
	}
};
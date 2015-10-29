#define _USE_MATH_DEFINES
#include <cmath> 
#include <string>
#include "Conversion.h"

const float FLT_PI = static_cast<float>(M_PI);

double ToRadians(double degrees)
{
	return degrees * M_PI / 180.0;
}

float ToRadians(float degrees)
{
	return degrees * FLT_PI / 180.0f;
}

double ToDegrees(double radians)
{
	return radians * 180.0 / M_PI;
}

float ToDegrees(float radians)
{
	return radians * 180.0f / FLT_PI;
}
#pragma once

// this class contains convenient things like typedefs or enums.

// typedefs:
typedef unsigned int uint;

// Curvature methods:
enum class Curvature
{
	HORIZON = 0,
	MEAN = 1,
	GAUSSIAN = 2,
	ORIGINAL = 3,
	DISTORTION = 4,
	CONE = 5,
	DISTORTION_SIGNED = 6,
	MEAN_SIGNED = 7,
	MAX_PRINCIPAL_DISTORTION = 8,
	MIN_PRINCIPAL_DISTORTION = 9,
	FALSE_GAUSSIAN = 10,
	FALSE_MEAN = 11,
	PRINCIPAL_DEVIATION = 12
};

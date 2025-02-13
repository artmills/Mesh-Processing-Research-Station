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
	MEAN_SIGNED = 7
};

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
	PRINCIPAL_DEVIATION = 12,
	DIFFERENCE = 13
};

// Statistics:
struct Statistics {
	double min;
	double max;
	double mean;

	Statistics() : min(0.0), max(0.0), mean(0.0) {}
	Statistics(double _min, double _max, double _mean) 
		: min(_min), max(_max), mean(_mean) {}
};

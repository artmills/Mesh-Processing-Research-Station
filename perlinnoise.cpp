#include "perlinnoise.hpp"

// Make sure to initialize the permutation array!
PerlinNoise::PerlinNoise()
{
	// Initialize permutation array.
	// We'll use the same array used by Perlin himself.
	p = {
		151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,
		8,99,37,240,21,10,23,190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,
		35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,
		134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,
		55,46,245,40,244,102,143,54, 65,25,63,161,1,216,80,73,209,76,132,187,208, 89,
		18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,
		250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,
		189,28,42,223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 
		43,172,9,129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,
		97,228,251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,
		107,49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
		138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180 };
	
	// Perlin duplicated this array.
	p.insert(p.end(), p.begin(), p.end());
}
PerlinNoise::~PerlinNoise()
{

}

float PerlinNoise::Noise(float x, float y, float z)
{
	//std::cout << std::endl;
	//std::cout << x << " " << y << " " << z << std::endl;

	// Find the unit cube that contains the given point.
	int X = (int)floor(x) & 255;
	int Y = (int)floor(y) & 255;
	int Z = (int)floor(z) & 255;

	//std::cout << X << " " << Y << " " << Z << std::endl;

	// Find relative x, y, z of point in cube.
	x -= floor(x);
	y -= floor(y);
	z -= floor(z);

	//std::cout << x << " " << y << " " << z << std::endl;

	// Compute fade curves of x, y, z.
	float u = Fade(x);
	float v = Fade(y);
	float w = Fade(z);

	//std::cout << u << " " << v << " " << w << std::endl;

	// Hash the coordinates of the eight cube corners.
	// Hash values are given by the permutation array p[].
	int A = p[X] + Y;
	int B = p[X+1] + Y;
	int AA = p[A] + Z;
	int BA = p[B] + Z;
	int AB = p[A+1] + Z;
	int BB = p[B+1] + Z;

	// Add blended results from the eight corners of the cube.
	return Lerp(w,   Lerp(v, Lerp(u, Gradient(p[AA  ], x  , y  , z   ),
                                     Gradient(p[BA  ], x-1, y  , z   )),
                             Lerp(u, Gradient(p[AB  ], x  , y-1, z   ),  
                                     Gradient(p[BB  ], x-1, y-1, z   ))),
                     Lerp(v, Lerp(u, Gradient(p[AA+1], x  , y  , z-1 ),  
                                     Gradient(p[BA+1], x-1, y  , z-1 )),
                             Lerp(u, Gradient(p[AB+1], x  , y-1, z-1 ),
                                     Gradient(p[BB+1], x-1, y-1, z-1 ))));	
}

float PerlinNoise::LayeredNoise(float x, float y, float z, int octaves, float persistence, float lacunarity)
{
	// Call the Perlin Noise algorithm multiple times and add them all together.
	// Octaves is the number of iterations.
	// The frequency of the ith iteration will be 2^i.
	// The amplitude of the ith iteration will be persistence^i.
	float total = 0;
	float frequency = 1;
	float amplitude = 1;
	//float max = 0; // Normalize.

	for (int i = 0; i < octaves; ++i)
	{
		total += Noise(x * frequency, y * frequency, z * frequency) * amplitude;
		//max += amplitude;
		amplitude *= persistence;
		frequency *= lacunarity;
	}

	return total;
}



float PerlinNoise::Fade(float t)
{
	return t * t * t * (t * (t * 6 - 15) + 10);
}

float PerlinNoise::Lerp(float t, float a, float b)
{
	return a + t * (b - a);
}

float PerlinNoise::Gradient(int hash, float x, float y, float z)
{
	// Convert lower 4 bits of hash into 12 Gradient directions.
	int h = hash % 15;
	float u = h < 8 ? x : y,
		   v = h < 4 ? y : h == 12 || h == 14 ? x : z;
	return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

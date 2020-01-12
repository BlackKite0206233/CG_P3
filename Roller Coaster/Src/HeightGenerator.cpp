#include "HeightGenerator.h"
#include <cmath>

int HeightGenerator::AMPLITUDE = 200;
int HeightGenerator::OCTAVES = 3;
float HeightGenerator::ROUGHNESS = 0.25;

HeightGenerator::HeightGenerator(int s) {
	dist = normal_distribution<float>(0, 1);
	seed = s;
}

float HeightGenerator::GenerateHeight(int x, int y) {
	float total = 0;
	float d = pow(4, OCTAVES - 1);
	for (int i = 0; i < OCTAVES; i++) {
		float freq = pow(4, i) / d;
		float ampl = pow(ROUGHNESS, i) * AMPLITUDE;
		total += getInterpolationNoise(x * freq, y * freq) * ampl;
	}
	return total;
}

float HeightGenerator::getNoise(int x, int y) {
	int randomSeed = x * 468165 + y * 125047 + seed;
	if (randomMap.find(randomSeed) == randomMap.end()) {
		rng.seed(randomSeed);
		randomMap[randomSeed] = dist(rng);
	}
	return randomMap[randomSeed];
}

float HeightGenerator::getSmoothNoise(int x, int y) {
	float noise = 0;
	noise += (getNoise(x - 1, y - 1) + getNoise(x + 1, y - 1) + getNoise(x + 1, y + 1) + getNoise(x - 1, y - 1)) / 16;
	noise += (getNoise(x, y - 1) + getNoise(x, y + 1) + getNoise(x - 1, y) + getNoise(x + 1, y)) / 8;
	noise += getNoise(x, y) / 4;
	return noise;
}

float HeightGenerator::getInterpolationNoise(float x, float y) {
	int intX = (int)x;
	int intY = (int)y;
	float fracX = x - intX;
	float fracY = y - intY;
	float v1 = getSmoothNoise(intX, intY);
	float v2 = getSmoothNoise(intX + 1, intY);
	float v3 = getSmoothNoise(intX, intY + 1);
	float v4 = getSmoothNoise(intX + 1, intY + 1);
	float i1 = interpolation(v1, v2, fracX);
	float i2 = interpolation(v3, v4, fracX);
	return interpolation(i1, i2, fracY);
}

float HeightGenerator::interpolation(float a, float b, float blend) {
	double theta = blend * _Pi;
	float f = (1 - cos(theta)) * 0.5;
	return a * (1 - f) + b * f;
}
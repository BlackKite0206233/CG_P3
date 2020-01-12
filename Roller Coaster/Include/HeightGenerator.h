#include <random>
#include <map>
using namespace std;

class HeightGenerator {
private:
	static int AMPLITUDE;
	static int OCTAVES;
	static float ROUGHNESS;

public:
	HeightGenerator(int s);
	float GenerateHeight(int x, int y);

	int seed;

private:
	float getNoise(int x, int y);
	float getSmoothNoise(int x, int y);
	float getInterpolationNoise(float x, float y);
	float interpolation(float a, float b, float blend);

	default_random_engine rng;
	normal_distribution<float> dist;
	map<int, float> randomMap;
};
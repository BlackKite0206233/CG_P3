#pragma once

#include <vector>
#include <map>
#include <utility>
#include "ControlPoint.h"

using namespace std;

struct MapComp {
	bool operator() (const pair<int, int>& a, const pair<int, int>& b) const {
		if (a.first < b.first) return true;
		if (a.first > b.first) return false;
		return a.second < b.second;
	}
};

struct PathData {
	vector<ControlPoint> pointSet;
	ControlPoint a, b, c, d;
	int p0, p1, p2, p3;
	double length;
};

typedef map<pair<int, int>, PathData, MapComp> Path;
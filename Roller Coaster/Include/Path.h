#pragma once

#include <vector>
#include <map>
#include <utility>
#include "ControlPoint.h"

using namespace std;

enum CurveType {
	Linear,
	Cardinal,
	Cubic
};

enum TrackType {
	Line,
	Track,
	Road,
};


struct MapComp {
	bool operator() (const pair<int, int>& a, const pair<int, int>& b) const {
		if (a.first < b.first) return true;
		if (a.first > b.first) return false;
		return a.second < b.second;
	}
};

class PathData {
public:
	static CurveType curve;
	static TrackType track;
	vector<ControlPoint> pointSet;
	ControlPoint a, b, c, d;
	int p0, p1, p2, p3;
	double length;
	ControlPoint CalInterpolation(double t);
	void Draw(bool doingShadows, bool isSelected);
};

typedef map<pair<int, int>, PathData, MapComp> Path;
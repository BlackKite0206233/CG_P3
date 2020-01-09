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
	bool operator()(const pair<int, int> &a, const pair<int, int> &b) const {
		if (a.first < b.first)
			return true;
		if (a.first > b.first)
			return false;
		return a.second < b.second;
	}
};

class PathData {
public:
	static CurveType curve;
	static TrackType track;
	static double speed;

	CtrlPoint CalInterpolation(double t);
	void Draw(bool doingShadows, bool isSelected);
	void DrawLine(int side);
	void DrawTrack();
	void DrawRoad();
	void DrawPole();

	vector<CtrlPoint> pointSet;
	CtrlPoint a, b, c, d;
	int p0, p1, p2, p3;
	double length;
};

typedef map<pair<int, int>, PathData, MapComp> Path;
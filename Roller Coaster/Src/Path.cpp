#include "Path.h"

CurveType PathData::type;

ControlPoint PathData::CalInterpolation(double t) {
	ControlPoint qt;
	if (type == Linear) {
		qt.pos    = (1 - t) * a.pos    + t * b.pos;
		qt.orient = (1 - t) * a.orient + t * b.orient;
	}
	else {
		qt.pos    = pow(t, 3) * a.pos    + pow(t, 2) * b.pos    + t * c.pos    + d.pos;
		qt.orient = pow(t, 3) * a.orient + pow(t, 2) * b.orient + t * c.orient + d.orient;
	}
	return qt;
}
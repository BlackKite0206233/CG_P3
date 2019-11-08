#include "Path.h"
#include <windows.h>
#include <GL/gl.h>

CurveType PathData::curve;
TrackType PathData::track;

ControlPoint PathData::CalInterpolation(double t) {
	ControlPoint qt;
	if (curve == Linear) {
		qt.pos    = (1 - t) * a.pos    + t * b.pos;
		qt.orient = (1 - t) * a.orient + t * b.orient;
	}
	else {
		qt.pos    = pow(t, 3) * a.pos    + pow(t, 2) * b.pos    + t * c.pos    + d.pos;
		qt.orient = pow(t, 3) * a.orient + pow(t, 2) * b.orient + t * c.orient + d.orient;
	}
	return qt;
}

void drawLine(const vector<ControlPoint>& pointSet, int side) {
	Pnt3f w;
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < pointSet.size() - 1; i++) {
		w = Pnt3f::CrossProduct(pointSet[i + 1].pos - pointSet[i].pos, pointSet[i].orient);
		w.normalize();
		w = w * 2.5;
		glVertex3dv((pointSet[i].pos + side * w).v());
	}
	glVertex3dv((pointSet.back().pos + side * w).v());
	glEnd();
}

void drawTrack(const vector<ControlPoint>& pointSet) {
	Pnt3f w, v;
	Pnt3f p;
	glBegin(GL_QUADS);
	for (int i = 0; i < pointSet.size() - 1; i += 5) {
		v = pointSet[i + 1].pos - pointSet[i].pos;
		v.normalize();
		w = Pnt3f::CrossProduct(v, pointSet[i].orient);
		w.normalize();
		w = w * 5;
		p = pointSet[i].pos - pointSet[i].orient;
		glVertex3dv((p + w + v).v());
		glVertex3dv((p + w - v).v());
		glVertex3dv((p - w - v).v());
		glVertex3dv((p - w + v).v());
	}
	glEnd();
}

void drawRoad(const vector<ControlPoint>& pointSet) {
	Pnt3f w;
	glBegin(GL_TRIANGLE_STRIP);
	for (int i = 0; i < pointSet.size() - 1; i++) {
		w = Pnt3f::CrossProduct(pointSet[i + 1].pos - pointSet[i].pos, pointSet[i].orient);
		w.normalize();
		w = w * 2.4;
		glVertex3dv((pointSet[i].pos + w).v());
		glVertex3dv((pointSet[i].pos - w).v());
	}
	glVertex3dv((pointSet.back().pos + w).v());
	glVertex3dv((pointSet.back().pos - w).v());
	glEnd();
}

void PathData::Draw(bool doingShadows, bool isSelected) {
	if (!doingShadows) {
		if (isSelected)
			glColor3d(1, 1, 0);
		else
			glColor3d(0.22, 0.18, 0.04);
	}
	drawLine(pointSet, 1);
	drawLine(pointSet, -1);
	if (!doingShadows) {
		if (isSelected)
			glColor3d(1, 1, 0);
		else
			glColor3d(0.49, 0.38, 0.11);
	}
	if (track == Track)
		drawTrack(pointSet);
	else if (track == Road)
		drawRoad(pointSet);
}
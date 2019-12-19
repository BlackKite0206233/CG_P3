#include "Path.h"
#include <windows.h>
#include <GL/gl.h>

#define INTERVAL 5

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
	qt.inter = t;
	return qt;
}

void PathData::DrawLine(int side) {
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

void PathData::DrawTrack() {
	Pnt3f w, v;
	Pnt3f p;
	ControlPoint p0, p1;
	glBegin(GL_QUADS);
	for (double t = 0; t < 1; t += INTERVAL / length) {
		p0 = CalInterpolation(t);
		p1 = CalInterpolation(t + INTERVAL / length);
		v = p1.pos - p0.pos;
		v.normalize();
		w = Pnt3f::CrossProduct(v, p0.orient);
		w.normalize();
		w = w * 5;
		p = p0.pos - p0.orient;
		glVertex3dv((p + w + v).v());
		glVertex3dv((p + w - v).v());
		glVertex3dv((p - w - v).v());
		glVertex3dv((p - w + v).v());
	}
	glEnd();
}

void PathData::DrawRoad() {
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
	DrawLine(1);
	DrawLine(-1);
	if (!doingShadows) {
		if (isSelected)
			glColor3d(1, 1, 0);
		else
			glColor3d(0.49, 0.38, 0.11);
	}
	if (track == Track)
		DrawTrack();
	else if (track == Road)
		DrawRoad();
}
#include "Train.h"
#include <windows.h>
#include <GL/gl.h>

#define DIVIDE_LINE 100
#define OFFSET 15

bool CTrain::isMove;
double CTrain::speed;
CTrack* CTrain::track;

CTrain::CTrain(CarType type): t(0), type(type) {
}

CTrain::CTrain(int p0, int p1, int p2, int p3, CarType type): p0(p0), p1(p1), p2(p2), p3(p3), t(0), type(type) {
	PathData pd = track->GetPath(p0, p1, p2, p3);
	SetNewPos(pd);
}

void CTrain::Move() {
	PathData pd = track->GetPath(p0, p1, p2, p3);
	if (type == Head) {
		t += speed / pd.length;
		if (t >= 1) {
			t -= 1;
			pd = track->GetNextPath(pd);
			p0 = pd.p0;
			p1 = pd.p1;
			p2 = pd.p2;
			p3 = pd.p3;
		}
	}

	SetNewPos(pd);
	for (int i = 0; i < car.size(); i++) {
		pd = track->GetPath(car[i].p0, car[i].p1, car[i].p2, car[i].p3);
		car[i].t += speed / pd.length;
		if (car[i].t >= 1) {
			car[i].t -= 1;
			car[i].p0 = i ? car[i - 1].p0 : p0;
			car[i].p1 = i ? car[i - 1].p1 : p1;
			car[i].p2 = i ? car[i - 1].p2 : p2;
			car[i].p3 = i ? car[i - 1].p3 : p3;
		}
		car[i].Move();
	}
}

void CTrain::SetNewPos(PathData& pd) {
	ControlPoint qt, qt_1;
	qt   = pd.CalInterpolation(t);
	qt_1 = pd.CalInterpolation(t + speed / pd.length);

	pos    = qt.pos;
	orient = qt.orient;
	v      = qt_1.pos - qt.pos;
	v.normalize();
	w = Pnt3f::CrossProduct(v, orient);
	w.normalize();
}

void CTrain::Draw(bool doingShadows, bool isSelected) {
	if (!doingShadows) {
		if (isSelected)
			glColor3d(1, 1, 0);
		else if (type == Head)
			glColor3d(1, 0, 0);
		else
			glColor3d(0, 0, 1);
	}
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3dv((pos - w * 5 - v * 5 + orient).v());
	glTexCoord2f(1, 0);
	glVertex3dv((pos + w * 5 - v * 5 + orient).v());
	glTexCoord2f(1, 1);
	glVertex3dv((pos + w * 5 + v * 5 + orient).v());
	glTexCoord2f(0, 1);
	glVertex3dv((pos - w * 5 + v * 5 + orient).v());
	glEnd();
	for (auto& c : car) {
		c.Draw(doingShadows, isSelected);
	}
}

void CTrain::AddCar() {
	CTrain train = car.empty() ? *this : car.back();
	PathData pd = CTrain::track->GetPath(train.p0, train.p1, train.p2, train.p3);

	CTrain c(Car);
	c.t = train.t - OFFSET / pd.length;
	if (c.t < 0) {
		pd = CTrain::track->GetPrevPath(pd);
		c.t += 1;
	}
	c.p0 = pd.p0;
	c.p1 = pd.p1;
	c.p2 = pd.p2;
	c.p3 = pd.p3;
	c.SetNewPos(pd);
	car.push_back(c);
}

void CTrain::RemoveCar() {
	car.pop_back();
}
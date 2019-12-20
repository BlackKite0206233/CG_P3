#include "Train.h"
#include <windows.h>
#include <GL/gl.h>

#define DIVIDE_LINE 100
#define OFFSET 12

bool CTrain::isMove;
double CTrain::speed0;
CTrack* CTrain::track;

CTrain::CTrain(CarType type): t(0), type(type), speed(0), carSpeed(0) {
}

CTrain::CTrain(int p0, int p1, int p2, int p3, CarType type): p0(p0), p1(p1), p2(p2), p3(p3), t(0), type(type), carSpeed(0) {
	PathData pd = track->GetPath(p0, p1, p2, p3);
	speed = pd.speed * CTrain::speed0;
	SetNewPos(pd);
}

void CTrain::Move() {
	PathData pd = track->GetPath(p0, p1, p2, p3);
	carSpeed -= Pnt3f::DotProduct(v, Pnt3f(0, 1, 0)) * 3;
	if (carSpeed < 0.3) {
		speed = pd.speed * CTrain::speed0;
	}
	else {
		speed = carSpeed * CTrain::speed0;
	}
	if (carSpeed < 0) {
		carSpeed = 0;
	}
	t += speed / pd.length;

	if (t >= 1) {
		double t_tmp = t - 1;
		double l_tmp = pd.length;

		pd = track->GetNextPath(pd);
		p0 = pd.p0;
		p1 = pd.p1;
		p2 = pd.p2;
		p3 = pd.p3;
		t  = t_tmp * l_tmp / pd.length;
	}
}

void CTrain::SetNewPos(PathData& pd) {
	ControlPoint qt, qt_1;
	qt   = pd.CalInterpolation(t);
	qt_1 = pd.CalInterpolation(t + 5 / pd.length);

	pos    = qt.pos;
	orient = qt.orient;
	v      = qt_1.pos - qt.pos;
	v.normalize();
	w = Pnt3f::CrossProduct(v, orient);
	w.normalize();
}

void CTrain::Draw(bool doingShadows, bool isSelected) {
	PathData pd = track->GetPath(p0, p1, p2, p3);
	SetNewPos(pd);

	if (!doingShadows) {
		if (isSelected && type == Head)
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

	for (int i = 0; i < car.size(); i++) {
		CTrain prev = i ? car[i - 1] : *this;
		PathData prevPd = track->GetPath(prev.p0, prev.p1, prev.p2, prev.p3);
		car[i].t = prev.t - OFFSET / prevPd.length;
		if (car[i].t < 0) {
			pd = track->GetPath(car[i].p0, car[i].p1, car[i].p2, car[i].p3);
			car[i].t = 1 + car[i].t * prevPd.length / pd.length;
		}
		else {
			car[i].p0 = prevPd.p0;
			car[i].p1 = prevPd.p1;
			car[i].p2 = prevPd.p2;
			car[i].p3 = prevPd.p3;
		}
		car[i].Draw(doingShadows, isSelected);
	}
}

void CTrain::AddCar() {
	CTrain train = car.empty() ? *this : car.back();
	PathData pd = CTrain::track->GetPath(train.p0, train.p1, train.p2, train.p3);

	CTrain c(Car);
	c.t = train.t - OFFSET / pd.length;
	if (c.t < 0) {
		pd  = CTrain::track->GetPrevPath(pd);
	}
	c.p0 = pd.p0;
	c.p1 = pd.p1;
	c.p2 = pd.p2;
	c.p3 = pd.p3;
	car.push_back(c);
}

void CTrain::RemoveCar() {
	car.pop_back();
}
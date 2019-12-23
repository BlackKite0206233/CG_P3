#include "Train.h"
#include <windows.h>
#include <GL/gl.h>

#define OFFSET 23

bool CTrain::isMove;
double CTrain::speed0;
CTrack* CTrain::track;

CTrain::CTrain(CarType type): t(0), type(type), speed(0), carSpeed(0) {
	if (type == CarType::Head) {
		this->model = new Model("./toon_train.obj", 25);
	}
	else {
		this->model = new Model("./car.obj", 25);
	}
}

CTrain::CTrain(int p0, int p1, int p2, int p3, CarType type): p0(p0), p1(p1), p2(p2), p3(p3), t(0), type(type), carSpeed(0) {
	PathData pd = track->GetPath(p0, p1, p2, p3);
	speed = pd.speed * CTrain::speed0;
	SetNewPos(pd);
	if (type == CarType::Head) {
		this->model = new Model("./toon_train.obj", 25);
	}
	else {
		this->model = new Model("./car.obj", 25);
	}
}

void CTrain::Move() {
	PathData pd = track->GetPath(p0, p1, p2, p3);
	Pnt3f n = v - pos;
	n.normalize();
	carSpeed -= Pnt3f::DotProduct(n, Pnt3f(0, 1, 0)) * 3;
	if (carSpeed < 0.3) {
		speed = pd.speed * CTrain::speed0;
	}
	else {
		speed = carSpeed * CTrain::speed0;
	}

	if (carSpeed < 0) {
		carSpeed = 0;
	}
	t += speed;

	if (t >= pd.length) {
		t -= pd.length;

		pd = track->GetNextPath(pd);
		p0 = pd.p0;
		p1 = pd.p1;
		p2 = pd.p2;
		p3 = pd.p3;
	}
}

double getT(PathData& pd, double l) {
	ControlPoint pnt = pd.pointSet[0];
	double len_0, len = 0;
	double t_;
	for (int i = 1; i < pd.pointSet.size(); i++) {
		len_0 = len;
		len += (pd.pointSet[i].pos - pnt.pos).Lenth();
		if (len >= l) {
			return (pnt.inter * (len - l) + pd.pointSet[i].inter * (l - len_0)) / (len - len_0);
		}
		else {
			pnt = pd.pointSet[i];
		}
	}
	return 1;
}

void CTrain::SetNewPos(PathData& pd) {
	ControlPoint qt, qt_1;

	qt   = pd.CalInterpolation(getT(pd, this->t));
	qt_1 = pd.CalInterpolation(getT(pd, this->t + 1));

	pos      = qt.pos + 6 * qt.orient;
	orient   = qt.orient;
	v        = qt_1.pos + 6 * qt_1.orient;
	v_orient = qt_1.orient;

	QVector3D eye(pos.x, pos.y, pos.z);
	QVector3D center(v.x, v.y, v.z);
	QVector3D up(orient.x, orient.y, orient.z);

	modelMatrix.setToIdentity();
	modelMatrix.lookAt(eye, center, up);
	modelMatrix = modelMatrix.inverted();
}

void CTrain::Draw(bool doingShadows, bool isSelected) {
	PathData pd = track->GetPath(p0, p1, p2, p3);
	SetNewPos(pd);

	QVector3D color;
	if (!doingShadows) {
		if (isSelected && type == Head)
			color = QVector3D(1, 1, 0);
		else if (type == Head)
			color = QVector3D(1, 0, 0);
		else
			color = QVector3D(0, 0, 1);
	}

	GLfloat ProjectionMatrex[16];
	GLfloat ViewMatrex[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, ViewMatrex);
	glGetFloatv(GL_PROJECTION_MATRIX, ProjectionMatrex);

	this->model->render(color, ProjectionMatrex, ViewMatrex, modelMatrix);

	for (int i = 0; i < car.size(); i++) {
		CTrain prev = i ? car[i - 1] : *this;
		PathData prevPd = track->GetPath(prev.p0, prev.p1, prev.p2, prev.p3);
		car[i].t = prev.t - OFFSET;
		if (car[i].t < 0) {
			pd = track->GetPath(car[i].p0, car[i].p1, car[i].p2, car[i].p3);
			car[i].t = pd.length + car[i].t;
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
	c.t = train.t - OFFSET;
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
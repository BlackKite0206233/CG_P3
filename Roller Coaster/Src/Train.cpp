#include "Train.h"
#include <windows.h>
#include <GL/gl.h>

#define DIVIDE_LINE 100

bool CTrain::isMove;
double CTrain::speed;

CTrain::CTrain(Pnt3f pos, Pnt3f orient, Pnt3f v): pos(pos), orient(orient), v(v) {
	w = Pnt3f::CrossProduct(v, orient);
	w.normalize();
	t = 0;
}

void CTrain::Move(Pnt3f pos, Pnt3f orient, Pnt3f v) {
	for (int i = car.size() - 1; i >= 1; i--) {
		car[i].SetNewPos(car[i + 1].pos, car[i + 1].orient, car[i + 1].v);
	}
	if (car.size()) {
		car[0].SetNewPos(this->pos, this->orient, this->v);
	}
	SetNewPos(pos, orient, v);
}

void CTrain::SetNewPos(Pnt3f pos, Pnt3f orient, Pnt3f v) {
	this->pos = pos;
	this->orient = orient;
	this->v = v;
	w = Pnt3f::CrossProduct(v, orient);
	w.normalize();
}

void CTrain::Draw(bool doingShadows) {
	if (!doingShadows) {
		glColor3d(1, 1, 1);
	}
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3dv((pos - w * 5 - v * 5 - orient * 5).v());
	glTexCoord2f(1, 0);
	glVertex3dv((pos + w * 5 - v * 5 - orient * 5).v());
	glTexCoord2f(1, 1);
	glVertex3dv((pos + w * 5 + v * 5 - orient * 5).v());
	glTexCoord2f(0, 1);
	glVertex3dv((pos - w * 5 + v * 5 - orient * 5).v());
	glEnd();
}
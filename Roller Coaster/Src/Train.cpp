#include "Train.h"
#include <windows.h>
#include <GL/gl.h>

#define DIVIDE_LINE 100

bool CTrain::isMove;
double CTrain::speed;

CTrain::CTrain(const PathData& pd): currentPath(pd) {
	t = 0;
	
	SetNewPos();
}

void CTrain::Move() {
	t += speed / currentPath.length;

	// for (int i = car.size() - 1; i >= 1; i--) {
	// 	car[i].SetNewPos(car[i + 1].pos, car[i + 1].orient, car[i + 1].v);
	// }
	// if (car.size()) {
	// 	car[0].SetNewPos(this->pos, this->orient, this->v);
	// }
	SetNewPos();
}

void CTrain::SetNewPos() {
	ControlPoint qt, qt_1;
	qt   = currentPath.CalInterpolation(t);
	qt_1 = currentPath.CalInterpolation(t + speed / currentPath.length);

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
		else
			glColor3d(1, 1, 1);
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
}
#include "Train.h"
#include <windows.h>
#include <GL/gl.h>

bool CTrain::isMove;
double CTrain::speed;

CTrain::CTrain(Pnt3f pos, Pnt3f orient, Pnt3f v): pos(pos), orient(orient), v(v) {
	w = Pnt3f::CrossProduct(v, orient);
	w.normalize();
}

void CTrain::Move() {

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
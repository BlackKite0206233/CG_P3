#include "Train.h"
#include <windows.h>
#include <GL/gl.h>

CTrain::CTrain() {

}

void CTrain::Move() {

}

void CTrain::Draw(bool doingShadows) {
	if (!doingShadows) {
		glColor3d(1, 1, 1);
	}
}
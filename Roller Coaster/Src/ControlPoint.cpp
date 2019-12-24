/************************************************************************
     File:        ControlPoint.cpp

     Author:     
                  Michael Gleicher, gleicher@cs.wisc.edu
     Modifier
                  Yu-Chi Lai, yu-chi@cs.wisc.edu
     
     Comment:     Data structure for control points

						These are not just points in space (see Point3D), 
						because they may have
						an orientation associated with them. For most people, 
						the orientation will
						just be ignored. However, if you try some advanced 
						features, you might find the orientation vectors handy.

						I assume the orientation points UP 
						(the positive Y axis), so that's the default.
						When things get drawn, the point "points" in that 
						direction

     Platform:    Visio Studio.Net 2003/2005

*************************************************************************/

#include <windows.h>
#include <GL/gl.h>
#include <math.h>
#include <QMatrix4x4>
#include <QVector4D>
#include <iostream>

#include "ControlPoint.h"
#include "Utilities/3dUtils.h"

using namespace std;
//****************************************************************************
//
// * Default contructor
//============================================================================
ControlPoint::ControlPoint() : pos(0, 0, 0), orient(0, 1, 0), children(set<int>()), parents(set<int>())
//============================================================================
{
}

//****************************************************************************
//
// * Set up the position and set orientation to default (0, 1, 0)
//============================================================================
ControlPoint::ControlPoint(const Pnt3f &_pos) : pos(_pos), orient(0, 1, 0), children(set<int>()), parents(set<int>())
//============================================================================
{
}

//****************************************************************************
//
// * Set up the position and orientation
//============================================================================
ControlPoint::ControlPoint(const Pnt3f &_pos, const Pnt3f &_orient) : pos(_pos), orient(_orient), children(set<int>()), parents(set<int>())
//============================================================================
{
	orient.normalize();
}

//****************************************************************************
//
// * Draw the control point
//============================================================================
void ControlPoint::draw()
//============================================================================
{
	float size = 2.0;

	glPushMatrix();
	glTranslatef(pos.x, pos.y, pos.z);
	float theta1 = -radiansToDegrees(atan2(orient.z, orient.x));
	glRotatef(theta1, 0, 1, 0);
	float theta2 = -radiansToDegrees(acos(orient.y));
	glRotatef(theta2, 0, 0, 1);

	glBegin(GL_QUADS);
	glNormal3f(0, 0, 1);
	glVertex3f(size, size, size);
	glVertex3f(-size, size, size);
	glVertex3f(-size, -size, size);
	glVertex3f(size, -size, size);

	glNormal3f(0, 0, -1);
	glVertex3f(size, size, -size);
	glVertex3f(size, -size, -size);
	glVertex3f(-size, -size, -size);
	glVertex3f(-size, size, -size);

	// no top - it will be the point

	glNormal3f(0, -1, 0);
	glVertex3f(size, -size, size);
	glVertex3f(-size, -size, size);
	glVertex3f(-size, -size, -size);
	glVertex3f(size, -size, -size);

	glNormal3f(1, 0, 0);
	glVertex3f(size, size, size);
	glVertex3f(size, -size, size);
	glVertex3f(size, -size, -size);
	glVertex3f(size, size, -size);

	glNormal3f(-1, 0, 0);
	glVertex3f(-size, size, size);
	glVertex3f(-size, size, -size);
	glVertex3f(-size, -size, -size);
	glVertex3f(-size, -size, size);
	glEnd();
	glBegin(GL_TRIANGLE_FAN);
	glNormal3f(0, 1.0f, 0);
	glVertex3f(0, 3.0f * size, 0);
	glNormal3f(1.0f, 0.0f, 1.0f);
	glVertex3f(size, size, size);
	glNormal3f(-1.0f, 0.0f, 1.0f);
	glVertex3f(-size, size, size);
	glNormal3f(-1.0f, 0.0f, -1.0f);
	glVertex3f(-size, size, -size);
	glNormal3f(1.0f, 0.0f, -1.0f);
	glVertex3f(size, size, -size);
	glNormal3f(1.0f, 0.0f, 1.0f);
	glVertex3f(size, size, size);
	glEnd();
	glPopMatrix();
}

void ControlPoint::setCenter(float x, float y) {
	centerX = x;
	centerY = y;
}

void ControlPoint::getMouseNDC(float mx, float my, float &x, float &y)
//==========================================================================
{

	x = (mx - centerX) / 500;
	if (x < -1) {
		x = -1;
	}
	else if (x > 1) {
		x = 1;
	}
	y = (centerY - my) / 500;
	if (y < -1) {
		y = -1;
	}
	else if (y > 1) {
		y = 1;
	}
}

void ControlPoint::down(const float x, const float y)
//==========================================================================
{
	start = now * start;
	now = Quat(); // identity

	downX = x;
	downY = y;
}

void ControlPoint::getMatrix(HMatrix m) const
//==========================================================================
{
	Quat qAll = now * start;
	qAll = qAll.conjugate(); // since Ken does everything transposed
	qAll.toMatrix(m);
}

static void onUnitSphere(const float mx, const float my, float &x, float &y, float &z)
//==========================================================================
{
	x = mx; // should divide radius
	y = my;
	float mag = x * x + y * y;
	if (mag > 1.0f) {
		float scale = 1.0f / ((float)sqrt(mag));
		x *= scale;
		y *= scale;
		z = 0;
	}
	else {
		z = (float)sqrt(1 - mag);
	}
}

void ControlPoint::computeNow(const float nowX, const float nowY)
//==========================================================================
{
	float dx, dy, dz;
	float mx, my, mz;
	onUnitSphere(downX, downY, dx, dy, dz);
	onUnitSphere(nowX, nowY, mx, my, mz);

	// here we compute the quaternion between these two points
	now.x = dy * mz - dz * my;
	now.y = dz * mx - dx * mz;
	now.z = dx * my - dy * mx;
	now.w = dx * mx + dy * my + dz * mz;

	now.renorm(); // just in case...

	HMatrix m;
	getMatrix(m);
	QMatrix4x4 mat(m[0][0], m[0][1], m[0][2], m[0][3], m[1][0], m[1][1], m[1][2], m[1][3], m[2][0], m[2][1], m[2][2], m[2][3], m[3][0], m[3][1], m[3][2], m[3][3]);
	QVector4D vec(orient.x, orient.y, orient.x, 0);
	vec = mat * vec;
	orient.x = vec.x();
	orient.y = vec.y();
	orient.z = vec.z();
	orient.normalize();
}
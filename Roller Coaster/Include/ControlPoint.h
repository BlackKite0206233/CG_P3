/************************************************************************
     File:        ControlPoint.H

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
#pragma once
#include <set>
#include "Utilities/Pnt3f.h"
#include "Utilities/Quat.h"

using namespace std;

struct CtrlPoint {
	CtrlPoint();
	CtrlPoint(const Pnt3f& pos);
	CtrlPoint(const Pnt3f& pos, const Pnt3f& orient);
	Pnt3f pos;
	Pnt3f orient;
	double inter;
};

class ControlPoint {
public:
	// constructors
	// need a default constructor for making arrays
	ControlPoint();

	// create in a position
	ControlPoint(const Pnt3f &pos);

	// Create in a position and orientation
	ControlPoint(const Pnt3f &pos, const Pnt3f &orient);

	// draw the control point - assumes the color is correct
	void draw();

	void getMouseNDC(float mx, float my, float &x, float &y);
	void getMatrix(HMatrix) const;
	void computeNow(const float nowX, const float nowY);
	void down(const float x, const float y);
	void setCenter(float x, float y);
public:
	CtrlPoint center;
	set<int> children;
	set<int> parents;
	bool visited = false;

	Quat start;
	Quat now;

	float downX;
	float downY;

	float centerX;
	float centerY;
};

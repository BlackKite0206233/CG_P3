#pragma once
#include "ControlPoint.h"
#include <vector>

using namespace std;

class CTrain
{
public:
	CTrain(Pnt3f pos, Pnt3f orient, Pnt3f v);

public:
	void Move();
	void Draw(bool doingShadows);

public:
	static bool isMove;
	static double speed;

	Pnt3f pos;  
	Pnt3f orient;
	Pnt3f v, w;
	vector<CTrain> car;
};


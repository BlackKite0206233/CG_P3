#pragma once
#include "ControlPoint.h"
#include <vector>

using namespace std;

class CTrain
{
public:
	CTrain();

public:
	void Move();
	void Draw(bool doingShadows);

public:
	double speed;
	bool isMove;
	Pnt3f pos;  
	Pnt3f orient;
	vector<CTrain> car;
};


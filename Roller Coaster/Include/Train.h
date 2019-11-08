#pragma once
#include "ControlPoint.h"
#include "Model.h"
#include "Path.h"
#include <vector>

using namespace std;

class CTrain
{
public:
	CTrain(Pnt3f pos, Pnt3f orient, Pnt3f v);

public:
	void Move(Pnt3f pos, Pnt3f orient, Pnt3f v);
	void Draw(bool doingShadows);
	void AddCar();
	void RemoveCar();
	void SetNewPos(Pnt3f pos, Pnt3f orient, Pnt3f v);

public:
	static bool isMove;
	static double speed;

	double t;
	Pnt3f pos;  
	Pnt3f orient;
	Pnt3f v, w;
	vector<CTrain> car;
	Model model;
	PathData currentPath;
};


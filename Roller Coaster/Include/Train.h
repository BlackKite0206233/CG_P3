#pragma once
#include "ControlPoint.h"
#include "Model.h"
#include "Path.h"
#include <vector>

using namespace std;

class CTrain
{
public:
	CTrain(const PathData& pd);

public:
	void Move();
	void Draw(bool doingShadows, bool isSelected);
	void AddCar();
	void RemoveCar();
	void SetNewPos();

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


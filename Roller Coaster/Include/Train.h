#pragma once
#include "ControlPoint.h"
#include "Model.h"
#include "Track.h"
#include <vector>

using namespace std;

enum CarType {
	Head,
	Car,
};

class CTrain
{
public:
	CTrain(CarType type);
	CTrain(int p0, int p1, int p2, int p3, CarType type);

public:
	void Move();
	void Draw(bool doingShadows, bool isSelected);
	void AddCar();
	void RemoveCar();
	void SetNewPos(PathData& pd);

public:
	static bool isMove;
	static double speed0;
	static CTrack* track;

	double speed;
	double t;
	Pnt3f pos;  
	Pnt3f orient;
	Pnt3f v, w;
	vector<CTrain> car;
	Model model;
	int p0, p1, p2, p3;
	CarType type;
};


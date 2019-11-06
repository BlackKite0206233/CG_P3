#pragma once
#include "ControlPoint.h"

class CTrain
{
public:
	CTrain();

public:
	void Move();

public:
	double speed;
	bool isMove;
	ControlPoint poistion;
};


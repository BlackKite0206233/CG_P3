#pragma once
#include "Utilities/3DUtils.h"

class Quat {
public:
	enum QuatPart { X = 0, Y = 1, Z = 2, W = 3, QuatLen };

public:
	Quat();						/* gives the identity */
	Quat(float x, float y, float z, float w);
	Quat(const Quat&);			/* copy constructor */

public:
	// conversions
	void toMatrix(HMatrix) const;

	// operations
	Quat conjugate() const;
	// make multiply look like multiply
	Quat operator* (const Quat&) const;
	// Normalize the quaternion back to length 1
	void renorm();

public:
	// the data
	float x, y, z, w;
};
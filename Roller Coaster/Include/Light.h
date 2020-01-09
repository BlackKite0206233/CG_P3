#pragma once

#include <QVector4D>
#include <QMatrix4x4>

class Light {
public:
	QVector4D position;
	QVector3D ambientColor;
	QVector3D diffuseColor;
	QVector3D specularColor;
	QMatrix4x4 rotationMatrix;

	void Move() {
		position = rotationMatrix * position;
	}
};
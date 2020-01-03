#pragma once

#include <QtGui/QOpenGLFunctions_4_3_Core>
#include <QtGui/QOpenGLVertexArrayObject>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLShader>
#include <QtGui/QOpenGLShaderProgram>

#include "Utilities/Pnt3f.h"
#include "Light.h"

class Water {
public:
	Water(int w, int h);

	void Init();
	void Render(int t, GLfloat* ProjectionMatrix, GLfloat* ViewMatrix, Light& light, QVector3D& eyePos);

private:
	void InitVAO();
	void InitVBO();

	int width;
	int height;

	QOpenGLShaderProgram* shaderProgram;
	QVector<QVector3D> vertices;
	QOpenGLVertexArrayObject* vao;
	QOpenGLBuffer vbo;
};
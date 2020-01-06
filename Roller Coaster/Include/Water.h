#pragma once

#include <QtGui/QOpenGLFunctions_4_3_Core>
#include <QtGui/QOpenGLVertexArrayObject>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLShader>
#include <QtGui/QOpenGLShaderProgram>

#include "Utilities/Pnt3f.h"
#include "Light.h"
#include "WaterFrameBuffer.h"

class Water : protected QOpenGLFunctions_4_3_Core {
public:
	Water(int w, int h);

	void Init();
	void Render(int t, GLfloat* ProjectionMatrix, GLfloat* ViewMatrix, Light& light, QVector3D& eyePos, WaterFrameBuffer& fbo);

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
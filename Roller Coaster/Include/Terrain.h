#pragma once

#include <QtGui/QOpenGLFunctions_4_3_Core>
#include <QtGui/QOpenGLVertexArrayObject>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLShader>
#include <QtGui/QOpenGLShaderProgram>
#include <QtOpenGL>

#include "Utilities/Pnt3f.h"
#include "Light.h"
#include "WaterFrameBuffer.h"

class Terrian : protected QOpenGLFunctions_4_3_Core {
public:
	static int MAX_HEIGHT;
	static int MAX_PIXEL_COLOR;

	Terrian(int w, int h);

	void Init();
	void Render(GLfloat* ProjectionMatrix, GLfloat* ViewMatrix, Light& light, QVector3D& eyePos, QVector<QOpenGLTexture*>& textures, QVector4D clipPlane);

private:
	void GeneratorTerrian();
	void InitVAO();
	void InitVBO();
	//void InitShader();
	
	float getHeight(QImage& image, int x, int y);

	int width;
	int height;

	QOpenGLShaderProgram* shaderProgram;
	QVector<QVector3D> vertices;
	QVector<QVector3D> normals;
	QVector<QVector2D> textureCoords;
	QVector<int> indices;
	QOpenGLVertexArrayObject* vao;
	QOpenGLBuffer vvbo;
	QOpenGLBuffer nvbo;
	QOpenGLBuffer tvbo;
};
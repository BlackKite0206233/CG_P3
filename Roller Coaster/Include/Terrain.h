#pragma once

#include <QtGui/QOpenGLFunctions_4_3_Core>
#include <QtGui/QOpenGLVertexArrayObject>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLShader>
#include <QtGui/QOpenGLShaderProgram>
#include <QtOpenGL>

#include "Utilities/Pnt3f.h"
#include "Light.h"
#include "HeightGenerator.h"
#include "SSAOFrameBuffer.h"

class Terrain : protected QOpenGLFunctions_4_3_Core {
public:
	Terrain(int w, int h);

	void Init();
	void Render(GLfloat* ProjectionMatrix, GLfloat* ViewMatrix, Light& light, QVector3D& eyePos, QVector<QOpenGLTexture*>& textures, SSAOFrameBuffer* ssaoFrameBuffer, int renderMode, QVector4D clipPlane);
	void DrawGeometry(QOpenGLShaderProgram* shader);
	float getHeightOfTerrain(float worldX, float worldY);

private:
	void GeneratorTerrain();
	void InitVAO();
	void InitVBO();
	
	float getHeight(HeightGenerator& generator, int x, int y);
	QVector3D calculateNormal(int x, int y);

	int width;
	int height;

	QVector<QVector<float>> terrainHeight;

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
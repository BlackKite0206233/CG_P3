#pragma once

#include <QtGui/QOpenGLFunctions_4_3_Core>
#include <QtGui/QOpenGLVertexArrayObject>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLShader>
#include <QtGui/QOpenGLShaderProgram>
#include <QVector>
#include <QVector3D>
#include <QFileInfo>
#include <QDebug>
#include <QString>
#include <QGLWidget>
#include <string>

using namespace std;

class SkyBox: protected QOpenGLFunctions_4_3_Core {
private:
	static float ROTATION_SPEED;
public:
	SkyBox();

	void Init();
	void Rotate(float t);
	void Render(GLfloat *ProjectionMatrix, GLfloat *viewMatrix, QVector3D& fogColor, QVector4D& lightPos);

private:
	void InitVAO();
	void InitVBO();
	GLuint InitTexture(string dir, GLenum texID);

	QOpenGLShaderProgram *shaderProgram;
	QOpenGLVertexArrayObject vao;
	GLuint day;
	GLuint night;
	QVector<QVector3D> vertices;
	QOpenGLBuffer vbo;

	float rotation;
};
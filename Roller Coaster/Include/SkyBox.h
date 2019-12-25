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

class SkyBox {
public:
	SkyBox();

	void Init();
	void Begin();
	void Render(GLfloat *ProjectionMatrix, GLfloat *viewMatrix);
	void End();

private:
	void InitVAO();
	void InitVBO();
	void InitTexture();

	QOpenGLShaderProgram *shaderProgram;
	QOpenGLVertexArrayObject vao;
	GLuint texture;
	QVector<QVector3D> vertices;
	QOpenGLBuffer vbo;
};
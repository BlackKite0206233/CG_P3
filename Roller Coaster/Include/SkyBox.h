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
	void Render(GLfloat* ProjectionMatrix, GLfloat* ModelViewMatrix);

private:
	void InitVAO();
	void InitTexture();
	void InitShader(QString vertexShaderPath, QString fragmentShaderPath);

	QOpenGLShaderProgram* shaderProgram;
	QOpenGLShader* vertexShader;
	QOpenGLShader* fragmentShader;
	QOpenGLVertexArrayObject vao;
	GLuint texture;
};
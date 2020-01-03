#include "Water.h"
#include <windows.h>
#include <GL/gl.h>
#include <math.h>
#include <QMatrix4x4>
#include <QVector4D>
#include "Utilities/3dUtils.h"

Water::Water(int w, int h) {
	width = w;
	height = h;
	vao = new QOpenGLVertexArrayObject();
}

void Water::Init() {
	shaderProgram = InitShader("./Shader/Water.vs", "./Shader/Water.fs");
	InitVAO();
	InitVBO();
}

void Water::InitVAO() {
	vao->create();
	vao->bind();
}

void Water::InitVBO() {
	vertices 
		<< QVector3D(-1, 0, -1)
		<< QVector3D(-1, 0,  1)
		<< QVector3D( 1, 0, -1)
		<< QVector3D( 1, 0, -1)
		<< QVector3D(-1, 0,  1)
		<< QVector3D( 1, 0,  1);

	vbo.create();
	vbo.bind();
	vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
	vbo.allocate(vertices.constData(), vertices.size() * sizeof(QVector3D));
}

void Water::Render(int t, GLfloat* ProjectionMatrix, GLfloat* ViewMatrix, Light& light, QVector3D& eyePos) {
	GLfloat P[4][4];
	GLfloat V[4][4];
	DimensionTransformation(ProjectionMatrix, P);
	DimensionTransformation(ViewMatrix, V);

	//Bind the shader we want to draw with
	shaderProgram->bind();
	//Bind the VAO we want to draw
	vao->bind();

	//pass projection matrix to shader
	shaderProgram->setUniformValue("ProjectionMatrix", P);
	//pass modelview matrix to shader
	shaderProgram->setUniformValue("ViewMatrix", V);

	shaderProgram->setUniformValue("color_ambient", light.ambientColor);
	shaderProgram->setUniformValue("color_diffuse", light.diffuseColor);
	shaderProgram->setUniformValue("color_specular", light.specularColor);
	shaderProgram->setUniformValue("light_position", light.position);
	shaderProgram->setUniformValue("eye_position", eyePos);

	shaderProgram->setUniformValue("width", GLfloat(width));
	shaderProgram->setUniformValue("height", GLfloat(height));

	vbo.bind();
	shaderProgram->enableAttributeArray(0);
	shaderProgram->setAttributeArray(0, GL_FLOAT, 0, 3, NULL);
	vbo.release();

	glDrawArrays(GL_TRIANGLES, 0, vertices.size());
	//Disable Attribute 0&1
	shaderProgram->disableAttributeArray(0);
	shaderProgram->disableAttributeArray(1);

	//unbind vao
	vao->release();
	//unbind vao
	shaderProgram->release();
}
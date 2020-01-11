#include "Water.h"
#include <windows.h>
#include <GL/gl.h>
#include <math.h>
#include <QMatrix4x4>
#include <QVector4D>
#include "Utilities/3dUtils.h"

double Water::WAVE_SPEED = 0.003;

Water::Water(int w, int h) {
	initializeOpenGLFunctions();
	width = w;
	height = h;
	moveFactor = 0;
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

void Water::Render(double t, GLfloat* ProjectionMatrix, GLfloat* ViewMatrix, Light& light, QVector3D& eyePos, WaterFrameBuffer& fbo, QVector<QOpenGLTexture*>& textures) {
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

	shaderProgram->setUniformValue("reflectionTexture", 0);
	shaderProgram->setUniformValue("refractionTexture", 1);
	shaderProgram->setUniformValue("dudvMap", 2);
	shaderProgram->setUniformValue("normalMap", 3);
	shaderProgram->setUniformValue("depthMap", 4);

	moveFactor += WAVE_SPEED * t;
	moveFactor = fmod(moveFactor, 1);
	shaderProgram->setUniformValue("moveFactor", GLfloat(moveFactor));

	shaderProgram->setUniformValue("color_specular", light.specularColor);
	shaderProgram->setUniformValue("light_position", light.position);
	shaderProgram->setUniformValue("eye_position", eyePos);

	shaderProgram->setUniformValue("width", GLfloat(width));
	shaderProgram->setUniformValue("height", GLfloat(height));

	shaderProgram->setUniformValue("near", GLfloat(0.1));
	shaderProgram->setUniformValue("far", GLfloat(1e10));

	vbo.bind();
	shaderProgram->enableAttributeArray(0);
	shaderProgram->setAttributeArray(0, GL_FLOAT, 0, 3, NULL);
	vbo.release();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, fbo.getReflectionTexture());
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, fbo.getRefractionTexture());
	glActiveTexture(GL_TEXTURE2);
	textures[1]->bind();
	glActiveTexture(GL_TEXTURE3);
	textures[2]->bind();
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, fbo.getRefractionDepthTexture());

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());
	glDisable(GL_BLEND);
	//Disable Attribute 0&1
	shaderProgram->disableAttributeArray(0);
	shaderProgram->disableAttributeArray(1);

	//unbind vao
	vao->release();
	//unbind vao
	shaderProgram->release();
}
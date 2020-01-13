/************************************************************************
     File:        ControlPoint.cpp

     Author:     
                  Michael Gleicher, gleicher@cs.wisc.edu
     Modifier
                  Yu-Chi Lai, yu-chi@cs.wisc.edu
     
     Comment:     Data structure for control points

						These are not just points in space (see Point3D), 
						because they may have
						an orientation associated with them. For most people, 
						the orientation will
						just be ignored. However, if you try some advanced 
						features, you might find the orientation vectors handy.

						I assume the orientation points UP 
						(the positive Y axis), so that's the default.
						When things get drawn, the point "points" in that 
						direction

     Platform:    Visio Studio.Net 2003/2005

*************************************************************************/

#include <windows.h>
#include <GL/gl.h>
#include <math.h>
#include <QMatrix4x4>
#include <QVector4D>
#include <iostream>

#include "ControlPoint.h"
#include "Utilities/3dUtils.h"

using namespace std;

CtrlPoint::CtrlPoint() : pos(0, 0, 0), orient(0, 1, 0) {
}

CtrlPoint::CtrlPoint(Pnt3f& _pos) : pos(_pos), orient(0, 1, 0) {
}

CtrlPoint::CtrlPoint(Pnt3f& _pos, Pnt3f& _orient) : pos(_pos), orient(_orient) {
	orient.normalize();
}
//****************************************************************************
//
// * Default contructor
//============================================================================
ControlPoint::ControlPoint() : center(), children(set<int>()), parents(set<int>())
//============================================================================
{
	vao = new QOpenGLVertexArrayObject();
	Init();
}

//****************************************************************************
//
// * Set up the position and set orientation to default (0, 1, 0)
//============================================================================
ControlPoint::ControlPoint(Pnt3f &_pos) : center(_pos), children(set<int>()), parents(set<int>())
//============================================================================
{
	vao = new QOpenGLVertexArrayObject();
	Init();
}

//****************************************************************************
//
// * Set up the position and orientation
//============================================================================
ControlPoint::ControlPoint(Pnt3f &_pos, Pnt3f &_orient) : center(_pos, _orient), children(set<int>()), parents(set<int>())
//============================================================================
{
	vao = new QOpenGLVertexArrayObject();
	Init();
}

//****************************************************************************
//
// * Draw the control point
//============================================================================
void ControlPoint::draw(QVector3D color, GLfloat* ProjectionMatrix, GLfloat* ViewMatrix, Light& light, QVector3D& eyePos, SSAOFrameBuffer* ssaoFrameBuffer, QVector4D& clipPlane)
//============================================================================
{
	ModelMatrix.setToIdentity();
	ModelMatrix.translate(center.pos.x, center.pos.y, center.pos.z);
	float theta1 = -radiansToDegrees(atan2(center.orient.z, center.orient.x));
	ModelMatrix.rotate(theta1, 0, 1, 0);
	float theta2 = -radiansToDegrees(acos(center.orient.y));
	ModelMatrix.rotate(theta2, 0, 0, 1);

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
	shaderProgram->setUniformValue("ModelMatrix", ModelMatrix);

	shaderProgram->setUniformValue("color_ambient", light.ambientColor);
	shaderProgram->setUniformValue("color_diffuse", light.diffuseColor);
	shaderProgram->setUniformValue("color_specular", light.specularColor);
	shaderProgram->setUniformValue("light_position", light.position);
	shaderProgram->setUniformValue("eye_position", eyePos);
	shaderProgram->setUniformValue("clipPlane", clipPlane);

	shaderProgram->setUniformValue("Color", color);

	shaderProgram->setUniformValue("Scale", GLfloat(2.0));

	shaderProgram->setUniformValue("ssaoColorBufferBlur", 0);

	vvbo.bind();
	shaderProgram->enableAttributeArray(0);
	shaderProgram->setAttributeArray(0, GL_FLOAT, 0, 3, NULL);
	vvbo.release();

	nvbo.bind();
	shaderProgram->enableAttributeArray(1);
	shaderProgram->setAttributeArray(1, GL_FLOAT, 0, 3, NULL);
	nvbo.release();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ssaoFrameBuffer.getBlurTexture());

	//Draw a triangle with 3 indices starting from the 0th index
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());
	//Disable Attribute 0&1
	shaderProgram->disableAttributeArray(0);
	shaderProgram->disableAttributeArray(1);

	//unbind vao
	vao->release();
	//unbind vao
	shaderProgram->release();
}

void ControlPoint::DrawGeometry(QOpenGLShaderProgram* shader) {
	ModelMatrix.setToIdentity();
	ModelMatrix.translate(center.pos.x, center.pos.y, center.pos.z);
	float theta1 = -radiansToDegrees(atan2(center.orient.z, center.orient.x));
	ModelMatrix.rotate(theta1, 0, 1, 0);
	float theta2 = -radiansToDegrees(acos(center.orient.y));
	ModelMatrix.rotate(theta2, 0, 0, 1);

	vao->bind();

	shader->setUniformValue("ModelMatrix", ModelMatrix);
	shader->setUniformValue("scale", GLfloat(2.0));

	vvbo.bind();
	shader->enableAttributeArray(0);
	shader->setAttributeArray(0, GL_FLOAT, 0, 3, NULL);
	vvbo.release();

	nvbo.bind();
	shader->enableAttributeArray(1);
	shader->setAttributeArray(1, GL_FLOAT, 0, 3, NULL);
	nvbo.release();

	glDrawArrays(GL_TRIANGLES, 0, vertices.size());

	shader->disableAttributeArray(0);
	shader->disableAttributeArray(1);
	vao->release();
}

void ControlPoint::setCenter(float x, float y) {
	centerX = x;
	centerY = y;
}

void ControlPoint::getMouseNDC(float mx, float my, float &x, float &y)
//==========================================================================
{

	x = (mx - centerX) / 500;
	if (x < -1) {
		x = -1;
	}
	else if (x > 1) {
		x = 1;
	}
	y = (centerY - my) / 500;
	if (y < -1) {
		y = -1;
	}
	else if (y > 1) {
		y = 1;
	}
}

void ControlPoint::down(const float x, const float y)
//==========================================================================
{
	start = now * start;
	now = Quat(); // identity

	downX = x;
	downY = y;
}

void ControlPoint::getMatrix(HMatrix m) const
//==========================================================================
{
	Quat qAll = now * start;
	qAll = qAll.conjugate(); // since Ken does everything transposed
	qAll.toMatrix(m);
}

static void onUnitSphere(const float mx, const float my, float &x, float &y, float &z)
//==========================================================================
{
	x = mx; // should divide radius
	y = my;
	float mag = x * x + y * y;
	if (mag > 1.0f) {
		float scale = 1.0f / ((float)sqrt(mag));
		x *= scale;
		y *= scale;
		z = 0;
	}
	else {
		z = (float)sqrt(1 - mag);
	}
}

void ControlPoint::computeNow(const float nowX, const float nowY)
//==========================================================================
{
	float dx, dy, dz;
	float mx, my, mz;
	onUnitSphere(downX, downY, dx, dy, dz);
	onUnitSphere(nowX, nowY, mx, my, mz);

	// here we compute the quaternion between these two points
	now.x = dy * mz - dz * my;
	now.y = dz * mx - dx * mz;
	now.z = dx * my - dy * mx;
	now.w = dx * mx + dy * my + dz * mz;

	now.renorm(); // just in case...

	HMatrix m;
	getMatrix(m);
	QMatrix4x4 mat(m[0][0], m[0][1], m[0][2], m[0][3], m[1][0], m[1][1], m[1][2], m[1][3], m[2][0], m[2][1], m[2][2], m[2][3], m[3][0], m[3][1], m[3][2], m[3][3]);
	QVector4D vec(center.orient.x, center.orient.y, center.orient.x, 0);
	vec = mat * vec;
	center.orient.x = vec.x();
	center.orient.y = vec.y();
	center.orient.z = vec.z();
	center.orient.normalize();
}


void ControlPoint::Init() {
	shaderProgram = InitShader("./Shader/Model.vs", "./Shader/Model.fs");
	InitVAO();
	InitVBO();
}

void ControlPoint::InitVAO() {
	vao->create();
	vao->bind();
}

void ControlPoint::InitVBO() {
	vertices 
		<< QVector3D( 1,  1, 1)
		<< QVector3D(-1,  1, 1)
		<< QVector3D(-1, -1, 1)
		<< QVector3D(-1, -1, 1)
		<< QVector3D( 1, -1, 1)
		<< QVector3D( 1,  1, 1)

		<< QVector3D( 1,  1, -1)
		<< QVector3D(-1, -1, -1)
		<< QVector3D(-1,  1, -1)
		<< QVector3D(-1, -1, -1)
		<< QVector3D( 1,  1, -1)
		<< QVector3D( 1, -1, -1)

		<< QVector3D( 1, -1,  1)
		<< QVector3D(-1, -1,  1)
		<< QVector3D(-1, -1, -1)
		<< QVector3D(-1, -1, -1)
		<< QVector3D( 1, -1, -1)
		<< QVector3D( 1, -1,  1)

		<< QVector3D(1,  1,  1)
		<< QVector3D(1, -1,  1)
		<< QVector3D(1, -1, -1)
		<< QVector3D(1, -1, -1)
		<< QVector3D(1,  1, -1)
		<< QVector3D(1,  1,  1)

		<< QVector3D(-1,  1,  1)
		<< QVector3D(-1, -1, -1)
		<< QVector3D(-1, -1,  1)
		<< QVector3D(-1, -1, -1)
		<< QVector3D(-1,  1,  1)
		<< QVector3D(-1,  1, -1)

		<< QVector3D(0, 3, 0)
		<< QVector3D(-1, 1, 1)
		<< QVector3D(1, 1, 1)

		<< QVector3D(0, 3, 0)
		<< QVector3D(-1, 1, -1)
		<< QVector3D(-1, 1, 1)

		<< QVector3D(0, 3, 0)
		<< QVector3D(1, 1, -1)
		<< QVector3D(-1, 1, -1)

		<< QVector3D(0, 3, 0)
		<< QVector3D(1, 1, 1)
		<< QVector3D(1, 1, -1);

	vvbo.create();
	vvbo.bind();
	vvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
	vvbo.allocate(vertices.constData(), vertices.size() * sizeof(QVector3D));

	QVector3D triNorm0 = QVector3D::crossProduct(QVector3D(-1, 1, 1) - QVector3D(0, 3, 0), QVector3D(1, 1, 1) - QVector3D(0, 3, 0));
	QVector3D triNorm1 = QVector3D::crossProduct(QVector3D(-1, 1, -1) - QVector3D(0, 3, 0), QVector3D(-1, 1, 1) - QVector3D(0, 3, 0));
	QVector3D triNorm2 = QVector3D::crossProduct(QVector3D(1, 1, -1) - QVector3D(0, 3, 0), QVector3D(-1, 1, -1) - QVector3D(0, 3, 0));
	QVector3D triNorm3 = QVector3D::crossProduct(QVector3D(1, 1, 1) - QVector3D(0, 3, 0), QVector3D(1, 1, -1) - QVector3D(0, 3, 0));

	normals
		<< QVector3D(0, 0, 1)
		<< QVector3D(0, 0, 1)
		<< QVector3D(0, 0, 1)
		<< QVector3D(0, 0, 1)
		<< QVector3D(0, 0, 1)
		<< QVector3D(0, 0, 1)

		<< QVector3D(0, 0, -1)
		<< QVector3D(0, 0, -1)
		<< QVector3D(0, 0, -1)
		<< QVector3D(0, 0, -1)
		<< QVector3D(0, 0, -1)
		<< QVector3D(0, 0, -1)

		<< QVector3D(0, -1, 0)
		<< QVector3D(0, -1, 0)
		<< QVector3D(0, -1, 0)
		<< QVector3D(0, -1, 0)
		<< QVector3D(0, -1, 0)
		<< QVector3D(0, -1, 0)

		<< QVector3D(1, 0, 0)
		<< QVector3D(1, 0, 0)
		<< QVector3D(1, 0, 0)
		<< QVector3D(1, 0, 0)
		<< QVector3D(1, 0, 0)
		<< QVector3D(1, 0, 0)

		<< QVector3D(-1, 0, 0)
		<< QVector3D(-1, 0, 0)
		<< QVector3D(-1, 0, 0)
		<< QVector3D(-1, 0, 0)
		<< QVector3D(-1, 0, 0)
		<< QVector3D(-1, 0, 0)

		<< triNorm0
		<< triNorm0
		<< triNorm0

		<< triNorm1
		<< triNorm1
		<< triNorm1

		<< triNorm2
		<< triNorm2
		<< triNorm2

		<< triNorm3
		<< triNorm3
		<< triNorm3;

	nvbo.create();
	nvbo.bind();
	nvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
	nvbo.allocate(normals.constData(), normals.size() * sizeof(QVector3D));
}
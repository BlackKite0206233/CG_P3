#include "SkyBox.h"
#include <QImage>
#include <iostream>
#include <vector>
#include "Utilities/3DUtils.h"
#include <QMatrix4x4>

using namespace std;

float SkyBox::ROTATION_SPEED = 0.1;

SkyBox::SkyBox() {
	initializeOpenGLFunctions();
	rotation = 0;
	vertices 
		<< QVector3D(-1.0f, -1.0f,  1.0f)
		<< QVector3D(-1.0f, -1.0f, -1.0f)
		<< QVector3D(-1.0f,  1.0f, -1.0f)
		<< QVector3D(-1.0f,  1.0f, -1.0f)
		<< QVector3D(-1.0f,  1.0f,  1.0f)
		<< QVector3D(-1.0f, -1.0f,  1.0f)

		<< QVector3D(1.0f, -1.0f, -1.0f)
		<< QVector3D(1.0f, -1.0f,  1.0f)
		<< QVector3D(1.0f,  1.0f,  1.0f)
		<< QVector3D(1.0f,  1.0f,  1.0f)
		<< QVector3D(1.0f,  1.0f, -1.0f)
		<< QVector3D(1.0f, -1.0f, -1.0f)

		<< QVector3D(-1.0f,  1.0f, -1.0f)
		<< QVector3D(-1.0f, -1.0f, -1.0f)
		<< QVector3D( 1.0f, -1.0f, -1.0f)
		<< QVector3D( 1.0f, -1.0f, -1.0f)
		<< QVector3D( 1.0f,  1.0f, -1.0f)
		<< QVector3D(-1.0f,  1.0f, -1.0f)

		<< QVector3D(-1.0f, -1.0f, 1.0f)
		<< QVector3D(-1.0f,  1.0f, 1.0f)
		<< QVector3D( 1.0f,  1.0f, 1.0f)
		<< QVector3D( 1.0f,  1.0f, 1.0f)
		<< QVector3D( 1.0f, -1.0f, 1.0f)
		<< QVector3D(-1.0f, -1.0f, 1.0f)

		<< QVector3D(-1.0f, -1.0f, -1.0f)
		<< QVector3D(-1.0f, -1.0f,  1.0f)
		<< QVector3D( 1.0f, -1.0f, -1.0f)
		<< QVector3D( 1.0f, -1.0f, -1.0f)
		<< QVector3D(-1.0f, -1.0f,  1.0f)
		<< QVector3D( 1.0f, -1.0f,  1.0f)

		<< QVector3D(-1.0f, 1.0f, -1.0f)
		<< QVector3D( 1.0f, 1.0f, -1.0f)
		<< QVector3D( 1.0f, 1.0f,  1.0f)
		<< QVector3D( 1.0f, 1.0f,  1.0f)
		<< QVector3D(-1.0f, 1.0f,  1.0f)
		<< QVector3D(-1.0f, 1.0f, -1.0f);
}

void SkyBox::Init() {
    InitVAO();
		InitVBO();
    day = InitTexture("Day", GL_TEXTURE0);
		night = InitTexture("Night", GL_TEXTURE1);
		shaderProgram = InitShader("./Shader/SkyBox.vs", "./Shader/SkyBox.fs");
}

void SkyBox::Rotate(float t) {
	rotation += ROTATION_SPEED * t;
}

void SkyBox::Render(GLfloat *ProjectionMatrix, GLfloat *viewMatrix, QVector3D& fogColor, QVector4D& lightPos) {
		glDepthMask(GL_FALSE);

		shaderProgram->bind();
		vao.bind();
    
		GLfloat P[4][4];
    GLfloat V[4][4];
    DimensionTransformation(ProjectionMatrix, P);
    DimensionTransformation(viewMatrix, V);

		QMatrix4x4 M;
		M.setToIdentity();
		M.rotate(rotation, 0, 1, 0);
		M.rotate(180, 1, 0, 0);

    shaderProgram->setUniformValue("projection", P);
    shaderProgram->setUniformValue("view", V);
		shaderProgram->setUniformValue("model", M);

		shaderProgram->setUniformValue("fogColor", fogColor);
		shaderProgram->setUniformValue("day", 0);
		shaderProgram->setUniformValue("night", 1);

		float blendFactor;
		if (lightPos.y() < 20000 && lightPos.y() > -20000) {
			blendFactor = (lightPos.y() + 20000) / 40000.0;
		}
		else if (lightPos.y() > 20000) {
			blendFactor = 1;
		}
		else {
			blendFactor = 0;
		}
		shaderProgram->setUniformValue("blendFactor", blendFactor);

		vbo.bind();
		shaderProgram->enableAttributeArray(0);
		shaderProgram->setAttributeArray(0, GL_FLOAT, 0, 3, NULL);
		vbo.release();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, day);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, night);

    glDrawArrays(GL_TRIANGLES, 0, 36);

		shaderProgram->disableAttributeArray(0);

		vao.release();
		shaderProgram->release();
		glDepthMask(GL_TRUE);
}

void SkyBox::InitVAO() {
    vao.create();
    vao.bind();
}

void SkyBox::InitVBO() {
	vbo.create();
	vbo.bind();
	vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
	vbo.allocate(vertices.constData(), vertices.size() * sizeof(QVector3D));
}

GLuint SkyBox::InitTexture(string dir, GLenum texID) {
		GLuint texture;
    vector<std::string> faces({"right",
                               "left",
                               "bottom",
                               "top",
                               "back",
                               "front"});
    glGenTextures(1, &texture);
		glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

    for (unsigned int i = 0; i < faces.size(); i++) {
				QImage tex = QImage(("./Textures/" + dir +  "/" + faces[i] + ".png").c_str());
        tex = QGLWidget::convertToGLFormat(tex);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, tex.width(), tex.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, tex.bits());
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		return texture;
}
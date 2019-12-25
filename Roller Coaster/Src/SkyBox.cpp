#include "SkyBox.h"
#include <QImage>
#include <iostream>
#include <vector>
#include "Utilities/3DUtils.h"
#include <QMatrix4x4>

using namespace std;

SkyBox::SkyBox() {
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
    InitTexture();
    InitShader("./Shader/SkyBox.vs", "./Shader/SkyBox.fs");
}

void SkyBox::Begin() {
	glDepthMask(GL_FALSE);

	shaderProgram->bind();
	vao.bind();
}

void SkyBox::End() {
	shaderProgram->disableAttributeArray(0);
	shaderProgram->disableAttributeArray(1);

	vao.release();
	shaderProgram->release();
	glDepthMask(GL_TRUE);
}

void SkyBox::Render(GLfloat *ProjectionMatrix, GLfloat *viewMatrix) {
    GLfloat P[4][4];
    GLfloat V[4][4];
    DimensionTransformation(ProjectionMatrix, P);
    DimensionTransformation(viewMatrix, V);

		QMatrix4x4 M;
		M.setToIdentity();
		M.rotate(180, 1, 0, 0);

    shaderProgram->setUniformValue("projection", P);
    shaderProgram->setUniformValue("view", V);
		shaderProgram->setUniformValue("model", M);

		vbo.bind();
		shaderProgram->enableAttributeArray(0);
		shaderProgram->setAttributeArray(0, GL_FLOAT, 0, 3, NULL);
		vbo.release();

    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

    glDrawArrays(GL_TRIANGLES, 0, 36);
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

void SkyBox::InitTexture()
{
    vector<std::string> faces({"RT",
                               "LF",
                               "DN",
                               "UP",
                               "FR",
                               "BK"});

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

    for (unsigned int i = 0; i < faces.size(); i++) {
        QImage tex = QImage(("./Textures/ct_" + faces[i] + ".png").c_str());
        tex = QGLWidget::convertToGLFormat(tex);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, tex.width(), tex.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, tex.bits());
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void SkyBox::InitShader(QString vertexShaderPath, QString fragmentShaderPath) {
    shaderProgram = new QOpenGLShaderProgram();
    QFileInfo vertexShaderFile(vertexShaderPath);
    if (vertexShaderFile.exists()) {
        vertexShader = new QOpenGLShader(QOpenGLShader::Vertex);
        if (vertexShader->compileSourceFile(vertexShaderPath))
            shaderProgram->addShader(vertexShader);
        else
            qWarning() << "Vertex Shader Error " << vertexShader->log();
    }
    else
        qDebug() << vertexShaderFile.filePath() << " can't be found";

    QFileInfo fragmentShaderFile(fragmentShaderPath);
    if (fragmentShaderFile.exists()) {
        fragmentShader = new QOpenGLShader(QOpenGLShader::Fragment);
        if (fragmentShader->compileSourceFile(fragmentShaderPath))
            shaderProgram->addShader(fragmentShader);
        else
            qWarning() << "fragment Shader Error " << fragmentShader->log();
    }
    else
        qDebug() << fragmentShaderFile.filePath() << " can't be found";
    shaderProgram->link();
}
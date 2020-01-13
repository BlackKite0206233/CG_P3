#pragma once

#include <QtGui/QOpenGLFunctions_4_3_Core>
#include <QtGui/QOpenGLVertexArrayObject>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLShader>
#include <QtGui/QOpenGLShaderProgram>
#include <QtWidgets/QWidget>
#include <QtWidgets/QMainWindow>
#include <random>

#include "SSAOFrameBuffer.h"

class SSAO: protected QOpenGLFunctions_4_3_Core {
public:
    SSAO();

    void Init();
    void GeometryShaderBegin(GLfloat* ProjectionMatrix, GLfloat* ViewMatrix);
    void GeometryShaderEnd();

    void SSAOPass(GLfloat* ProjectionMatrix, SSAOFrameBuffer& fbo, int width, int height);
    void BlurPass(SSAOFrameBuffer& fbo);

    QOpenGLShaderProgram* geometryShaderProgram;
    QOpenGLShaderProgram* SSAOShaderProgram;
    QOpenGLShaderProgram* blurShaderProgram;
    QOpenGLShaderProgram* currentShader;

private:
    void InitVAO();
	void InitVBO();
    void CreateNoiseTextureAttachment();
    void CreateKernel();

    uniform_real_distribution<GLfloat> randomFloat;
    default_random_engine generator;

    QVector<Qvector3D> ssaoKernel;

    GLuint noiseTexture;

    QVector<QVector3D> vertices;
    QVector<QVector2D> textureCoords;
	QOpenGLVertexArrayObject* vao;
	QOpenGLBuffer vbo;
}
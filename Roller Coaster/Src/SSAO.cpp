#include "SSAO.h"
#include "Utilities/3dUtils.h"
#include <sstream>
#include <string>

using namespace std;

SSAO::SSAO() {
    initializeOpenGLFunctions();
    vao = new QOpenGLVertexArrayObject();
    randomFloat = uniform_real_distribution<GLfloat>(0, 1);
}

void SSAO::Init() {
    geometryShaderProgram = InitShader("./Shader/SSAO/Geometry.vs", "./Shader/SSAO/Geometry.fs");
    SSAOShaderProgram = InitShader("./Shader/SSAO/SSAO.vs", "./Shader/SSAO/SSAO.fs");
    blurShaderProgram = InitShader("./Shader/SSAO/SSAO.vs", "./Shader/SSAO/Blur.fs");

    InitVAO();
    InitVBO();
    CreateNoiseTextureAttachment();
    CreateKernel();
}

void SSAO::InitVAO() {
    vao->create();
    vao->bind();
}

void SSAO::InitVBO() {
    vertices
        << QVector3D(-1,  1, 0)
        << QVector3D(-1, -1, 0)
        << QVector3D( 1,  1, 0)
        << QVector3D( 1, -1, 0);

    vbo.create();
    vbo.bind();
    vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    vbo.allocate(vertices.constData(), vertices.size() * sizeof(QVector3D));
}

void SSAO::CreateNoiseTextureAttachment() {
    QVector<QVector3D> ssaoNoise;
    for (int i = 0; i < 16; i++) {
        QVector3D noise(randomFloat(generator) * 2.0 - 1.0, randomFloat(generator) * 2.0 - 1.0, 0.0f);
        ssaoNoise << noise;
    }
    glGenTextures(1, &noiseTexture);
    glBindTexture(GL_TEXTURE_2D, noiseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

float lerp(float a, float b, float f) {
    return a + f * (b - a);
}

void SSAO::CreateKernel() {
    for (int i = 0; i < 64; ++i) {
        QVector3D sample(randomFloat(generator) * 2.0 - 1.0, randomFloat(generator) * 2.0 - 1.0, randomFloat(generator));
        sample.normalize();
        sample *= randomFloat(generator);
        float scale = float(i) / 64.0;

        scale = lerp(0.1f, 1.0f, scale * scale);
        sample *= scale;
        ssaoKernel << sample;
    }
}

void SSAO::GeometryShaderBegin(GLfloat *ProjectionMatrix, GLfloat *ViewMatrix) {
    currentShader = geometryShaderProgram;
    GLfloat P[4][4];
    GLfloat V[4][4];
    DimensionTransformation(ProjectionMatrix, P);
    DimensionTransformation(ViewMatrix, V);

    currentShader->bind();

    currentShader->setUniformValue("ProjectionMatrix", P);
    currentShader->setUniformValue("ViewMatrix", V);
}

void SSAO::GeometryShaderEnd() {
    currentShader->release();
}

void SSAO::SSAOPass(GLfloat *ProjectionMatrix, SSAOFrameBuffer &fbo, int width, int height) {
    currentShader = SSAOShaderProgram;
    GLfloat P[4][4];
    DimensionTransformation(ProjectionMatrix, P);

    currentShader->bind();
    vao->bind();

    currentShader->setUniformValue("projection", P);

    currentShader->setUniformValue("gPosition", 0);
    currentShader->setUniformValue("gNormal", 1);
    currentShader->setUniformValue("texNoise", 2);

    stringstream ss;
    string str;
    for (int i = 0; i < ssaoKernel.size(); i++) {
        ss.clear();
        ss << "samples[" << i << "]";
        ss >> str;
        currentShader->setUniformValue(str.c_str(), ssaoKernel[i]);
    }

    currentShader->setUniformValue("noiseScale", QVector2D(width, height));
    currentShader->setUniformValue("kernelSize", GLfloat(ssaoKernel.size()));

    vbo.bind();
    currentShader->enableAttributeArray(0);
    currentShader->setAttributeArray(0, GL_FLOAT, 0, 3, NULL);
    vbo.release();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fbo.getPositionTexture());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, fbo.getNormalTexture());
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, noiseTexture);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, vertices.size());

    currentShader->disableAttributeArray(0);

    vao->release();
    currentShader->release();
}

void SSAO::BlurPass(SSAOFrameBuffer &fbo) {
    currentShader = blurShaderProgram;

    currentShader->bind();
    vao->bind();

    currentShader->setUniformValue("ssaoInput", 0);

    vbo.bind();
    currentShader->enableAttributeArray(0);
    currentShader->setAttributeArray(0, GL_FLOAT, 0, 3, NULL);
    vbo.release();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fbo.getSSAOTexture());

    glDrawArrays(GL_TRIANGLE_STRIP, 0, vertices.size());

    currentShader->disableAttributeArray(0);

    vao->release();
    currentShader->release();
}
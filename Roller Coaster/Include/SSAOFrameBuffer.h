#pragma once

#include <QtGui/QOpenGLFunctions_4_3_Core>
#include <QtGui/QOpenGLVertexArrayObject>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLShader>
#include <QtGui/QOpenGLShaderProgram>
#include <QtWidgets/QWidget>
#include <QtWidgets/QMainWindow>

class SSAOFrameBuffer: protected QOpenGLFunctions_4_3_Core {
public:
    static int WIDTH;
    static int HEIGHT;
public:
    SSAOFrameBuffer(QWidget *w);

    void CleanUp();
    void BindGeometryFrameBuffer();
    void BindSSAOFrameBuffer();
    void BindBlurFrameBuffer();
    void UnbindCurrentFrameBuffer();

    GLuint CreateNoiseTextureAttachment(int width, int height);

    GLuint getPositionTexture();
    GLuint getNormalTexture();
    GLuint getAlbedoTexture();
    GLuint getSSAOTexture();
    GLuint getBlurTexture();

private:
    void initGeometryFrameBuffer();
    void initSSAOFrameBuffer();
    void initBlurFrameBuffer();

    void bindFrameBuffer(GLuint framebuffer, int width, int height);

    GLuint createFrameBuffer();
    GLuint createTextureAttachment(int width, int height, int format, int type);
    GLuint createDepthBufferAttachment(int width, int height);

    GLuint geometryFrameBuffer;
    GLuint positionTexture;
    GLuint normalTexture;
    GLuint albedoTexture;
    GLuint depthRenderBuffer;

    GLuint ssaoFrameBuffer;
    GLuint ssaoTexture;

    GLuint blurFrameBuffer;
    GLuint blurTexture;

    QWidget *wind;
};
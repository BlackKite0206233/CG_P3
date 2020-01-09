#pragma once

#include <QtGui/QOpenGLFunctions_4_3_Core>
#include <QtGui/QOpenGLVertexArrayObject>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLShader>
#include <QtGui/QOpenGLShaderProgram>
#include <QtWidgets/QWidget>
#include <QtWidgets/QMainWindow>

class WaterFrameBuffer: protected QOpenGLFunctions_4_3_Core {
private:
    static int REFLECTION_WIDTH;
    static int REFLECTION_HEIGHT;
    static int REFRACTION_WIDTH;
    static int REFRACTION_HEIGHT;
public:
    WaterFrameBuffer(QWidget *w);

    void CleanUp();
    void BindReflectionFrameBuffer();
    void BindRefractionFrameBuffer();
    void UnbindCurrentFrameBuffer();

    GLuint getReflectionTexture();
    GLuint getRefractionTexture();
    GLuint getRefractionDepthTexture();

private:
    void initReflectionFrameBuffer();
    void initRefractionFrameBuffer();
    void bindFrameBuffer(GLuint framebuffer, int width, int height);
    GLuint createFrameBuffer();
    GLuint createTextureAttachment(int width, int height);
    GLuint createDepthTextureAttachment(int width, int height); 
    GLuint createDepthBufferAttachment(int width, int height);

    GLuint reflectionFrameBuffer;
    GLuint reflectionTexture;
    GLuint reflectionDepthBuffer;

    GLuint refractionFrameBuffer;
    GLuint refractionTexture;
    GLuint refractionDepthTexture;

    QWidget *wind;
};
#include "WaterFrameBuffer.h"

int WaterFrameBuffer::REFLECTION_WIDTH = 1920;
int WaterFrameBuffer::REFLECTION_HEIGHT = 1080;
int WaterFrameBuffer::REFRACTION_WIDTH = 1920;
int WaterFrameBuffer::REFRACTION_HEIGHT = 1080;

WaterFrameBuffer::WaterFrameBuffer(QWidget *w) {
    initializeOpenGLFunctions();
    wind = w;
    initReflectionFrameBuffer();
    initRefractionFrameBuffer();
}

void WaterFrameBuffer::CleanUp() {
    glDeleteFramebuffers(1, &reflectionFrameBuffer);
    glDeleteTextures(1, &reflectionTexture);
    glDeleteRenderbuffers(1, &reflectionDepthBuffer);

    glDeleteFramebuffers(1, &refractionFrameBuffer);
    glDeleteTextures(1, &refractionTexture);
    glDeleteRenderbuffers(1, &refractionDepthTexture);
}

void WaterFrameBuffer::BindReflectionFrameBuffer() {
    bindFrameBuffer(reflectionFrameBuffer,REFLECTION_WIDTH,REFLECTION_HEIGHT);
}

void WaterFrameBuffer::BindRefractionFrameBuffer() {
    bindFrameBuffer(refractionFrameBuffer, REFRACTION_WIDTH, REFRACTION_HEIGHT);
}

void WaterFrameBuffer::UnbindCurrentFrameBuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, wind->width(), wind->height());
}

GLuint WaterFrameBuffer::getReflectionTexture() {
    return reflectionTexture;
}

GLuint WaterFrameBuffer::getRefractionTexture() {
    return refractionTexture;
}

GLuint WaterFrameBuffer::getRefractionDepthTexture() {
    return refractionDepthTexture;
}

void WaterFrameBuffer::initReflectionFrameBuffer() {
    reflectionFrameBuffer = createFrameBuffer();
    reflectionTexture = createTextureAttachment(REFLECTION_WIDTH, REFLECTION_HEIGHT);
    reflectionDepthBuffer = createDepthBufferAttachment(REFLECTION_WIDTH, REFLECTION_HEIGHT);
    UnbindCurrentFrameBuffer();
}

void WaterFrameBuffer::initRefractionFrameBuffer() {
    refractionFrameBuffer = createFrameBuffer();
    refractionTexture = createTextureAttachment(REFRACTION_WIDTH, REFRACTION_HEIGHT);
    refractionDepthTexture = createDepthTextureAttachment(REFRACTION_WIDTH, REFRACTION_HEIGHT);
}

void WaterFrameBuffer::bindFrameBuffer(GLuint framebuffer, int width, int height) {
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glViewport(0, 0, width, height);
}

GLuint WaterFrameBuffer::createFrameBuffer() {
    GLuint framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    return framebuffer;
}

GLuint WaterFrameBuffer::createTextureAttachment(int width, int height) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture, 0);
    return texture;
}

GLuint WaterFrameBuffer::createDepthTextureAttachment(int width, int height) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture, 0);
    return texture;
}

GLuint WaterFrameBuffer::createDepthBufferAttachment(int width, int height) {
    GLuint depthBuffer;
    glGenRenderbuffers(1, &depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
    return depthBuffer;
}
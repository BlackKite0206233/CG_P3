#include "SSAOFrameBuffer.h"

int SSAOFrameBuffer::WIDTH = 1920;
int SSAOFrameBuffer::HEIGHT = 1080;

SSAOFrameBuffer::SSAOFrameBuffer(QWidget *w) {
    initializeOpenGLFunctions();
    wind = w;

    initGeometryFrameBuffer();
    initSSAOFrameBuffer();
    initBlurFrameBuffer();
}

void SSAOFrameBuffer::CleanUp() {
    glDeleteFramebuffers(1, &geometryFrameBuffer);
    glDeleteTextures(1, &positionTexture);
    glDeleteTextures(1, &normalTexture);
    glDeleteTextures(1, &albedoTexture);
    glDeleteRenderbuffers(1, &depthRenderBuffer);

    glDeleteFramebuffers(1, &ssaoFrameBuffer);
    glDeleteTextures(1, &ssaoTexture);

    glDeleteFramebuffers(1, &blurFrameBuffer);
    glDeleteTextures(1, &blurTexture);
}

void SSAOFrameBuffer::BindGeometryFrameBuffer() {
    bindFrameBuffer(geometryFrameBuffer, WIDTH, HEIGHT);
}

void SSAOFrameBuffer::BindSSAOFrameBuffer() {
    bindFrameBuffer(ssaoFrameBuffer, WIDTH, HEIGHT);
}

void SSAOFrameBuffer::BindBlurFrameBuffer() {
    bindFrameBuffer(blurFrameBuffer, WIDTH, HEIGHT);
}

void SSAOFrameBuffer::UnbindCurrentFrameBuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, wind->width(), wind->height());
}

GLuint SSAOFrameBuffer::getPositionTexture() {
    return positionTexture;
}

GLuint SSAOFrameBuffer::getNormalTexture() {
    return normalTexture;
}

GLuint SSAOFrameBuffer::getAlbedoTexture() {
    return albedoTexture;
}

GLuint SSAOFrameBuffer::getSSAOTexture() {
    return ssaoTexture;
}

GLuint SSAOFrameBuffer::getBlurTexture() {
    return blurTexture;
}

void SSAOFrameBuffer::initGeometryFrameBuffer() {
    geometryFrameBuffer = createFrameBuffer();
    positionTexture = createTextureAttachment(WIDTH, HEIGHT, GL_RGB16F, GL_FLOAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, positionTexture, 0);

    normalTexture = createTextureAttachment(WIDTH, HEIGHT, GL_RGB16F, GL_FLOAT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normalTexture, 0);

    albedoTexture = createTextureAttachment(WIDTH, HEIGHT, GL_RGB, GL_UNSIGNED_BYTE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, albedoTexture, 0);
    unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);
    depthRenderBuffer = createDepthBufferAttachment(WIDTH, HEIGHT);

    UnbindCurrentFrameBuffer();
}

void SSAOFrameBuffer::initSSAOFrameBuffer() {
    ssaoFrameBuffer = createFrameBuffer();
    ssaoTexture = createTextureAttachment(WIDTH, HEIGHT, GL_RED, GL_FLOAT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoTexture, 0);

    UnbindCurrentFrameBuffer();
}

void SSAOFrameBuffer::initBlurFrameBuffer() {
    blurFrameBuffer = createFrameBuffer();
    blurTexture = createTextureAttachment(WIDTH, HEIGHT, GL_RED, GL_FLOAT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurTexture, 0);
    
    UnbindCurrentFrameBuffer();
}

void SSAOFrameBuffer::bindFrameBuffer(GLuint framebuffer, int width, int height) {
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glViewport(0, 0, width, height);
}

GLuint SSAOFrameBuffer::createFrameBuffer() {
    GLuint framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    return framebuffer;
}

GLuint SSAOFrameBuffer::createTextureAttachment(int width, int height, int format, int type) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_RGB, type, NULL);
    gglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    return texture;
}

GLuint SSAOFrameBuffer::createDepthBufferAttachment(int width, int height) {
    GLuint depthBuffer;
    glGenRenderbuffers(1, &depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
    return depthBuffer;
}
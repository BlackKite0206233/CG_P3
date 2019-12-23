#include "SkyBox.h"
#include <QImage>
#include <iostream>

void DimensionTransformation(GLfloat source[],GLfloat target[][4])
{
	//for uniform value, transfer 1 dimension to 2 dimension
	int i = 0;
	for(int j=0;j<4;j++)
		for(int k=0;k<4;k++)
		{
			target[j][k] = source[i];
			i++;
		}
}

SkyBox::SkyBox() {
}


void SkyBox::Init() {
    InitVAO();
    InitTexture();
    InitShader("./Shader/SkyBox.vs","./Shader/SkyBox.fs");
}

void SkyBox::Render(GLfloat* ProjectionMatrix, GLfloat* ModelViewMatrix) {
    GLfloat P[4][4];
	GLfloat MV[4][4];
	DimensionTransformation(ProjectionMatrix,P);
	DimensionTransformation(ModelViewMatrix,MV);

    glDisable(GL_DEPTH_MASK);
    
    shaderProgram->bind();
    vao.bind();
    
    //pass projection matrix to shader
	shaderProgram->setUniformValue("ProjectionMatrix",P);
	//pass modelview matrix to shader
	shaderProgram->setUniformValue("ModelViewMatrix",MV);

    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

    glDrawArrays(GL_TRIANGLES, 0, 36);

    glEnable(GL_DEPTH_MASK);
}

void SkyBox::InitVAO() {
    vao.create();
    vao.bind();
}

void SkyBox::InitTexture() {
    vector<std::string> faces({
        "RT.png",
        "LF.png",
        "UP.png",
        "DN.png",
        "FR.png",
        "BK.png"
    });

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++) {
        unsigned char *data = QImage("./Textures/ct_" + faces[i]).bits();
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
                         0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
        } else {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void SkyBox::InitShader(QString vertexShaderPath,QString fragmentShaderPath) {
    shaderProgram = new QOpenGLShaderProgram();
	QFileInfo  vertexShaderFile(vertexShaderPath);
	if(vertexShaderFile.exists())
	{
		vertexShader = new QOpenGLShader(QOpenGLShader::Vertex);
		if(vertexShader->compileSourceFile(vertexShaderPath))
			shaderProgram->addShader(vertexShader);
		else
			qWarning() << "Vertex Shader Error " << vertexShader->log();
	}
	else
		qDebug()<<vertexShaderFile.filePath()<<" can't be found";

	QFileInfo  fragmentShaderFile(fragmentShaderPath);
	if(fragmentShaderFile.exists())
	{
		fragmentShader = new QOpenGLShader(QOpenGLShader::Fragment);
		if(fragmentShader->compileSourceFile(fragmentShaderPath))
			shaderProgram->addShader(fragmentShader);
		else
			qWarning() << "fragment Shader Error " << fragmentShader->log();
	}
	else
		qDebug()<<fragmentShaderFile.filePath()<<" can't be found";
	shaderProgram->link();
}
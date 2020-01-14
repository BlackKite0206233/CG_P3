#include "Terrain.h"
#include "Utilities/3dUtils.h"
#include <ctime>
#include <iostream>
using namespace std;

Terrain::Terrain(int w, int h) {
	initializeOpenGLFunctions();
	width = w;
	height = h;
	vao = new QOpenGLVertexArrayObject();
}

void Terrain::Init() {
	GeneratorTerrain();
	shaderProgram = InitShader("./Shader/Terrain.vs", "./Shader/Terrain.fs");
	InitVAO();
	InitVBO();
}

void Terrain::GeneratorTerrain() {
	//int seed = time(NULL);
	//cout << seed << endl;
	HeightGenerator heightGenerator(1578846500);

	int VERTEX_COUNT = 256;
	int count = VERTEX_COUNT * VERTEX_COUNT;
	terrainHeight = QVector<QVector<float>>(VERTEX_COUNT, QVector<float>(VERTEX_COUNT));
	vertices = QVector<QVector3D>(count);
	normals = QVector<QVector3D>(count);
	textureCoords = QVector<QVector2D>(count);
	indices = QVector<int>(6 * (VERTEX_COUNT - 1) * (VERTEX_COUNT - 1));

	int vertexPointer = 0;
	for (int i = 0; i < VERTEX_COUNT; i++) {
		for (int j = 0; j < VERTEX_COUNT; j++) {
			terrainHeight[j][i] = getHeight(heightGenerator, j, i);
			vertices[vertexPointer] = QVector3D((float)(j - VERTEX_COUNT / 2) / ((float)VERTEX_COUNT / 2 - 1) * width, terrainHeight[j][i], (float)(i - VERTEX_COUNT / 2) / ((float)VERTEX_COUNT / 2 - 1) * height);
			textureCoords[vertexPointer] = QVector2D(j, i);
			vertexPointer++;
		}
	}
	vertexPointer = 0;
	for (int i = 0; i < VERTEX_COUNT; i++) {
		for (int j = 0; j < VERTEX_COUNT; j++) {
			normals[vertexPointer] = calculateNormal(j, i);
			vertexPointer++;
		}
	}

	int pointer = 0;
	for (int gz = 0; gz < VERTEX_COUNT - 1; gz++) {
		for (int gx = 0; gx < VERTEX_COUNT - 1; gx++) {
			int topLeft = (gz * VERTEX_COUNT) + gx;
			int topRight = topLeft + 1;
			int bottomLeft = ((gz + 1) * VERTEX_COUNT) + gx;
			int bottomRight = bottomLeft + 1;
			indices[pointer++] = topLeft;
			indices[pointer++] = bottomLeft;
			indices[pointer++] = topRight;
			indices[pointer++] = topRight;
			indices[pointer++] = bottomLeft;
			indices[pointer++] = bottomRight;
		}
	}
}

float barryCentric(QVector3D p1, QVector3D p2, QVector3D p3, QVector2D pos) {
	float det = (p2.z() - p3.z()) * (p1.x() - p3.x()) + (p3.x() - p2.x()) * (p1.z() - p3.z());
	float l1 = ((p2.z() - p3.z()) * (pos.x() - p3.x()) + (p3.x() - p2.x()) * (pos.y() - p3.z())) / det;
	float l2 = ((p3.z() - p1.z()) * (pos.x() - p3.x()) + (p1.x() - p3.x()) * (pos.y() - p3.z())) / det;
	float l3 = 1.0f - l1 - l2;
	return l1 * p1.y() + l2 * p2.y() + l3 * p3.y();
}

float Terrain::getHeightOfTerrain(float worldX, float worldY) {
	float terrainX = worldX + width;
	float terrainY = worldY + height;
	float gridWidth = (float)width * 2 / (terrainHeight.size() - 1);
	float gridHeight = (float)height * 2 / (terrainHeight.size() - 1);
	int gridX = floor(terrainX / gridWidth);
	int gridY = floor(terrainY / gridHeight);
	if (gridX >= terrainHeight.size() - 1 || gridY >= terrainHeight.size() - 1 || gridX < 0 || gridY < 0) {
		return 0;
	}
	float xCoord = fmod(terrainX, gridWidth) / gridWidth;
	float yCoord = fmod(terrainY, gridHeight) / gridHeight;
	float answer;
	if (xCoord <= 1 - yCoord) {
		answer = barryCentric(QVector3D(0, terrainHeight[gridX][gridY], 0), QVector3D(1, terrainHeight[gridX + 1][gridY], 0),
			QVector3D(0, terrainHeight[gridX][gridY + 1], 1), QVector2D(xCoord, yCoord));
	}
	else {
		answer = barryCentric(QVector3D(1, terrainHeight[gridX + 1][gridY], 0), QVector3D(1, terrainHeight[gridX + 1][gridY + 1], 1),
			QVector3D(0, terrainHeight[gridX][gridY + 1], 1), QVector2D(xCoord, yCoord));
	}

	return answer;
}

float Terrain::getHeight(HeightGenerator& generator, int x, int y) {
	return generator.GenerateHeight(x, y);
}

QVector3D Terrain::calculateNormal(int x, int y) {
	float heightL, heightR, heightD, heightU;
	if (x - 1 < 0 || x - 1 >= terrainHeight.size()) {
		heightL = 0;
	}
	else {
		heightL = terrainHeight[x - 1][y];
	}
	if (x + 1 < 0 || x + 1 >= terrainHeight.size()) {
		heightR = 0;
	}
	else {
		heightR = terrainHeight[x + 1][y];
	}
	if (y - 1 < 0 || y - 1 >= terrainHeight.size()) {
		heightD = 0;
	}
	else {
		heightD = terrainHeight[x][y - 1];
	}
	if (y + 1 < 0 || y + 1 >= terrainHeight.size()) {
		heightU = 0;
	}
	else {
		heightU = terrainHeight[x][y + 1];
	}
	QVector3D normal = QVector3D(heightL - heightR, 2, heightD - heightU);
	normal.normalize();
	return normal;
}

void Terrain::InitVAO() {
	vao->create();
	vao->bind();
}

void Terrain::InitVBO() {
	vvbo.create();
	vvbo.bind();
	vvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
	vvbo.allocate(vertices.constData(), vertices.size() * sizeof(QVector3D));

	nvbo.create();
	nvbo.bind();
	nvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
	nvbo.allocate(normals.constData(), normals.size() * sizeof(QVector3D));

	tvbo.create();
	tvbo.bind();
	tvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
	tvbo.allocate(textureCoords.constData(), textureCoords.size() * sizeof(QVector2D));
}

void Terrain::Render(GLfloat* ProjectionMatrix, GLfloat* ViewMatrix, Light& light, QVector3D& eyePos, QVector3D& fogColor, QVector<QOpenGLTexture*>& textures, SSAOFrameBuffer* ssaoFrameBuffer, int renderMode, QVector4D clipPlane) {
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

	//shaderProgram->setUniformValue("heightMap", 0);
	shaderProgram->setUniformValue("grass", 0);
	shaderProgram->setUniformValue("mud", 1);

	shaderProgram->setUniformValue("renderMode", renderMode);
	shaderProgram->setUniformValue("ssaoColorBufferBlur", 2);

	shaderProgram->setUniformValue("color_ambient", light.ambientColor);
	shaderProgram->setUniformValue("color_diffuse", light.diffuseColor);
	shaderProgram->setUniformValue("color_specular", light.specularColor);
	shaderProgram->setUniformValue("light_position", light.position);
	shaderProgram->setUniformValue("eye_position", eyePos);
	shaderProgram->setUniformValue("clipPlane", clipPlane);

	shaderProgram->setUniformValue("width", GLfloat(width));
	shaderProgram->setUniformValue("height", GLfloat(height));

	shaderProgram->setUniformValue("fogColor", fogColor);

	vvbo.bind();
	shaderProgram->enableAttributeArray(0);
	shaderProgram->setAttributeArray(0, GL_FLOAT, 0, 3, NULL);
	vvbo.release();

	nvbo.bind();
	shaderProgram->enableAttributeArray(1);
	shaderProgram->setAttributeArray(1, GL_FLOAT, 0, 3, NULL);
	nvbo.release();

	tvbo.bind();
	shaderProgram->enableAttributeArray(2);
	shaderProgram->setAttributeArray(2, GL_FLOAT, 0, 2, NULL);
	tvbo.release();

	//glActiveTexture(GL_TEXTURE0);
	//textures[3]->bind();
	glActiveTexture(GL_TEXTURE0);
	textures[4]->bind();
	glActiveTexture(GL_TEXTURE1);
	textures[5]->bind();
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, ssaoFrameBuffer->getBlurTexture());

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, indices.data());
	//Disable Attribute 0&1
	shaderProgram->disableAttributeArray(0);
	shaderProgram->disableAttributeArray(1);
	shaderProgram->disableAttributeArray(2);

	//unbind vao
	vao->release();
	//unbind vao
	shaderProgram->release();
}

void Terrain::DrawGeometry(QOpenGLShaderProgram* shader) {
	vao->bind();

	QMatrix4x4 modelMatrix;
	modelMatrix.setToIdentity();
	shader->setUniformValue("ModelMatrix", modelMatrix);
	shader->setUniformValue("scale", GLfloat(1));

	vvbo.bind();
	shader->enableAttributeArray(0);
	shader->setAttributeArray(0, GL_FLOAT, 0, 3, NULL);
	vvbo.release();

	nvbo.bind();
	shader->enableAttributeArray(1);
	shader->setAttributeArray(1, GL_FLOAT, 0, 3, NULL);
	nvbo.release();

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, indices.data());

	shader->disableAttributeArray(0);
	shader->disableAttributeArray(1);
	vao->release();
}
#include "Terrain.h"
#include "Utilities/3dUtils.h"

int Terrian::MAX_HEIGHT = 100;
int Terrian::MAX_PIXEL_COLOR = 256;

Terrian::Terrian(int w, int h) {
	initializeOpenGLFunctions();
	width = w;
	height = h;
	vao = new QOpenGLVertexArrayObject();
}


void Terrian::Init() {
	GeneratorTerrian();
	shaderProgram = InitShader("./Shader/Terrain.vs", "./Shader/Terrain.fs");
	InitVAO();
	InitVBO();
}

void Terrian::GeneratorTerrian() {
	QImage heightMap = QImage("./Textures/height_map.jpg");
	int VERTEX_COUNT = heightMap.height();
	int count = VERTEX_COUNT * VERTEX_COUNT;
	vertices = QVector<QVector3D>(count);
	normals = QVector<QVector3D>(count);
	textureCoords = QVector<QVector2D>(count);
	indices = QVector<int>(6 * (VERTEX_COUNT - 1) * (VERTEX_COUNT - 1));
	int vertexPointer = 0;

	for (int i = 0; i < VERTEX_COUNT; i++) {
		for (int j = 0; j < VERTEX_COUNT; j++) {
			vertices[vertexPointer] = QVector3D((float)(j - VERTEX_COUNT / 2) / ((float)VERTEX_COUNT / 2 - 1) * width, getHeight(heightMap, j, i), (float)(i - VERTEX_COUNT / 2) / ((float)VERTEX_COUNT / 2 - 1) * height);
			normals[vertexPointer] = QVector3D(0, 1, 0);
			textureCoords[vertexPointer] = QVector2D((float)j / (float)VERTEX_COUNT - 1, (float)i / (float)VERTEX_COUNT - 1);
			vertexPointer++;
		}
	}
	int pointer = 0;
	for (int gz = 0; gz < VERTEX_COUNT - 1; gz++) {
		for (int gx = 0; gx < VERTEX_COUNT - 1; gx++) {
			int topLeft = (gz * VERTEX_COUNT) + gx;
			int topRight = topLeft + 1;
			int buttomLeft = ((gz + 1) * VERTEX_COUNT) + gx;
			int buttonRight = buttomLeft + 1;
			indices[pointer++] = topLeft;
			indices[pointer++] = buttomLeft;
			indices[pointer++] = topRight;
			indices[pointer++] = topRight;
			indices[pointer++] = buttomLeft;
			indices[pointer++] = buttonRight;
		}
	}
}

float Terrian::getHeight(QImage& image, int x, int y) {
	if (x < 0 || x > image.width() || y < 0 || y > image.height()) {
		return 0;
	}
	float height = image.pixelColor(x, y).red();
	height -= MAX_PIXEL_COLOR / 2;
	height /= MAX_PIXEL_COLOR / 2;
	height *= MAX_HEIGHT;
	return height;
}

void Terrian::InitVAO() {
	vao->create();
	vao->bind();
}

void Terrian::InitVBO() {
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
//
//void Terrian::InitShader() {
//	QString vertexShaderPath         = "./Shader/Terrian.vs";
//	QString tessControlShaderPath    = "./Shader/Terrian.tcs";
//	QString tessEvaluationShaderPath = "./Shader/Terrian.tes";
//	QString fragmentShaderPath       = "./Shader/Terrian.fs";
//
//	shaderProgram = new QOpenGLShaderProgram();
//
//	QFileInfo vertexShaderFile(vertexShaderPath);
//	if (vertexShaderFile.exists()) {
//		QOpenGLShader* vertexShader = new QOpenGLShader(QOpenGLShader::Vertex);
//		if (vertexShader->compileSourceFile(vertexShaderPath))
//			shaderProgram->addShader(vertexShader);
//		else
//			qWarning() << "Vertex Shader Error " << vertexShader->log();
//	}
//	else
//		qDebug() << vertexShaderFile.filePath() << " can't be found";
//
//	QFileInfo tessControlShaderFile(tessControlShaderPath);
//	if (tessControlShaderFile.exists()) {
//		QOpenGLShader* tessControlShader = new QOpenGLShader(QOpenGLShader::TessellationControl);
//		if (tessControlShader->compileSourceFile(tessControlShaderPath))
//			shaderProgram->addShader(tessControlShader);
//		else
//			qWarning() << "Vertex Shader Error " << tessControlShader->log();
//	}
//	else
//		qDebug() << tessControlShaderFile.filePath() << " can't be found";
//
//	QFileInfo tessEvaluationShaderFile(vertexShaderPath);
//	if (tessEvaluationShaderFile.exists()) {
//		QOpenGLShader* tessEvaluationShader = new QOpenGLShader(QOpenGLShader::TessellationEvaluation);
//		if (tessEvaluationShader->compileSourceFile(tessEvaluationShaderPath))
//			shaderProgram->addShader(tessEvaluationShader);
//		else
//			qWarning() << "Vertex Shader Error " << tessEvaluationShader->log();
//	}
//	else
//		qDebug() << tessEvaluationShaderFile.filePath() << " can't be found";
//
//	QFileInfo fragmentShaderFile(fragmentShaderPath);
//	if (fragmentShaderFile.exists()) {
//		QOpenGLShader* fragmentShader = new QOpenGLShader(QOpenGLShader::Fragment);
//		if (fragmentShader->compileSourceFile(fragmentShaderPath))
//			shaderProgram->addShader(fragmentShader);
//		else
//			qWarning() << "fragment Shader Error " << fragmentShader->log();
//	}
//	else
//		qDebug() << fragmentShaderFile.filePath() << " can't be found";
//	shaderProgram->link();
//}

void Terrian::Render(GLfloat* ProjectionMatrix, GLfloat* ViewMatrix, Light& light, QVector3D& eyePos, QVector<QOpenGLTexture*>& textures, QVector4D clipPlane) {
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
	shaderProgram->setUniformValue("terrianTexture", 0);

	shaderProgram->setUniformValue("color_ambient", light.ambientColor);
	shaderProgram->setUniformValue("color_diffuse", light.diffuseColor);
	shaderProgram->setUniformValue("color_specular", light.specularColor);
	shaderProgram->setUniformValue("light_position", light.position);
	shaderProgram->setUniformValue("eye_position", eyePos);
	shaderProgram->setUniformValue("clipPlane", clipPlane);

	shaderProgram->setUniformValue("width", GLfloat(width));
	shaderProgram->setUniformValue("height", GLfloat(height));

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
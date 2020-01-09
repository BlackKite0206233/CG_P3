#include "model.h"

#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QVarLengthArray>
#include <QColor>
#include <QtOpenGL/QtOpenGL>
#include "Utilities/3DUtils.h"

Model::Model(const QString &filePath) : m_fileName(QFileInfo(filePath).fileName()) {
	QFile file(filePath);
	if (!file.open(QIODevice::ReadOnly))
		return;

	Point3d boundsMin(1e9, 1e9, 1e9);
	Point3d boundsMax(-1e9, -1e9, -1e9);

	QTextStream in(&file);
	while (!in.atEnd()) {
		QString input = in.readLine();
		if (input.isEmpty() || input[0] == '#')
			continue;

		QTextStream ts(&input);
		QString id;
		ts >> id;
		if (id == "v") {
			Point3d p;
			for (int i = 0; i < 3; ++i) {
				ts >> p[i];
				boundsMin[i] = qMin(boundsMin[i], p[i]);
				boundsMax[i] = qMax(boundsMax[i], p[i]);
			}
			m_points << p;
		}
		else if (id == "vn") {
			Point3d n;
			for (int i = 0; i < 3; ++i) {
				ts >> n[i];
			}
			m_normals << n;
		}
		else if (id == "f" || id == "fo") {
			QVarLengthArray<int, 4> p;
			QVarLengthArray<int, 4> n;

			while (!ts.atEnd()) {
				QString vertex;
				ts >> vertex;
				const int vertexIndex = vertex.split('/').value(0).toInt();
				if (vertexIndex)
					p.append(vertexIndex > 0 ? vertexIndex - 1 : m_points.size() + vertexIndex);

				const int normalIndex = vertex.split('/').value(2).toInt();
				if (normalIndex)
					n.append(normalIndex > 0 ? normalIndex - 1 : m_normals.size() + normalIndex);
			}

			for (int i = 0; i < p.size(); ++i) {
				const int edgeA = p[i];
				const int edgeB = p[(i + 1) % p.size()];

				if (edgeA < edgeB)
					m_edgeIndices << edgeA << edgeB;
			}

			for (int i = 0; i < 3; ++i)
				m_pointIndices << p[i];

			if (p.size() == 4)
				for (int i = 0; i < 3; ++i)
					m_pointIndices << p[(i + 2) % 4];

			for (int i = 0; i < 3; ++i)
				m_normalIndices << n[i];

			if (n.size() == 4)
				for (int i = 0; i < 3; ++i)
					m_normalIndices << n[(i + 2) % 4];
		}
	}

	bounds = boundsMax - boundsMin;
	
	for (int i = 0; i < m_points.size(); ++i)
		m_points[i] = (m_points[i] - (boundsMin + bounds * 0.5));

	Init();
}

void Model::render(QVector3D color, GLfloat *ProjectionMatrix, GLfloat *ViewMatrix, QMatrix4x4 ModelMatrix, Light& light, QVector3D& eyePos, QVector4D clipPlane, double s, bool wireframe, bool normals) {
	GLfloat P[4][4];
	GLfloat V[4][4];
	DimensionTransformation(ProjectionMatrix, P);
	DimensionTransformation(ViewMatrix, V);

	//Bind the shader we want to draw with
	shaderProgram->bind();
	//Bind the VAO we want to draw
	vao.bind();

	//pass projection matrix to shader
	shaderProgram->setUniformValue("ProjectionMatrix", P);
	//pass modelview matrix to shader
	shaderProgram->setUniformValue("ViewMatrix", V);
	shaderProgram->setUniformValue("ModelMatrix", ModelMatrix);

	shaderProgram->setUniformValue("color_ambient", light.ambientColor);
	shaderProgram->setUniformValue("color_diffuse", light.diffuseColor);
	shaderProgram->setUniformValue("color_specular", light.specularColor);
	shaderProgram->setUniformValue("light_position", light.position);
	shaderProgram->setUniformValue("eye_position", eyePos);
	shaderProgram->setUniformValue("clipPlane", clipPlane);

	shaderProgram->setUniformValue("Color", color);

	const GLfloat scale = s / qMax(bounds.x, qMax(bounds.y, bounds.z));
	shaderProgram->setUniformValue("Scale", scale);

	vvbo.bind();
	shaderProgram->enableAttributeArray(0);
	shaderProgram->setAttributeArray(0, GL_FLOAT, 0, 3, NULL);
	vvbo.release();

	nvbo.bind();
	shaderProgram->enableAttributeArray(1);
	shaderProgram->setAttributeArray(1, GL_FLOAT, 0, 3, NULL);
	nvbo.release();

	//Draw a triangle with 3 indices starting from the 0th index
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());
	//Disable Attribute 0&1
	shaderProgram->disableAttributeArray(0);
	shaderProgram->disableAttributeArray(1);

	//unbind vao
	vao.release();
	//unbind vao
	shaderProgram->release();
}

void Model::Init() {
	shaderProgram = InitShader("./Shader/Model.vs", "./Shader/Model.fs");
	InitVAO();
	InitVBO();
}

void Model::InitVAO() {
	vao.create();
	vao.bind();
}

void Model::InitVBO() {
	for (int i = 0; i < m_pointIndices.size(); i++) {
		Point3d p = m_points.at(m_pointIndices[i]);
		vertices << QVector3D(p.x, p.y, p.z);
	}
	vvbo.create();
	vvbo.bind();
	vvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
	vvbo.allocate(vertices.constData(), vertices.size() * sizeof(QVector3D));

	for (int i = 0; i < m_normalIndices.size(); i++) {
		Point3d n = m_normals.at(m_normalIndices[i]);
		normals << QVector3D(n.x, n.y, n.z);
	}
	nvbo.create();
	nvbo.bind();
	nvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
	nvbo.allocate(normals.constData(), normals.size() * sizeof(QVector3D));
}
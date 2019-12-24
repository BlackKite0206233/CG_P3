#ifndef MODEL_H
#define MODEL_H
#include <QtGui/QOpenGLFunctions_4_3_Core>
#include <QtGui/QOpenGLVertexArrayObject>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLShader>
#include <QtGui/QOpenGLShaderProgram>
#include <QtCore/QString>
#include <QtCore/QVector>
#include <QVector3D>
#include <QMatrix4x4>
#include <QString>
#include <math.h>

#include "Point3d.h"
#include "Utilities/Pnt3f.h"

class Model {
public:
	Model() {}
	Model(const QString &filePath, int s);

	void render(QVector3D color, GLfloat *ProjectionMatrix, GLfloat *ViewMatrix, QMatrix4x4 ModelMatrix, bool wireframe = false, bool normals = false);

	QString fileName() const { return m_fileName; }
	int faces() const { return m_pointIndices.size() / 3; }
	int edges() const { return m_edgeIndices.size() / 2; }
	int points() const { return m_points.size(); }

	void Init();
	void InitVAO();
	void InitVBO();
	void InitShader(QString vertexShaderPath, QString fragmentShaderPath);

private:
	QString m_fileName;
	QVector<Point3d> m_points;
	QVector<Point3d> m_normals;
	QVector<int> m_edgeIndices;
	QVector<int> m_pointIndices;

	QOpenGLShaderProgram *shaderProgram;
	QOpenGLShader *vertexShader;
	QOpenGLShader *fragmentShader;
	QVector<QVector3D> vertices;
	QVector<QVector3D> normals;
	QOpenGLVertexArrayObject vao;
	QOpenGLBuffer vvbo;
	QOpenGLBuffer nvbo;
};

#endif
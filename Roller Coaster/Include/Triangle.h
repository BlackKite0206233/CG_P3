#include <QtGui/QOpenGLFunctions_4_3_Core>
#include <QtGui/QOpenGLVertexArrayObject>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLShader>
#include <QtGui/QOpenGLShaderProgram>
#include <QVector>
#include <QVector3D>
#include <QFileInfo>
#include <QDebug>
#include <QString>

class Triangle {
public:
	QOpenGLShaderProgram *shaderProgram;
	QVector<QVector3D> vertices;
	QVector<QVector3D> colors;
	QOpenGLVertexArrayObject vao;
	QOpenGLBuffer vvbo;
	QOpenGLBuffer cvbo;

public:
	Triangle();
	void Init();
	void InitVAO();
	void InitVBO();
	void Paint(GLfloat *ProjectionMatrix, GLfloat *ModelViewMatrix);
};
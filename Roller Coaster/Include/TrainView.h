#ifndef TRAINVIEW_H
#define TRAINVIEW_H
#include <QGLWidget>
#include <vector>
#include <QtGui>
#include <QtOpenGL>
#include <GL/GLU.h>
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#include "Utilities/ArcBallCam.h"
#include "Utilities/3DUtils.h"
#include "Track.H"
#include "Triangle.h"
#include "Square.h"
#include "Train.h"
#include "SkyBox.h"
#include "Light.h"
#include "Water.h"
#include "WaterFrameBuffer.h"
#include "Terrain.h"
#include "SSAO.h"
#include "SSAOFrameBuffer.h"

using namespace std;

class AppMain;
class CTrack;

//#######################################################################
// TODO
// You might change the TrainView in order to add different objects to
// be drawn, or adjust the lighting, or ...
//#######################################################################

enum RenderMode {
	Normal_Mode,
	SSAO_Mode,
	NPR_Mode
};

enum CameraType {
	World,
	Top,
	Train
};

class TrainView : public QGLWidget, protected QOpenGLFunctions_4_3_Core {
	Q_OBJECT
public:
	explicit TrainView(QWidget *parent = 0);
	~TrainView();

public:
	// overrides of important window things
	//virtual int handle(int);
	virtual void paintGL();

	// all of the actual drawing happens in this routine
	// it has to be encapsulated, since we draw differently if
	// we're drawing shadows (no colors, for example)
	void drawStuff(QVector4D& clipplane = QVector4D(0, 0, 0, 0), bool doingShadows = false);
	void drawGeometry();
	void drawSkyBox();

	// setup the projection - assuming that the projection stack has been
	// cleared for you
	void setProjection();

	void SetCamera(CameraType);

	// Reset the Arc ball control
	void resetArcball();

	// pick a point (for when the mouse goes down)
	void doPick(int mx, int my);

	void initializeGL();
	void initializeTexture();

	void AddTrain();
	void RemoveTrain(int index);
	void MoveTrain();

	QVector3D getCameraPosition();

private:
	void InitVAO();
	void InitVBO();
	void Render(QOpenGLShaderProgram* currentShader, float intensity, GLuint texture);

	GLuint createFrameBuffer();
	GLuint createTexture();
	GLuint createDepthTexture();

public:
	ArcBallCam *arcball; // keep an ArcBall for the UI
	vector<ArcBallCam> cameras;
	int selectedPoint; // simple - just remember which cube is selected
	int lastSelectedPoint;
	int selectedPath;
	int selectedTrain;

	CTrack *m_pTrack; // The track of the entire scene

	CameraType camera;
	RenderMode renderMode;

	Triangle *triangle;
	Square *square;
	SkyBox *skybox;
	GLfloat ProjectionMatrix[16];
	GLfloat ModelViewMatrix[16];
	GLfloat lastModelViewMatrix[16];
	QVector<QOpenGLTexture *> Textures;
	vector<CTrain> trains;
	int currentTrain = 0;
	Light light;
	Water *water;
	WaterFrameBuffer *fbos;
	Terrain *terrain;
	SSAO *ssao;
	SSAOFrameBuffer *ssaoFrameBuffer;
	QVector3D fogColor;

	GLuint motionBlurFrameBuffer;
	GLuint motionBlurTexture;
	GLuint bloomFrameBuffer;
	GLuint bloomTexture;
	GLuint depthTexture;

	QOpenGLShaderProgram* motionBlurShader;
	QOpenGLShaderProgram* bloomShader;
	QVector<QVector3D> vertices;
	QOpenGLVertexArrayObject* vao;
	QOpenGLBuffer vbo;

	bool drawTrain;
	float blurIntensity;
	float bloomIntensity;
};
#endif // TRAINVIEW_H
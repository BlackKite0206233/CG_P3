#ifndef TRAINVIEW_H  
#define TRAINVIEW_H  
#include <QGLWidget> 
#include <vector>
#include <QtGui>  
#include <QtOpenGL>  
#include <GL/GLU.h>
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib") 
#include "Utilities/ArcBallCam.h"
#include "Utilities/3DUtils.h"
#include "Track.H"
#include "Triangle.h"
#include "Square.h"
#include "Train.h"
#include "Model.h"
#include "SkyBox.h"

using namespace std;

class AppMain;
class CTrack;

//#######################################################################
// TODO
// You might change the TrainView in order to add different objects to
// be drawn, or adjust the lighting, or ...
//#######################################################################

enum CameraType {
	World,
	Top,
	Train
};


class TrainView : public QGLWidget, protected QOpenGLFunctions_4_3_Core
{  
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
	void drawStuff(bool doingShadows=false);

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

public:
	ArcBallCam*		arcball;			// keep an ArcBall for the UI
	vector<ArcBallCam> cameras;
	int				selectedPoint;  // simple - just remember which cube is selected
	int lastSelectedPoint;
	int selectedPath;
	int selectedTrain;

	CTrack*			m_pTrack;		// The track of the entire scene

	CameraType camera;
	Triangle* triangle;
	Square* square;
	SkyBox* skybox;
	GLfloat ProjectionMatrex[16];
	GLfloat ModelViewMatrex[16];
	QVector<QOpenGLTexture*> Textures;
	vector<CTrain> trains;
	int currentTrain = 0;
};  
#endif // TRAINVIEW_H  
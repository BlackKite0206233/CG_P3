#include "TrainView.h"  
#include <ctime>
#include <cstdlib>

TrainView::TrainView(QWidget *parent) :  
QGLWidget(parent)  
{  
	QWidget::setFocusPolicy(Qt::StrongFocus);
	cameras = vector<ArcBallCam>(3);
	cameras[0].setup(this, 40, 250, .2f, .4f, 0);
	cameras[0].type = ArcBallCam::Perspective;
	cameras[1].setup(this, 40, 250, 1, 0, 0);
	cameras[1].type = ArcBallCam::Orthogonal;
	cameras[2].setup(this, 40, 250, .2f, .4f, 0);
	cameras[2].type = ArcBallCam::Perspective;
	SetCamera(World);
	resetArcball();

	srand(time(NULL));
	CTrain::speed = 3;
}  
TrainView::~TrainView()  
{}  
void TrainView::initializeGL()
{
	initializeOpenGLFunctions();
	//Create a triangle object
	triangle = new Triangle();
	//Initialize the triangle object
	triangle->Init();
	//Create a square object
	square = new Square();
	//Initialize the square object
	square->Init();
	//Initialize texture 
	initializeTexture();
	
}
void TrainView::initializeTexture()
{
	//Load and create a texture for square;'stexture
	QOpenGLTexture* texture = new QOpenGLTexture(QImage("./Textures/Tupi.bmp"));
	Textures.push_back(texture);
}
void TrainView:: resetArcball()
	//========================================================================
{
	// Set up the camera to look at the world
	// these parameters might seem magical, and they kindof are
	// a little trial and error goes a long way
	cameras[0].setup(this, 40, 250, .2f, .4f, 0);
	cameras[1].setup(this, 40, 250, 0.5, 0, 0);
}

void TrainView::paintGL()
{

	//*********************************************************************
	//
	// * Set up basic opengl informaiton
	//
	//**********************************************************************
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	// Set up the view port
	glViewport(0,0,width(),height());
	// clear the window, be sure to clear the Z-Buffer too
	glClearColor(0,0,0.3f,0);
	
	// we need to clear out the stencil buffer since we'll use
	// it for shadows
	glClearStencil(0);
	glEnable(GL_DEPTH);

	// Blayne prefers GL_DIFFUSE
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	// prepare for projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	setProjection();		// put the code to set up matrices here

	//######################################################################
	// TODO: 
	// you might want to set the lighting up differently. if you do, 
	// we need to set up the lights AFTER setting up the projection
	//######################################################################
	// enable the lighting
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	// top view only needs one light
	if (this->camera == Top) {
		glDisable(GL_LIGHT1);
		glDisable(GL_LIGHT2);
	} else {
		glEnable(GL_LIGHT1);
		glEnable(GL_LIGHT2);
	}

	//*********************************************************************
	//
	// * set the light parameters
	//
	//**********************************************************************
	GLfloat lightPosition1[]	= {0,1,1,0}; // {50, 200.0, 50, 1.0};
	GLfloat lightPosition2[]	= {1, 0, 0, 0};
	GLfloat lightPosition3[]	= {0, -1, 0, 0};
	GLfloat yellowLight[]		= {0.5f, 0.5f, .1f, 1.0};
	GLfloat whiteLight[]		= {1.0f, 1.0f, 1.0f, 1.0};
	GLfloat blueLight[]			= {.1f,.1f,.3f,1.0};
	GLfloat grayLight[]			= {.3f, .3f, .3f, 1.0};

	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition1);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteLight);
	glLightfv(GL_LIGHT0, GL_AMBIENT, grayLight);

	glLightfv(GL_LIGHT1, GL_POSITION, lightPosition2);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, yellowLight);

	glLightfv(GL_LIGHT2, GL_POSITION, lightPosition3);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, blueLight);



	//*********************************************************************
	// now draw the ground plane
	//*********************************************************************
	setupFloor();
	glDisable(GL_LIGHTING);
	drawFloor(200,10);


	//*********************************************************************
	// now draw the object and we need to do it twice
	// once for real, and then once for shadows
	//*********************************************************************
	glEnable(GL_LIGHTING);
	setupObjects();

	drawStuff();

	// this time drawing is for shadows (except for top view)
	if (this->camera != Top) {
		setupShadows();
		drawStuff(true);
		unsetupShadows();
	}

	//Get modelview matrix
 	glGetFloatv(GL_MODELVIEW_MATRIX,ModelViewMatrex);
	//Get projection matrix
 	glGetFloatv(GL_PROJECTION_MATRIX,ProjectionMatrex);

	/*
	//Call triangle's render function, pass ModelViewMatrex and ProjectionMatrex
 	triangle->Paint(ProjectionMatrex,ModelViewMatrex);
    
	//we manage textures by Trainview class, so we modify square's render function
	square->Begin();
		//Active Texture
		glActiveTexture(GL_TEXTURE0);
		//Bind square's texture
		Textures[0]->bind();
		//pass texture to shader
		square->shaderProgram->setUniformValue("Texture", 0);
		//Call square's render function, pass ModelViewMatrex and ProjectionMatrex
		square->Paint(ProjectionMatrex,ModelViewMatrex);
	square->End();
	*/
}

//************************************************************************
//
// * This sets up both the Projection and the ModelView matrices
//   HOWEVER: it doesn't clear the projection first (the caller handles
//   that) - its important for picking
//========================================================================
void TrainView::
setProjection()
//========================================================================
{
	arcball->setProjection(false);
	update();
}

static unsigned long lastRedraw = 0;
//************************************************************************
//
// * this draws all of the stuff in the world
//
//	NOTE: if you're drawing shadows, DO NOT set colors (otherwise, you get 
//       colored shadows). this gets called twice per draw 
//       -- once for the objects, once for the shadows
//########################################################################
// TODO: 
// if you have other objects in the world, make sure to draw them
//########################################################################
//========================================================================
void TrainView::drawStuff(bool doingShadows)
{
	// Draw the control points
	// don't draw the control points if you're driving 
	// (otherwise you get sea-sick as you drive through them)
	if (this->camera != Train) {
		for(size_t i = 0; i < this->m_pTrack->points.size(); ++i) {
			if (!doingShadows) {
				if ( ((int) i) != selectedPoint)
					glColor3ub(240, 60, 60);
				else
					glColor3ub(240, 240, 30);
			}
			this->m_pTrack->points[i].draw();
		}
		update();
	}
	// draw the track
	//####################################################################
	// TODO: 
	// call your own track drawing code
	//####################################################################

#ifdef EXAMPLE_SOLUTION
	drawTrack(this, doingShadows);
#endif
	this->m_pTrack->Draw(doingShadows, selectedPath);

	if (CTrain::isMove) {
		if (clock() - lastRedraw > CLOCKS_PER_SEC / 30) {
			lastRedraw = clock();
			MoveTrain();
		}
	}

	for (int i = 0; i < trains.size(); i++) {
		trains[i].Draw(doingShadows, i == selectedTrain);
	}
	// draw the train
	//####################################################################
	// TODO: 
	//	call your own train drawing code
	//####################################################################
#ifdef EXAMPLE_SOLUTION
	// don't draw the train if you're looking out the front window
	if (!tw->trainCam->value())
		drawTrain(this, doingShadows);
#endif
}

void TrainView::
	doPick(int mx, int my)
	//========================================================================
{
	// since we'll need to do some GL stuff so we make this window as 
	// active window
	makeCurrent();

	// get the viewport - most reliable way to turn mouse coords into GL coords
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPickMatrix((double)mx, (double)(viewport[3]-my), 5, 5, viewport);

	// now set up the projection
	setProjection();

	// now draw the objects - but really only see what we hit
	GLuint buf[100];
	glSelectBuffer(100,buf);
	glRenderMode(GL_SELECT);
	glInitNames();
	glPushName(0);


	// draw the cubes, loading the names as we go
	for(size_t i=0; i<m_pTrack->points.size(); ++i) {
		glLoadName((GLuint) (i+1));
		m_pTrack->points[i].draw();
	}

	auto it = m_pTrack->paths.begin();
	for (int i = 0; it != m_pTrack->paths.end(); i++, it++) {
		glLoadName((GLuint)(i + 1 + m_pTrack->points.size()));
		for (auto& pathData : it->second) {
			pathData.second.Draw(false, false);
		}
	}

	for (int i = 0; i < trains.size(); i++) {
		glLoadName((GLuint)(i + 1 + m_pTrack->points.size() + m_pTrack->paths.size()));
		trains[i].Draw(false, false);
	}

	// go back to drawing mode, and see how picking did
	int hits = glRenderMode(GL_RENDER);
	
	selectedPoint = selectedPath = selectedTrain = -1;
	if (hits) {
		// warning; this just grabs the first object hit - if there
		// are multiple objects, you really want to pick the closest
		// one - see the OpenGL manual 
		// remember: we load names that are one more than the index
		int tmp = buf[3] - 1;
		selectedPoint = selectedPath = selectedTrain = -1;
		if (tmp < m_pTrack->points.size()) {
			selectedPoint = tmp;
		}
		else if (tmp < m_pTrack->points.size() + m_pTrack->paths.size()) {
			selectedPath = tmp - m_pTrack->points.size();
			lastSelectedPoint = -1;
		}
		else {
			selectedTrain = tmp - m_pTrack->points.size() - m_pTrack->paths.size();
			lastSelectedPoint = -1;
		}
	}
	else {
		lastSelectedPoint = -1;
	}
}

void TrainView::SetCamera(CameraType type) {
	camera = type;
	arcball = &cameras[camera];
}

void TrainView::AddTrain() {
	if (!m_pTrack->paths.size())
		return;
	
	PathData pd;
	if (selectedPoint >= 0) {
		auto children = m_pTrack->points[selectedPoint].children;
		if (children.size()) {
			auto it1 = children.begin();
			advance(it1, rand() % children.size());
			Path p = m_pTrack->paths[pair<int, int>(selectedPoint, *it1)];
			auto it2 = p.begin();
			advance(it2, rand() % p.size());
			pd = it2->second;
		}
		else {
			pd = m_pTrack->GetRandomPath();
		}
	}
	else if (selectedPath >= 0) {
		auto it1 = m_pTrack->paths.begin();
		advance(it1, selectedPath);
		auto it2 = it1->second.begin();
		advance(it2, rand() % it1->second.size());
		pd = it2->second;
	}
	else {
		pd = m_pTrack->GetRandomPath();
	}
	CTrain train(pd);
	trains.push_back(train);
}

PathData TrainView::getNewPath(const PathData& curr) {
	return m_pTrack->points[curr.p2].children.size() ? m_pTrack->GetNextPath(curr) : m_pTrack->GetRandomPath();
}

void TrainView::MoveTrain() {
	for (auto& train : trains) {
		if (train.t >= 1) {
			train.t -= 1;
			train.currentPath = getNewPath(train.currentPath);
		}
		train.Move();
	}
}

void TrainView::RemoveTrain(int index) {
	trains.erase(trains.begin() + index);
}
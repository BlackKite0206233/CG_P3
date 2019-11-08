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
	CTrain::speed = 10;
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
	// Compute the aspect ratio (we'll need it)
	float aspect = static_cast<float>(width()) / static_cast<float>(height());

	// Check whether we use the world camp
	if (this->camera == World){
		arcball->setProjection(false);
		update();
	// Or we use the top cam
	}else if (this->camera == Top) {
		arcball->setProjection(false);
		/*
		float wi, he;
		if (aspect >= 1) {
			wi = 110;
			he = wi / aspect;
		} 
		else {
			he = 110;
			wi = he * aspect;
		}

		// Set up the top camera drop mode to be orthogonal and set
		// up proper projection matrix
		glMatrixMode(GL_PROJECTION);
		glOrtho(-wi, wi, -he, he, 200, -200);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glRotatef(-90,1,0,0);
		*/
		update();
	} 
	// Or do the train view or other view here
	//####################################################################
	// TODO: 
	// put code for train view projection here!	
	//####################################################################
	else {
		arcball->setProjection(false);
#ifdef EXAMPLE_SOLUTION
		trainCamView(this,aspect);
#endif
		update();
	}
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
				if ( ((int) i) != selectedCube)
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
	this->m_pTrack->Draw(doingShadows);

	if (CTrain::isMove) {
		if (clock() - lastRedraw > CLOCKS_PER_SEC / 30) {
			lastRedraw = clock();
			MoveTrain();
		}
	}

	for (auto& train : trains) {
		train.Draw(doingShadows);
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
	glLoadIdentity ();

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

	// go back to drawing mode, and see how picking did
	int hits = glRenderMode(GL_RENDER);
	if (hits) {
		// warning; this just grabs the first object hit - if there
		// are multiple objects, you really want to pick the closest
		// one - see the OpenGL manual 
		// remember: we load names that are one more than the index
		selectedCube = buf[3]-1;
	} else // nothing hit, nothing selected
		selectedCube = -1;
}

void TrainView::SetCamera(CameraType type) {
	camera = type;
	arcball = &cameras[camera];
}

void TrainView::AddTrain() {
	auto it1 = m_pTrack->paths.begin();
	advance(it1, rand() % m_pTrack->paths.size());
	Path p = it1->second;
	auto it2 = p.begin();
	advance(it2, rand() % p.size());
	PathData pd = it2->second;

	Pnt3f v = pd.pointSet[0].pos - pd.pointSet[1].pos;
	v.normalize();
	CTrain train(pd.pointSet[0].pos, pd.pointSet[0].orient, v);
	train.currentPath = pd;
	trains.push_back(train);
}

PathData TrainView::getNewPath(PathData curr) {
	set<int> children;
	children = m_pTrack->points[curr.p2].children;
	Path nextPath;
	if (children.size()) {
		children = m_pTrack->points[curr.p3].children;
		nextPath = m_pTrack->paths[pair<int, int>(curr.p2, curr.p3)];
		set<int>::iterator it = children.begin();
		advance(it, rand() % children.size());
		int p3 = *it;
		return nextPath[pair<int, int>(curr.p1, p3)];
	}
	else {
		auto it1 = m_pTrack->paths.begin();
		advance(it1, rand() % m_pTrack->paths.size());
		nextPath = it1->second;
		auto it2 = nextPath.begin();
		advance(it2, rand() % nextPath.size());
		return it2->second;
	}
}

void TrainView::MoveTrain() {
	for (auto& train : trains) {
		train.t += 1.0 / train.currentPath.length * CTrain::speed;
		if (train.t >= 1) {
			train.t = 0;
			train.currentPath = getNewPath(train.currentPath);
			Pnt3f v = train.currentPath.pointSet[1].pos - train.currentPath.pointSet[0].pos;
			v.normalize();
			train.Move(train.currentPath.pointSet[0].pos, train.currentPath.pointSet[0].orient, v);
		}
		else {
			double t = train.t;
			PathData curr = train.currentPath;
			ControlPoint qt, qt_1;
			if (m_pTrack->curve == Linear) {
				qt.pos    = (1 - t) * curr.a.pos    + t * curr.b.pos;
				qt.orient = (1 - t) * curr.a.orient + t * curr.b.orient;

				t += 1.0 / train.currentPath.length * CTrain::speed;
				qt_1.pos = (1 - t) * curr.a.pos + t * curr.b.pos;
			}
			else {
				qt.pos    = pow(t, 3) * curr.a.pos    + pow(t, 2) * curr.b.pos    + t * curr.c.pos    + curr.d.pos;
				qt.orient = pow(t, 3) * curr.a.orient + pow(t, 2) * curr.b.orient + t * curr.c.orient + curr.d.orient;

				t += 1.0 / train.currentPath.length * CTrain::speed;
				qt_1.pos = pow(t, 3) * curr.a.pos + pow(t, 2) * curr.b.pos + t * curr.c.pos + curr.d.pos;
			}
			Pnt3f v = qt_1.pos - qt.pos;
			v.normalize();
			train.Move(qt.pos, qt.orient, v);
		}
	}
}
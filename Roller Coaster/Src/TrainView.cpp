#include "TrainView.h"
#include <ctime>
#include <cstdlib>

TrainView::TrainView(QWidget *parent) : QGLWidget(parent) {
	QWidget::setFocusPolicy(Qt::StrongFocus);
	cameras = vector<ArcBallCam>(3);
	cameras[0].type = ArcBallCam::World;
	cameras[1].type = ArcBallCam::Top;
	cameras[2].type = ArcBallCam::Train;
	SetCamera(World);
	resetArcball();

	srand(time(NULL));

	light.position = QVector4D(100000, 0, 0, 1);
	light.ambientColor = light.diffuseColor = light.specularColor = QVector3D(1, 1, 1);
	light.rotationMatrix.setToIdentity();
	light.rotationMatrix.rotate(0.1, 0, 0, 1);
}

TrainView::~TrainView() {
}

void TrainView::initializeGL() {
	initializeOpenGLFunctions();
	initializeTexture();
	//Create a triangle object
	triangle = new Triangle();
	//Initialize the triangle object
	triangle->Init();
	//Create a square object
	square = new Square();
	//Initialize the square object
	square->Init();

	skybox = new SkyBox();
	skybox->Init();

	water = new Water(1024, 1024);
	water->Init();

	terrain = new Terrain(1024, 1024);
	terrain->Init();

	PathData::terrain = terrain;

	fbos = new WaterFrameBuffer(this);
	//Initialize texture

}

void TrainView::initializeTexture()
{
	//Load and create a texture for square;'stexture
	QOpenGLTexture* texture = new QOpenGLTexture(QImage("./Textures/Tupi.bmp"));
	Textures.push_back(texture);
	texture = new QOpenGLTexture(QImage("./Textures/dudv_map.png"));
	Textures.push_back(texture);
	texture = new QOpenGLTexture(QImage("./Textures/normal_map.png"));
	Textures.push_back(texture);
	texture = new QOpenGLTexture(QImage("./Textures/height_map.jpg"));
	Textures.push_back(texture);
	texture = new QOpenGLTexture(QImage("./Textures/grassy2.png"));
	Textures.push_back(texture);
	texture = new QOpenGLTexture(QImage("./Textures/mud.png"));
	Textures.push_back(texture);
}

void TrainView::resetArcball()
//========================================================================
{
	// Set up the camera to look at the world
	// these parameters might seem magical, and they kindof are
	// a little trial and error goes a long way
	cameras[0].setup(this, 40, 0, 0, 250, M_PI / 4, 0, 0);
	cameras[1].setup(this, 40, 0, 0, 250);
	cameras[2].setup(this, 40, 0, 0, 250);
}

static unsigned long lastRedraw = 0;

void TrainView::paintGL()
{

	//*********************************************************************
	//
	// * Set up basic opengl informaiton
	//
	//**********************************************************************
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Set up the view port
	glViewport(0, 0, width(), height());
	// clear the window, be sure to clear the Z-Buffer too
	glClearColor(0, 0, 0, 0);

	// we need to clear out the stencil buffer since we'll use
	// it for shadows
	glClearStencil(0);
	//glEnable(GL_DEPTH);

	// Blayne prefers GL_DIFFUSE
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	// prepare for projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	setProjection(); // put the code to set up matrices here

	//*********************************************************************
	// now draw the ground plane
	//*********************************************************************
	// setupFloor();
	// drawFloor(200, 10);

	glGetFloatv(GL_PROJECTION_MATRIX, ProjectionMatrex);

	fbos->BindReflectionFrameBuffer();
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrex);
	QMatrix4x4 m(ModelViewMatrex);
	m = QMatrix4x4({
		1, 0, 0, 0,
		0, -1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
		}) * m;
	m.copyDataTo(ModelViewMatrex);
	glLoadMatrixf(ModelViewMatrex);

	drawSkyBox();
	setupObjects();
	GLdouble reflectionClipPlane[] = { 0, 1, 0, 0 };
	glClipPlane(GL_CLIP_PLANE0, reflectionClipPlane);
	glEnable(GL_CLIP_PLANE0);
	glEnable(GL_CLIP_DISTANCE0);
	drawStuff(QVector4D(reflectionClipPlane[0], reflectionClipPlane[1], reflectionClipPlane[2], reflectionClipPlane[3]));
	glDisable(GL_CLIP_PLANE0);
	glDisable(GL_CLIP_DISTANCE0);
	fbos->UnbindCurrentFrameBuffer();
	
	fbos->BindRefractionFrameBuffer();
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	setProjection();
	glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrex);
	//QMatrix4x4 rotationMatrix = QMatrix4x4({ 
	//	ModelViewMatrex[0], ModelViewMatrex[1], ModelViewMatrex[2], 0,
	//	ModelViewMatrex[4], ModelViewMatrex[5], ModelViewMatrex[6], 0,
	//	ModelViewMatrex[8], ModelViewMatrex[9], ModelViewMatrex[10], 0,
	//	0, 0, 0, 1
	//	});
	//rotationMatrix = rotationMatrix.transposed();
	//QVector4D ray(0, 0, 1, 0);
	//ray = rotationMatrix.inverted() * ray;
	//ray.normalize();
	//double angle1 = acos(QVector3D::dotProduct(QVector3D(ray), QVector3D(0, -1, 0)));
	//double angle2 = asin(sin(angle1) / 1.3);
	//rotationMatrix.rotate(angle1 - angle2, QVector3D::crossProduct(QVector3D(ray), QVector3D(0, -1, 0)));

	//QVector3D cameraPos = getCameraPosition();
	//double dx = tan(angle1) * cameraPos[1];
	//double dy = dx / tan(angle2);
	//m.setToIdentity();
	//m.translate(ModelViewMatrex[12], ModelViewMatrex[13], ModelViewMatrex[14]);
	//m = m * rotationMatrix;
	//m = m.transposed();
	//m.copyDataTo(ModelViewMatrex);
	//glLoadMatrixf(ModelViewMatrex);
	
	drawSkyBox();
	setupObjects();
	GLdouble refractionClipPlane[] = { 0, -1, 0, 0 };
	glClipPlane(GL_CLIP_PLANE0, refractionClipPlane);
	glEnable(GL_CLIP_PLANE0);
	glEnable(GL_CLIP_DISTANCE0);
	drawStuff(QVector4D(refractionClipPlane[0], refractionClipPlane[1], refractionClipPlane[2], refractionClipPlane[3]));
	glDisable(GL_CLIP_PLANE0);
	glDisable(GL_CLIP_DISTANCE0);
	fbos->UnbindCurrentFrameBuffer();

	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	setProjection();
	glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrex);

	drawSkyBox();
	setupObjects();
	drawStuff();
	this->water->Render((clock() - lastRedraw) / 65.0, ProjectionMatrex, ModelViewMatrex, light, getCameraPosition(), *fbos, Textures);
	
	if (clock() - lastRedraw > CLOCKS_PER_SEC / 65) {
		lastRedraw = clock();
		if (CTrain::isMove) {
			MoveTrain();
		}
		light.Move();
	}

	// this time drawing is for shadows (except for top view)
	/*if (this->camera != Top) {
		setupShadows();
		drawStuff(true);
		unsetupShadows();
	}*/

	
	//Call triangle's render function, pass ModelViewMatrex and ProjectionMatrex
 	//triangle->Paint(ProjectionMatrex,ModelViewMatrex);
    
	//we manage textures by Trainview class, so we modify square's render function
	//square->Begin();
	//	//Active Texture
	//	glActiveTexture(GL_TEXTURE0);
	//	//Bind square's texture
	//	//Textures[0]->bind();
	//	glBindTexture(GL_TEXTURE_2D, fbos->getReflectionTexture());
	//	//pass texture to shader
	//	square->shaderProgram->setUniformValue("Texture", 0);
	//	//Call square's render function, pass ModelViewMatrex and ProjectionMatrex
	//	square->Paint(ProjectionMatrex,ModelViewMatrex);
	//square->End();
	
}

//************************************************************************
//
// * This sets up both the Projection and the ModelView matrices
//   HOWEVER: it doesn't clear the projection first (the caller handles
//   that) - its important for picking
//========================================================================
void TrainView::setProjection()
//========================================================================
{
	if (camera == Train && !trains.empty()) {
		CTrain train = trains[currentTrain];
		glMatrixMode(GL_PROJECTION);
		double aspect = ((double)width() / (double)height());
		gluPerspective(40, aspect, .1, INFINITE);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		Pnt3f n = train.v - train.pos;
		n.normalize();
		Pnt3f v = train.v + train.v_orient * 20;
		Pnt3f pos = train.pos + train.orient * 20 - 20 * n;
		arcball->multMatrix();
		gluLookAt(pos.x, pos.y, pos.z,
				  v.x, v.y, v.z,
				  train.orient.x, train.orient.y, train.orient.z);
	}
	else {
		arcball->setProjection(false);
	}
	update();
}

void TrainView::drawSkyBox() {
	GLfloat viewMatrix[16];

	viewMatrix[0] = ModelViewMatrex[0];
	viewMatrix[1] = ModelViewMatrex[1];
	viewMatrix[2] = ModelViewMatrex[2];
	viewMatrix[3] = 0;
	viewMatrix[4] = ModelViewMatrex[4];
	viewMatrix[5] = ModelViewMatrex[5];
	viewMatrix[6] = ModelViewMatrex[6];
	viewMatrix[7] = 0;
	viewMatrix[8] = ModelViewMatrex[8];
	viewMatrix[9] = ModelViewMatrex[9];
	viewMatrix[10] = ModelViewMatrex[10];
	viewMatrix[11] = 0;
	viewMatrix[12] = 0;
	viewMatrix[13] = 0;
	viewMatrix[14] = 0;
	viewMatrix[15] = 1;


	skybox->Begin();
	glActiveTexture(GL_TEXTURE0);
	skybox->Render(ProjectionMatrex, viewMatrix);
	skybox->End();
}

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
void TrainView::drawStuff(QVector4D& clipPlane, bool doingShadows)
{

	this->terrain->Render(ProjectionMatrex, ModelViewMatrex, light, getCameraPosition(), Textures, clipPlane);
	this->m_pTrack->Draw(doingShadows, selectedPath);

	// Draw the control points
	// don't draw the control points if you're driving
	// (otherwise you get sea-sick as you drive through them)
	if (this->camera != Train) {
		int i = 0;
		QVector3D color;
		for (auto &p : this->m_pTrack->points) {
			if (!doingShadows) {
				if (p.first != selectedPoint)
					color = QVector3D(240 / 255.0, 60 / 255.0, 60 / 255.0);
				else
					color = QVector3D(240 / 255.0, 240 / 255.0, 30 / 255.0);
			}
			p.second.draw(color, ProjectionMatrex, ModelViewMatrex, light, getCameraPosition(), clipPlane);
			i++;
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

	for (int i = 0; i < trains.size(); i++) {
		trains[i].Draw(false, i == selectedTrain, light, getCameraPosition(), clipPlane);
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

void TrainView::doPick(int mx, int my)
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

	gluPickMatrix((double)mx, (double)(viewport[3] - my), 5, 5, viewport);

	// now set up the projection
	setProjection();

	GLfloat ProjectionMatrex[16];
	GLfloat ModelViewMatrex[16];
	glGetFloatv(GL_PROJECTION_MATRIX, ProjectionMatrex);
	glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrex); 

	// now draw the objects - but really only see what we hit
	GLuint buf[100];
	glSelectBuffer(100, buf);
	glRenderMode(GL_SELECT);
	glInitNames();
	glPushName(0);

	// draw the cubes, loading the names as we go
	auto it1 = m_pTrack->points.begin();
	for (int i = 0; it1 != m_pTrack->points.end(); i++, it1++) {
		glLoadName((GLuint)(i + 1));
		it1->second.draw(QVector3D(0, 0, 0), ProjectionMatrex, ModelViewMatrex, light, getCameraPosition());
	}

	auto it2 = m_pTrack->paths.begin();
	for (int i = 0; it2 != m_pTrack->paths.end(); i++, it2++) {
		glLoadName((GLuint)(i + 1 + m_pTrack->points.size()));
		for (auto &pathData : it2->second) {
			pathData.second.Draw(false, false);
		}
	}

	for (int i = 0; i < trains.size(); i++) {
		glLoadName((GLuint)(i + 1 + m_pTrack->points.size() + m_pTrack->paths.size()));
		trains[i].Draw(false, false, light, getCameraPosition());
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
		if (tmp < m_pTrack->points.size()) {
			auto it = m_pTrack->points.begin();
			advance(it, tmp);
			selectedPoint = it->first;
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
	if (m_pTrack->paths.empty())
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
	CTrain train(pd.p0, pd.p1, pd.p2, pd.p3, Head);
	trains.push_back(train);
}

void TrainView::MoveTrain() {
	for (auto &train : trains) {
		train.Move();
	}
}

void TrainView::RemoveTrain(int index) {
	trains.erase(trains.begin() + index);
}

QVector3D TrainView::getCameraPosition() {
	GLfloat ModelViewMatrex[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrex);
	QMatrix4x4 m(ModelViewMatrex);
	m = m.inverted();
	return QVector3D(m(3, 0), m(3, 1), m(3, 2));
}
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
	fogColor = QVector3D(0.5, 0.5, 0.5);
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

	ssao = new SSAO();
	ssao->Init();

	PathData::terrain = terrain;
	CTrack::terrain = terrain;

	fbos = new WaterFrameBuffer(this);
	ssaoFrameBuffer = new SSAOFrameBuffer(this);
	//Initialize texture

	glGenFramebuffers(1, &finalFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, finalFrameBuffer);
	glGenTextures(1, &finalTexture);
	glBindTexture(GL_TEXTURE_2D, finalTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1920, 1080, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, finalTexture, 0);
	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, 1920, 1080, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	vao = new QOpenGLVertexArrayObject();

	InitVAO();
	InitVBO();
	shaderProgram = InitShader("./Shader/MotionBlur.vs", "./Shader/MotionBlur.fs");
}

void TrainView::InitVAO() {
	vao->create();
	vao->bind();
}

void TrainView::InitVBO() {
	vertices
		<< QVector3D(-1,  1, 0)
		<< QVector3D(-1, -1, 0)
		<< QVector3D( 1,  1, 0)
		<< QVector3D( 1, -1, 0);

	vbo.create();
	vbo.bind();
	vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
	vbo.allocate(vertices.constData(), vertices.size() * sizeof(QVector3D));
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
	texture = new QOpenGLTexture(QImage("./Textures/water_height_map.jpg"));
	Textures.push_back(texture);
	texture = new QOpenGLTexture(QImage("./Textures/water.jpg"));
	Textures.push_back(texture);
}

void TrainView::resetArcball()
//========================================================================
{
	// Set up the camera to look at the world
	// these parameters might seem magical, and they kindof are
	// a little trial and error goes a long way
	cameras[0].setup(this, 40, 0, 0, 500, M_PI / 4, 0, 0);
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
	for (int i = 0; i < 16; i++) {
		lastModelViewMatrix[i] = ModelViewMatrix[i];
	}

	glGetFloatv(GL_PROJECTION_MATRIX, ProjectionMatrix);

	fbos->BindReflectionFrameBuffer();
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrix);
	QMatrix4x4 m(ModelViewMatrix);
	m = QMatrix4x4({
		1, 0, 0, 0,
		0, -1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
		}) * m;
	m.copyDataTo(ModelViewMatrix);
	glLoadMatrixf(ModelViewMatrix);

	drawSkyBox();
	setupObjects();
	GLdouble reflectionClipPlane[] = { 0, 1, 0, -1 };
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
	glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrix);
	
	drawSkyBox();
	setupObjects();
	GLdouble refractionClipPlane[] = { 0, -1, 0, 1 };
	glClipPlane(GL_CLIP_PLANE0, refractionClipPlane);
	glEnable(GL_CLIP_PLANE0);
	glEnable(GL_CLIP_DISTANCE0);
	drawStuff(QVector4D(refractionClipPlane[0], refractionClipPlane[1], refractionClipPlane[2], refractionClipPlane[3]));
	glDisable(GL_CLIP_PLANE0);
	glDisable(GL_CLIP_DISTANCE0);
	fbos->UnbindCurrentFrameBuffer();

	ssaoFrameBuffer->BindGeometryFrameBuffer();
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ssao->GeometryShaderBegin(ProjectionMatrix, ModelViewMatrix);
	setupObjects();
	drawGeometry();
	ssao->GeometryShaderEnd();
	ssaoFrameBuffer->UnbindCurrentFrameBuffer();
	
	ssaoFrameBuffer->BindSSAOFrameBuffer();
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	ssao->SSAOPass(ProjectionMatrix, *ssaoFrameBuffer, width(), height());
	ssaoFrameBuffer->UnbindCurrentFrameBuffer();

	ssaoFrameBuffer->BindBlurFrameBuffer();
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	ssao->BlurPass(*ssaoFrameBuffer);
	ssaoFrameBuffer->UnbindCurrentFrameBuffer();

	glBindFramebuffer(GL_FRAMEBUFFER, finalFrameBuffer);
	glViewport(0, 0, 1920, 1080);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	drawSkyBox();
	setupObjects();
	drawStuff();
	this->water->Render((clock() - lastRedraw) / 65.0, ProjectionMatrix, ModelViewMatrix, light, getCameraPosition(), *fbos, Textures);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(0, 0, width(), height());
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Render();


	skybox->Rotate((float)(clock() - lastRedraw) / 65.0);
	if (clock() - lastRedraw > CLOCKS_PER_SEC / 65) {
		lastRedraw = clock();
		if (CTrain::isMove) {
			MoveTrain();
		}
		light.Move();
		if (light.position.y() < 20000 && light.position.y() > -20000) {
			fogColor = QVector3D(0.5, 0.5, 0.5) * ((light.position.y() + 20000) / 40000.0);
		}
		else if (light.position.y() > 20000) {
			fogColor = QVector3D(0.5, 0.5, 0.5);
		}
		else {
			fogColor = QVector3D(0, 0, 0);
		}
	}

	
	//Call triangle's render function, pass ModelViewMatrix and ProjectionMatrix
 	//triangle->Paint(ProjectionMatrix,ModelViewMatrix);
    
	//we manage textures by Trainview class, so we modify square's render function
	//square->Begin();
	//	//Active Texture
	//	glActiveTexture(GL_TEXTURE0);
	//	//Bind square's texture
	//	//Textures[0]->bind();
	//	glBindTexture(GL_TEXTURE_2D, fbos->getReflectionTexture());
	//	//pass texture to shader
	//	square->shaderProgram->setUniformValue("Texture", 0);
	//	//Call square's render function, pass ModelViewMatrix and ProjectionMatrix
	//	square->Paint(ProjectionMatrix,ModelViewMatrix);
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
		gluPerspective(40, aspect, .1, 1e10);
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

	viewMatrix[0] = ModelViewMatrix[0];
	viewMatrix[1] = ModelViewMatrix[1];
	viewMatrix[2] = ModelViewMatrix[2];
	viewMatrix[3] = 0;
	viewMatrix[4] = ModelViewMatrix[4];
	viewMatrix[5] = ModelViewMatrix[5];
	viewMatrix[6] = ModelViewMatrix[6];
	viewMatrix[7] = 0;
	viewMatrix[8] = ModelViewMatrix[8];
	viewMatrix[9] = ModelViewMatrix[9];
	viewMatrix[10] = ModelViewMatrix[10];
	viewMatrix[11] = 0;
	viewMatrix[12] = 0;
	viewMatrix[13] = 0;
	viewMatrix[14] = 0;
	viewMatrix[15] = 1;

	skybox->Render(ProjectionMatrix, viewMatrix, fogColor, light.position);
}

void TrainView::Render() {
	GLfloat P[4][4];
	GLfloat V[4][4];
	GLfloat lastV[4][4];
	DimensionTransformation(ProjectionMatrix, P);
	DimensionTransformation(ModelViewMatrix, V);
	DimensionTransformation(lastModelViewMatrix, lastV);

	shaderProgram->bind();
	vao->bind();

	shaderProgram->setUniformValue("projection", P);
	shaderProgram->setUniformValue("view", V);
	shaderProgram->setUniformValue("lastView", lastV);

	shaderProgram->setUniformValue("colorMap", 0);
	shaderProgram->setUniformValue("depthMap", 1);

	shaderProgram->setUniformValue("near", GLfloat(0.1));
	shaderProgram->setUniformValue("far", GLfloat(1e10));

	vbo.bind();
	shaderProgram->enableAttributeArray(0);
	shaderProgram->setAttributeArray(0, GL_FLOAT, 0, 3, NULL);
	vbo.release();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, finalTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthTexture);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, vertices.size());

	shaderProgram->disableAttributeArray(0);

	vao->release();
	shaderProgram->release();
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
	this->terrain->Render(ProjectionMatrix, ModelViewMatrix, light, getCameraPosition(), fogColor, Textures, ssaoFrameBuffer, renderMode, clipPlane);
	this->m_pTrack->Draw(false, selectedPath);
	
	if (this->camera != Train) {
		QVector3D color;
		for (auto &p : this->m_pTrack->points) {
			if (p.first != selectedPoint)
				color = QVector3D(240 / 255.0, 60 / 255.0, 60 / 255.0);
			else
				color = QVector3D(240 / 255.0, 240 / 255.0, 30 / 255.0);
			p.second->draw(color, ProjectionMatrix, ModelViewMatrix, light, getCameraPosition(), ssaoFrameBuffer, renderMode, clipPlane);
		}
		update();
	}

	for (int i = 0; i < trains.size(); i++) {
		trains[i].Draw(false, i == selectedTrain, light, getCameraPosition(), ssaoFrameBuffer, renderMode, this->camera != Train || this->drawTrain, clipPlane);
	}
}

void TrainView::drawGeometry() {
	this->terrain->DrawGeometry(ssao->currentShader);
	if (this->camera != Train) {
		for (auto &p : this->m_pTrack->points) {
			p.second->DrawGeometry(ssao->currentShader);
		}
	}
	for (int i = 0; i < trains.size(); i++) {
		trains[i].DrawGeometry(ssao->currentShader);
	}
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

	GLfloat ProjectionMatrix[16];
	GLfloat ModelViewMatrix[16];
	glGetFloatv(GL_PROJECTION_MATRIX, ProjectionMatrix);
	glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrix); 

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
		it1->second->draw(QVector3D(0, 0, 0), ProjectionMatrix, ModelViewMatrix, light, getCameraPosition(), ssaoFrameBuffer, 0);
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
		trains[i].Draw(false, false, light, getCameraPosition(), ssaoFrameBuffer, 0, true);
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
		auto children = m_pTrack->points[selectedPoint]->children;
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
	GLfloat ModelViewMatrix[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrix);
	QMatrix4x4 m(ModelViewMatrix);
	m = m.inverted();
	return QVector3D(m(3, 0), m(3, 1), m(3, 2));
}
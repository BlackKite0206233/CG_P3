#include "AppMain.h"

#include "Utilities/3DUtils.h"
#include "Track.h"
#include <math.h>
#include <time.h>

AppMain* AppMain::Instance = NULL;
AppMain::AppMain(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	trainview = new TrainView();  
	trainview->m_pTrack =  &m_Track;
	setGeometry(100,25,1000,768);   
	ui.mainLayout->layout()->addWidget(trainview);
	trainview->installEventFilter(this);
	this->currentMode = None;
	this->canpan = false;
	this->isHover = false;
	this->trainview->camera = 0;
	this->trainview->m_pTrack->track = 0;
	this->trainview->m_pTrack->curve = 0;
	CTrain::isMove = false;

	setWindowTitle( "Roller Coaster" );

	connect( ui.aLoadPath	,SIGNAL(triggered()),this,SLOT(LoadTrackPath())	);
	connect( ui.aSavePath	,SIGNAL(triggered()),this,SLOT(SaveTrackPath())	);
	connect( ui.aExit		,SIGNAL(triggered()),this,SLOT(ExitApp())		);

	connect( ui.comboCamera	,SIGNAL(currentIndexChanged(QString)),this,SLOT(ChangeCameraType(QString)));
	connect( ui.aWorld		,SIGNAL(triggered()),this,SLOT(ChangeCamToWorld())	);
	connect( ui.aTop		,SIGNAL(triggered()),this,SLOT(ChangeCamToTop())	);
	connect( ui.aTrain		,SIGNAL(triggered()),this,SLOT(ChangeCamToTrain())	);

	connect( ui.comboCurve	,SIGNAL(currentIndexChanged(QString)),this,SLOT(ChangeCurveType(QString)));
	connect( ui.aLinear		,SIGNAL(triggered()),this,SLOT(ChangeCurveToLinear())	);
	connect( ui.aCardinal	,SIGNAL(triggered()),this,SLOT(ChangeCurveToCardinal())	);
	connect( ui.aCubic		,SIGNAL(triggered()),this,SLOT(ChangeCurveToCubic())	);

	connect( ui.comboTrack	,SIGNAL(currentIndexChanged(QString)),this,SLOT(ChangeTrackType(QString)));
	connect( ui.aLine		,SIGNAL(triggered()),this,SLOT(ChangeTrackToLine())		);
	connect( ui.aTrack		,SIGNAL(triggered()),this,SLOT(ChangeTrackToTrack())	);
	connect( ui.aRoad		,SIGNAL(triggered()),this,SLOT(ChangeTrackToRoad())		);

	connect( ui.bPlay		,SIGNAL(clicked()),this,SLOT(SwitchPlayAndPause())				);
	connect( ui.sSpeed		,SIGNAL(valueChanged(int)),this,SLOT(ChangeSpeedOfTrain(int))	);
	connect( ui.bAdd		,SIGNAL(clicked()),this,SLOT(AddControlPoint())					);
	connect( ui.bDelete		,SIGNAL(clicked()),this,SLOT(DeleteControlPoint())				);

	connect( ui.rcpxadd		,SIGNAL(clicked()),this,SLOT(RotateControlPointAddX())					);
	connect( ui.rcpxsub		,SIGNAL(clicked()),this,SLOT(RotateControlPointSubX())				);
	connect( ui.rcpzadd		,SIGNAL(clicked()),this,SLOT(RotateControlPointAddZ())					);
	connect( ui.rcpzsub		,SIGNAL(clicked()),this,SLOT(RotateControlPointSubZ())				);
}

AppMain::~AppMain()
{

}

void changeMode(int& currentMode, Mode newMode) {
	currentMode = (currentMode == newMode) ? None : newMode;
}

bool AppMain::eventFilter(QObject *watched, QEvent *e) {
	if (e->type() == QEvent::MouseButtonPress) {
		QMouseEvent *event = static_cast<QMouseEvent*> (e);
		// Get the mouse position
		float x, y;
		trainview->arcball->getMouseNDC((float)event->localPos().x(), (float)event->localPos().y(), x,y);

		// Compute the mouse position
		trainview->arcball->down(x, y);
		if(event->button()==Qt::LeftButton){
			this->isHover = true;
			switch (currentMode) {
			case None:
				trainview->doPick(event->localPos().x(), event->localPos().y());
				break;
			case InsertPoint:
				break;
			case InsertTrain:
				trainview->AddTrain();
				break;
			case InsertCar:
				break;
			}

			if(this->canpan)
				trainview->arcball->mode = trainview->arcball->Pan;
			else {
				trainview->arcball->mode = trainview->arcball->None;
			}
		}
		if(event->button()==Qt::RightButton){
			trainview->arcball->mode = trainview->arcball->Rotate;
		}
	}

	if (e->type() == QEvent::MouseButtonRelease) {
		// this->canpan = false;
		this->isHover = false;
		trainview->arcball->mode = trainview->arcball->None;
	}

	if (e->type() == QEvent::Wheel) {
		QWheelEvent *event = static_cast<QWheelEvent*> (e);
		float zamt = (event->delta() < 0) ? 1.1f : 1/1.1f;
		trainview->arcball->eyeZ *= zamt;
	}

	if (e->type() == QEvent::MouseMove) {
		QMouseEvent *event = static_cast<QMouseEvent*> (e);
		if(isHover && trainview->selectedCube >= 0){
			ControlPoint* cp = &trainview->m_pTrack->points[trainview->selectedCube];

			double r1x, r1y, r1z, r2x, r2y, r2z;
			int x = event->localPos().x();
			int iy = event->localPos().y();
			double mat1[16],mat2[16];		// we have to deal with the projection matrices
			int viewport[4];

			glGetIntegerv(GL_VIEWPORT, viewport);
			glGetDoublev(GL_MODELVIEW_MATRIX,mat1);
			glGetDoublev(GL_PROJECTION_MATRIX,mat2);

			int y = viewport[3] - iy; // originally had an extra -1?

			int i1 = gluUnProject((double) x, (double) y, .25, mat1, mat2, viewport, &r1x, &r1y, &r1z);
			int i2 = gluUnProject((double) x, (double) y, .75, mat1, mat2, viewport, &r2x, &r2y, &r2z);

			if (currentMode == None) {
				double rx, ry, rz;
				mousePoleGo(r1x, r1y, r1z, r2x, r2y, r2z, 
					static_cast<double>(cp->pos.x), 
					static_cast<double>(cp->pos.y),
					static_cast<double>(cp->pos.z),
					rx, ry, rz,
					false);

				cp->pos.x = (float) rx;
				cp->pos.y = (float) ry;
				cp->pos.z = (float) rz;
			}
			else if (currentMode == RotatePoint) {

			}

			trainview->m_pTrack->BuildTrack();
		}
		if(trainview->arcball->mode != trainview->arcball->None) { // we're taking the drags
			float x,y;
			trainview->arcball->getMouseNDC((float)event->localPos().x(), (float)event->localPos().y(),x,y);
			trainview->arcball->computeNow(x,y);
		};
	}

	if(e->type() == QEvent::KeyPress){
		QKeyEvent *event = static_cast< QKeyEvent*> (e);
		// Set up the mode
		if (event->key() == Qt::Key_Alt) {
			this->canpan = true;
		}
	}

	if (e->type() == QEvent::KeyRelease) {
		QKeyEvent* event = static_cast<QKeyEvent*> (e);
		// Set up the mode
		switch (event->key()) {
		case Qt::Key_Alt:
			this->canpan = false;
			trainview->arcball->mode = trainview->arcball->None;
			break;

		case Qt::Key_P:
			changeMode(currentMode, InsertPoint);
			break;
		case Qt::Key_T:
			changeMode(currentMode, InsertTrain);
			break;
		case Qt::Key_C:
			changeMode(currentMode, InsertCar);
			break;
		case Qt::Key_R:
			changeMode(currentMode, RotatePoint);
			break;

		case Qt::Key_1:
			trainview->SetCamera(World);
			break;
		case Qt::Key_2:
			trainview->SetCamera(Top);
			break;
		case Qt::Key_3:
			trainview->SetCamera(Train);
			break;

		case Qt::Key_4:
			trainview->m_pTrack->SetCurve(Linear);
			break;
		case Qt::Key_5:
			trainview->m_pTrack->SetCurve(Cardinal);
			break;
		case Qt::Key_6:
			trainview->m_pTrack->SetCurve(Cubic);
			break;

		case Qt::Key_7:
			trainview->m_pTrack->track = Line;
			break;
		case Qt::Key_8:
			trainview->m_pTrack->track = Track;
			break;
		case Qt::Key_9:
			trainview->m_pTrack->track = Road;
			break;
		}
	}

	return QWidget::eventFilter(watched, e);
}

void AppMain::ExitApp()
{
	QApplication::quit();
}

AppMain * AppMain::getInstance()
{
	if( !Instance )
	{
		Instance = new AppMain();
		return Instance;
	}
	else 
		return Instance;
}

void AppMain::ToggleMenuBar()
{
	ui.menuBar->setHidden( !ui.menuBar->isHidden() );
}

void AppMain::ToggleToolBar()
{
	ui.mainToolBar->setHidden( !ui.mainToolBar->isHidden() );
}

void AppMain::ToggleStatusBar()
{
	ui.statusBar->setHidden( !ui.statusBar->isHidden() );
}

void AppMain::LoadTrackPath()
{
	QString fileName = QFileDialog::getOpenFileName( 
		this,
		"OpenImage",
		"./",
		tr("Txt (*.txt)" )
		);
	QByteArray byteArray = fileName.toLocal8Bit();
	const char* fname = byteArray.data();
	if ( !fileName.isEmpty() )
	{
		this->m_Track.readPoints(fname);
	}
}

void AppMain::SaveTrackPath()
{
	QString fileName = QFileDialog::getSaveFileName( 
		this,
		"OpenImage",
		"./",
		tr("Txt (*.txt)" )
		);

	QByteArray byteArray = fileName.toLocal8Bit();
	const char* fname = byteArray.data();
	if ( !fileName.isEmpty() )
	{
		this->m_Track.writePoints(fname);
	}
}

void AppMain::TogglePanel()
{
	if( !ui.groupCamera->isHidden() )
	{
		ui.groupCamera->hide();
		ui.groupCurve->hide();
		ui.groupTrack->hide();
		ui.groupPlay->hide();
		ui.groupCP->hide();
	}
	else
	{
		ui.groupCamera->show();
		ui.groupCurve->show();
		ui.groupTrack->show();
		ui.groupPlay->show();
		ui.groupCP->show();
	}
}

void AppMain::ChangeCameraType( QString type )
{
	if( type == "World" )
		this->trainview->SetCamera(World);
	else if( type == "Top" )
		this->trainview->SetCamera(Top);
	else if( type == "Train" )
		this->trainview->SetCamera(Train);
	update();
}

void AppMain::ChangeCurveType( QString type )
{
	if( type == "Linear" )
		trainview->m_pTrack->SetCurve(Linear);
	else if( type == "Cardinal" )
		trainview->m_pTrack->SetCurve(Cardinal);
	else if( type == "Cubic" )
		trainview->m_pTrack->SetCurve(Cubic);
}

void AppMain::ChangeTrackType( QString type )
{
	if( type == "Line" )
		this->trainview->m_pTrack->track = Line;
	else if( type == "Track" )
		this->trainview->m_pTrack->track = Track;
	else if( type == "Road" )
		this->trainview->m_pTrack->track = Road;
}


void AppMain::SwitchPlayAndPause()
{
	CTrain::isMove = !CTrain::isMove;
	if( !CTrain::isMove )
	{
		ui.bPlay->setIcon(QIcon(":/AppMain/Resources/Icons/play.ico"));
	}
	else
	{
		ui.bPlay->setIcon(QIcon(":/AppMain/Resources/Icons/pause.ico"));
	}
	/*if(CTrain::isMove) {
		if (clock() - lastRedraw > CLOCKS_PER_SEC / 30) {
			lastRedraw = clock();
			this->advanceTrain();
			this->damageMe();
		}
	}*/
}

void AppMain::ChangeSpeedOfTrain( int val )
{
	//m_rollerCoaster->trainSpeed = m_rollerCoaster->MAX_TRAIN_SPEED * float(val) / 100.0f;
}

void AppMain::AddControlPoint()
{
	// get the number of points
	size_t npts = this->m_Track.points.size();
	// the number for the new point
	size_t newidx = (this->trainview->selectedCube>=0) ? this->trainview->selectedCube : 0;

	// pick a reasonable location
	size_t previdx = (newidx + npts -1) % npts;
	Pnt3f npos = (this->m_Track.points[previdx].pos + this->m_Track.points[newidx].pos) * .5f;

	this->m_Track.points.insert(this->m_Track.points.begin() + newidx,npos);

	// make it so that the train doesn't move - unless its affected by this control point
	// it should stay between the same points
	if (ceil(this->m_Track.trainU) > ((float)newidx)) {
		this->m_Track.trainU += 1;
		if (this->m_Track.trainU >= npts) this->m_Track.trainU -= npts;
	}
	this->damageMe();
}

void AppMain::DeleteControlPoint()
{
	if (this->m_Track.points.size() > 4) {
		if (this->trainview->selectedCube >= 0) {
			this->m_Track.points.erase(this->m_Track.points.begin() + this->trainview->selectedCube);
		} else
			this->m_Track.points.pop_back();
	}
	this->damageMe();
}


//***************************************************************************
//
// * Rotate the selected control point about x axis
//===========================================================================
void AppMain::rollx(float dir)
{
	int s = this->trainview->selectedCube;
	if (s >= 0) {
		Pnt3f old = this->m_Track.points[s].orient;
		float si = sin(((float)M_PI_4) * dir);
		float co = cos(((float)M_PI_4) * dir);
		this->m_Track.points[s].orient.y = co * old.y - si * old.z;
		this->m_Track.points[s].orient.z = si * old.y + co * old.z;
	}
	this->damageMe();
} 

void AppMain::RotateControlPointAddX()
{
	rollx(1);
}

void AppMain::RotateControlPointSubX()
{
	rollx(-1);
}

void AppMain::rollz(float dir)
{
	int s = this->trainview->selectedCube;
	if (s >= 0) {

		Pnt3f old = this->m_Track.points[s].orient;

		float si = sin(((float)M_PI_4) * dir);
		float co = cos(((float)M_PI_4) * dir);

		this->m_Track.points[s].orient.y = co * old.y - si * old.x;
		this->m_Track.points[s].orient.x = si * old.y + co * old.x;
	}
	this->damageMe();
} 

void AppMain::RotateControlPointAddZ()
{
	rollz(1);
}

void AppMain::RotateControlPointSubZ()
{
	rollz(-1);
}

void AppMain::ChangeCamToWorld()
{
	this->trainview->SetCamera(World);
}

void AppMain::ChangeCamToTop()
{
	this->trainview->SetCamera(Top);
}

void AppMain::ChangeCamToTrain()
{
	this->trainview->SetCamera(Train);
}

void AppMain::ChangeCurveToLinear()
{
	this->trainview->m_pTrack->SetCurve(Linear);
}

void AppMain::ChangeCurveToCardinal()
{
	this->trainview->m_pTrack->SetCurve(Cardinal);
}

void AppMain::ChangeCurveToCubic()
{
	this->trainview->m_pTrack->SetCurve(Cubic);
}

void AppMain::ChangeTrackToLine()
{
	this->trainview->m_pTrack->track = Line;
}

void AppMain::ChangeTrackToTrack()
{
	this->trainview->m_pTrack->track = Track;
}

void AppMain::ChangeTrackToRoad()
{
	this->trainview->m_pTrack->track = Road;
}

void AppMain::UpdateCameraState( int index )
{
	ui.aWorld->setChecked( (index==0)?true:false );
	ui.aTop	 ->setChecked( (index==1)?true:false );
	ui.aTrain->setChecked( (index==2)?true:false );
}

void AppMain::UpdateCurveState( int index )
{
	ui.aLinear	->setChecked( (index==0)?true:false );
	ui.aCardinal->setChecked( (index==1)?true:false );
	ui.aCubic	->setChecked( (index==2)?true:false );
}

void AppMain::UpdateTrackState( int index )
{
	ui.aLine ->setChecked( (index==0)?true:false );
	ui.aTrack->setChecked( (index==1)?true:false );
	ui.aRoad ->setChecked( (index==2)?true:false );
}

//************************************************************************
//
// *
//========================================================================
void AppMain::
damageMe()
//========================================================================
{
	if (trainview->selectedCube >= ((int)m_Track.points.size()))
		trainview->selectedCube = 0;
	//trainview->damage(1);
}

//************************************************************************
//
// * This will get called (approximately) 30 times per second
//   if the run button is pressed
//========================================================================
void AppMain::
advanceTrain(float dir)
//========================================================================
{
	//#####################################################################
	// TODO: make this work for your train
	//#####################################################################
}

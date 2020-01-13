#include "AppMain.h"

#include "Utilities/3DUtils.h"
#include "Track.h"
#include <math.h>
#include <time.h>

AppMain *AppMain::Instance = NULL;
AppMain::AppMain(QWidget *parent) : QMainWindow(parent) {
	ui.setupUi(this);
	trainview = new TrainView();
	setGeometry(100, 25, 1000, 768);
	ui.mainLayout->layout()->addWidget(trainview);
	trainview->installEventFilter(this);
	trainview->m_pTrack = &m_Track;
	CTrain::track = &m_Track;
	this->currentMode = None;
	this->canpan = false;
	this->isHover = false;
	this->rotatePoint = false;
	this->trainview->camera = World;
	PathData::curve = Linear;
	PathData::track = Line;
	CTrain::isMove = false;
	CTrain::speed0 = 3;
	PathData::speed = 0.3;

	setWindowTitle("Roller Coaster");

	connect(ui.aLoadPath, SIGNAL(triggered()), this, SLOT(LoadTrackPath()));
	connect(ui.aSavePath, SIGNAL(triggered()), this, SLOT(SaveTrackPath()));
	connect(ui.aExit,     SIGNAL(triggered()), this, SLOT(ExitApp()));

	ui.label->setText("Mode: Normal");
}

AppMain::~AppMain() {
}

void AppMain::changeMode(int &currentMode, Mode newMode) {
	currentMode = (currentMode == newMode) ? None : newMode;
	trainview->lastSelectedPoint = trainview->selectedPath = trainview->selectedPoint = trainview->selectedTrain = -1;
	QString s;
	switch (currentMode) {
	case None:
		s = "Normal";
		break;
	case InsertPoint:
		s = "InsertPoint";
		break;
	case InsertPath:
		s = "InsertPath";
		break;
	case InsertTrain:
		s = "InsertTrain";
		break;
	}
	ui.label->setText("Mode: " + s);
}

bool AppMain::eventFilter(QObject *watched, QEvent *e) {
	if (e->type() == QEvent::MouseButtonPress) {
		QMouseEvent *event = static_cast<QMouseEvent *>(e);
		// Get the mouse position
		float x, y;
		trainview->arcball->getMouseNDC((float)event->localPos().x(), (float)event->localPos().y(), x, y);

		// Compute the mouse position
		trainview->arcball->down(x, y);
		if (event->button() == Qt::LeftButton) {
			if (trainview->camera == Train) {
				return QWidget::eventFilter(watched, e);
			}
			this->isHover = true;
			trainview->doPick(event->localPos().x(), event->localPos().y());
			ControlPoint p;
			switch (currentMode) {
			case None:
				break;
			case InsertPath:
				if (trainview->selectedPoint >= 0 && trainview->lastSelectedPoint >= 0) {
					trainview->m_pTrack->AddPath(trainview->lastSelectedPoint, trainview->selectedPoint);
				}
				trainview->lastSelectedPoint = trainview->selectedPoint;
				break;
			case InsertPoint:
			{
				double r1x, r1y, r1z, r2x, r2y, r2z;
				int x = event->localPos().x();
				int iy = event->localPos().y();
				double mat1[16], mat2[16];
				int viewport[4];

				glGetIntegerv(GL_VIEWPORT, viewport);
				glGetDoublev(GL_MODELVIEW_MATRIX, mat1);
				glGetDoublev(GL_PROJECTION_MATRIX, mat2);

				int y = viewport[3] - iy;

				int i1 = gluUnProject((double)x, (double)y, .25, mat1, mat2, viewport, &r1x, &r1y, &r1z);
				int i2 = gluUnProject((double)x, (double)y, .75, mat1, mat2, viewport, &r2x, &r2y, &r2z);

				double rx, ry, rz;
				mousePoleGo(r1x, r1y, r1z, r2x, r2y, r2z,
					static_cast<double>(0),
					static_cast<double>(0),
					static_cast<double>(0),
					rx, ry, rz,
					false);

				float height = trainview->terrain->getHeightOfTerrain(rx, rz);
				if (height < 0)
					height = 0;
				p.center = CtrlPoint(Pnt3f(rx, height + 5, rz), Pnt3f(0, 1, 0));
				trainview->m_pTrack->AddPoint(p);
			}
				break;
			case InsertTrain:
				trainview->AddTrain();
				break;
			}

			if (this->canpan)
				trainview->arcball->mode = trainview->arcball->Pan;
			else {
				trainview->arcball->mode = trainview->arcball->None;
			}

			if (this->rotatePoint && trainview->selectedPoint >= 0) {
				trainview->m_pTrack->points[trainview->selectedPoint].setCenter((float)event->localPos().x(), (float)event->localPos().y());
			}
		}
		if (event->button() == Qt::RightButton) {
			if (!(rotatePoint && trainview->selectedPoint >= 0))
				trainview->arcball->mode = trainview->arcball->Rotate;
			else {
				trainview->m_pTrack->points[trainview->selectedPoint].getMouseNDC((float)event->localPos().x(), (float)event->localPos().y(), x, y);
				trainview->m_pTrack->points[trainview->selectedPoint].down(x, y);
			}
		}
	}

	if (e->type() == QEvent::MouseButtonRelease) {
		// this->canpan = false;
		this->isHover = false;
		trainview->arcball->mode = trainview->arcball->None;
	}

	if (e->type() == QEvent::Wheel) {
		if (trainview->camera == Train) {
			return QWidget::eventFilter(watched, e);
		}
		QWheelEvent *event = static_cast<QWheelEvent *>(e);
		float zamt = (event->delta() < 0) ? 1.1f : 1 / 1.1f;
		trainview->arcball->eyeZ *= zamt;
	}

	if (e->type() == QEvent::MouseMove) {
		QMouseEvent *event = static_cast<QMouseEvent *>(e);
		if ((rotatePoint || isHover) && trainview->selectedPoint >= 0) {
			if (trainview->camera == Train) {
				return QWidget::eventFilter(watched, e);
			}
			ControlPoint *cp = &trainview->m_pTrack->points[trainview->selectedPoint];
			if (!rotatePoint) {
				double r1x, r1y, r1z, r2x, r2y, r2z;
				int x = event->localPos().x();
				int iy = event->localPos().y();
				double mat1[16], mat2[16]; // we have to deal with the projection matrices
				int viewport[4];

				glGetIntegerv(GL_VIEWPORT, viewport);
				glGetDoublev(GL_MODELVIEW_MATRIX, mat1);
				glGetDoublev(GL_PROJECTION_MATRIX, mat2);

				int y = viewport[3] - iy; // originally had an extra -1?

				int i1 = gluUnProject((double)x, (double)y, .25, mat1, mat2, viewport, &r1x, &r1y, &r1z);
				int i2 = gluUnProject((double)x, (double)y, .75, mat1, mat2, viewport, &r2x, &r2y, &r2z);

				double rx, ry, rz;
				mousePoleGo(r1x, r1y, r1z, r2x, r2y, r2z,
							static_cast<double>(cp->center.pos.x),
							static_cast<double>(cp->center.pos.y),
							static_cast<double>(cp->center.pos.z),
							rx, ry, rz,
							false);

				cp->center.pos.x = (float)rx;
				cp->center.pos.z = (float)rz;
				float height = trainview->terrain->getHeightOfTerrain(rx, rz);
				if (cp->center.pos.y < height + 5) {
					cp->center.pos.y = height + 5;
				}
			}
			else if (!isHover) {
				float x, y;
				trainview->m_pTrack->points[trainview->selectedPoint].getMouseNDC((float)event->localPos().x(), (float)event->localPos().y(), x, y);
				trainview->m_pTrack->points[trainview->selectedPoint].computeNow(x, y);
			}

			trainview->m_pTrack->BuildTrack();
		}
		if (trainview->arcball->mode != trainview->arcball->None) { // we're taking the drags
			float x, y;
			trainview->arcball->getMouseNDC((float)event->localPos().x(), (float)event->localPos().y(), x, y);
			trainview->arcball->computeNow(x, y);
		}
	}

	if (e->type() == QEvent::KeyPress) {
		QKeyEvent *event = static_cast<QKeyEvent *>(e);
		// Set up the mode
		switch (event->key()) {
		case Qt::Key_Alt:
			this->canpan = true;
			break;

		case Qt::Key_Plus:
			CTrain::speed0 += 0.2;
			if (CTrain::speed0 > 5)
				CTrain::speed0 = 5;
			break;
		case Qt::Key_Minus:
			CTrain::speed0 -= 0.2;
			if (CTrain::speed0 < 0.2)
				CTrain::speed0 = 0.2;
			break;

		case Qt::Key_R:
			rotatePoint = true;
			break;

		case Qt::Key_Up:
			if (trainview->selectedPoint >= 0) {
				ControlPoint *cp = &trainview->m_pTrack->points[trainview->selectedPoint];
				cp->center.pos.y += 1;
				trainview->m_pTrack->BuildTrack();
			}
			break;
		case Qt::Key_Down:
			if (trainview->selectedPoint >= 0) {
				ControlPoint *cp = &trainview->m_pTrack->points[trainview->selectedPoint];
				cp->center.pos.y -= 1;
				trainview->m_pTrack->BuildTrack();
			}
			break;
		}
	}

	if (e->type() == QEvent::KeyRelease) {
		QKeyEvent *event = static_cast<QKeyEvent *>(e);
		// Set up the mode
		switch (event->key()) {
		case Qt::Key_Alt:
			this->canpan = false;
			trainview->arcball->mode = trainview->arcball->None;
			break;

		case Qt::Key_N:
			if (trainview->trains.size())
				trainview->currentTrain = (trainview->currentTrain + 1) % trainview->trains.size();
			break;

		case Qt::Key_Escape:
			changeMode(currentMode, None);
			break;
		case Qt::Key_C:
			changeMode(currentMode, InsertPoint);
			break;
		case Qt::Key_P:
			changeMode(currentMode, InsertPath);
			break;
		case Qt::Key_T:
			changeMode(currentMode, InsertTrain);
			break;

		case Qt::Key_R:
			rotatePoint = false;
			break;

		case Qt::Key_D:
			if (trainview->selectedPoint >= 0) {
				trainview->m_pTrack->RemovePoint(trainview->selectedPoint);
				trainview->selectedPoint = -1;
			}
			else if (trainview->selectedPath >= 0) {
				auto it = trainview->m_pTrack->paths.begin();
				advance(it, trainview->selectedPath);
				trainview->m_pTrack->RemovePath(it->first.first, it->first.second);
				trainview->selectedPath = -1;
			}
			else if (trainview->selectedTrain >= 0) {
				trainview->RemoveTrain(trainview->selectedTrain);
				trainview->selectedTrain = -1;
			}
			break;

		case Qt::Key_Q:
			if (trainview->selectedTrain >= 0)
				trainview->trains[trainview->selectedTrain].AddCar();
			break;
		case Qt::Key_W:
			if (trainview->selectedTrain >= 0)
				trainview->trains[trainview->selectedTrain].RemoveCar();
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
			PathData::track = Line;
			break;
		case Qt::Key_8:
			PathData::track = Track;
			break;
		case Qt::Key_9:
			PathData::track = Road;
			break;

		case Qt::Key_0:
			trainview->arcball->reset();
			break;

		case Qt::Key_Space:
			SwitchPlayAndPause();
			break;
		}
	}

	return QWidget::eventFilter(watched, e);
}

void AppMain::ExitApp() {
	QApplication::quit();
}

AppMain *AppMain::getInstance() {
	if (!Instance) {
		Instance = new AppMain();
		return Instance;
	}
	else
		return Instance;
}

void AppMain::ToggleMenuBar() {
	ui.menuBar->setHidden(!ui.menuBar->isHidden());
}

void AppMain::ToggleToolBar() {
	ui.mainToolBar->setHidden(!ui.mainToolBar->isHidden());
}

void AppMain::ToggleStatusBar() {
	ui.statusBar->setHidden(!ui.statusBar->isHidden());
}

void AppMain::LoadTrackPath() {
	QString fileName = QFileDialog::getOpenFileName(
		this,
		"OpenImage",
		"./",
		tr("Txt (*.txt)"));
	QByteArray byteArray = fileName.toLocal8Bit();
	const char *fname = byteArray.data();
	if (!fileName.isEmpty()) {
		this->m_Track.readPoints(fname);
	}
}

void AppMain::SaveTrackPath() {
	QString fileName = QFileDialog::getSaveFileName(
		this,
		"OpenImage",
		"./",
		tr("Txt (*.txt)"));

	QByteArray byteArray = fileName.toLocal8Bit();
	const char *fname = byteArray.data();
	if (!fileName.isEmpty()) {
		this->m_Track.writePoints(fname);
	}
}

void AppMain::SwitchPlayAndPause() {
	CTrain::isMove = !CTrain::isMove;
}
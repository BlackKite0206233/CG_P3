#ifndef APPMAIN_H
#define APPMAIN_H

#include <QMainWindow>
#include <QWidget>
#include <QFileDialog>
#include "ui_AppMain.h"
#include <qgl.h>
#include "TrainView.h"  
#include "Track.h"

class CTrack;

enum Mode {
	None,
	InsertPath,
	InsertPoint,
	InsertTrain,
};

class AppMain : public QMainWindow
{
	Q_OBJECT

public:
	AppMain(QWidget *parent = 0);
	~AppMain();

	void ToggleMenuBar();
	void ToggleToolBar();
	void ToggleStatusBar();

	static AppMain *getInstance();
	static AppMain *Instance;

public:
	// call this method when things change

public:
	// keep track of the stuff in the world
	CTrack				m_Track;

	// the widgets that make up the Window
	TrainView*			trainview;

	bool canpan;
	bool isHover;
	bool rotatePoint;

private:
	Ui::AppMainClass ui;

	private slots:
		void LoadTrackPath();
		void SaveTrackPath();
		void ExitApp();

		void SwitchPlayAndPause();

		void changeMode(int& currentMode, Mode newMode);

protected:
	bool eventFilter(QObject *watched, QEvent *e); 

	int currentMode;
};

#endif // APPMAIN_H

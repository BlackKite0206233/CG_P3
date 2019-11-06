/************************************************************************
     File:        Track.H

     Author:     
                  Michael Gleicher, gleicher@cs.wisc.edu
     Modifier
                  Yu-Chi Lai, yu-chi@cs.wisc.edu
     
     Comment:     Container for the "World"

						This provides a container for all of the "stuff" 
						in the world.

						It could have been all global variables, or it could 
						have just been
						contained in the window. The advantage of doing it 
						this way is that
						we might have multiple windows looking at the same 
						world. But, I don't	think we'll actually do that.

						See the readme for commentary on code style

     Platform:    Visio Studio.Net 2003/2005

*************************************************************************/
#pragma once

#include <fstream>
#include <iostream>
#include <utility>
#include <vector>
#include <map>
#include "ControlPoint.h"

#define DIVIDE_LINE 100

using std::vector; // avoid having to say std::vector all of the time
using std::pair;
using std::map;

// make use of other data structures from this project

enum CurveType {
	Linear,
	Cardinal,
	Cubic
};

enum TrackType {
	Line,
	Track,
	Road,
};

class Path;

class CTrack {
public:		
		// Constructor
		CTrack();

public:
		// when we want to clear the control points, we really "reset" them 
		// to have 4 default positions (since we should never have fewer
		// than 4 points)
		void resetPoints();


		// read and write to files
		void readPoints(const char* filename);
		void writePoints(const char* filename);

		void Draw(bool doingShadows);

		void BuildTrack();
public:
		// rather than have generic objects, we make a special case for these few
		// objects that we know that all implementations are going to need and that
		// we're going to have to handle specially
		vector<ControlPoint> points;
		vector<Path> paths;

		//###################################################################
		// TODO: you might want to do this differently
		//###################################################################
		// the state of the train - basically, all I need to remember is where
		// it is in parameter space
		float trainU;
		int curve;
		int track;
		int pathN;
};

struct MapComp {
	bool operator() (const pair<int, int>& a, const pair<int, int>& b) const {
		if (a.first < b.first) return true;
		if (a.first > b.first) return false;
		return a.second < b.second;
	}
};

class Path {
public:
	ControlPoint p0, p1;
	map<pair<int, int>, vector<ControlPoint>, MapComp> points;
};
/************************************************************************
     File:        Track.cpp

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

#include "Track.h"
#include <windows.h>
#include <GL/gl.h>
#include <queue>
using namespace std;

//****************************************************************************
//
// * Constructor
//============================================================================
CTrack::
CTrack() : trainU(0)
//============================================================================
{
	resetPoints();
}

//****************************************************************************
//
// * provide a default set of points
//============================================================================
void CTrack::
resetPoints()
//============================================================================
{

	points.clear();
	points.push_back(ControlPoint(Pnt3f(50,5,0)));
	points.push_back(ControlPoint(Pnt3f(0,5,50)));
	points.push_back(ControlPoint(Pnt3f(-50,5,0)));
	points.push_back(ControlPoint(Pnt3f(0,5,-50)));

	// we had better put the train back at the start of the track...
	trainU = 0.0;
}

//****************************************************************************
//
// * Handy utility to break a string into a list of words
//============================================================================
void breakString(char* str, std::vector<const char*>& words) 
//============================================================================
{
	// start with no words
	words.clear();

	// scan through the string, starting at the beginning
	char* p = str;

	// stop when we hit the end of the string
	while(*p) {
		// skip over leading whitespace - stop at the first character or end of string
		while (*p && *p<=' ') p++;

		// now we're pointing at the first thing after the spaces
		// make sure its not a comment, and that we're not at the end of the string
		// (that's actually the same thing)
		if (! (*p) || *p == '#')
		break;

		// so we're pointing at a word! add it to the word list
		words.push_back(p);

		// now find the end of the word
		while(*p > ' ') p++;	// stop at space or end of string

		// if its ethe end of the string, we're done
		if (! *p) break;

		// otherwise, turn this space into and end of string (to end the word)
		// and keep going
		*p = 0;
		p++;
	}
}

//****************************************************************************
//
// * The file format is simple
//   first line: an integer with the number of control points
//	  other lines: one line per control point
//   either 3 (X,Y,Z) numbers on the line, or 6 numbers (X,Y,Z, orientation)
//============================================================================
void CTrack::
readPoints(const char* filename)
//============================================================================
{
	ifstream fs = ifstream(filename);
	if (!fs.is_open()) {
		printf("Can't Open File!\n");
	} 
	else {
		int n;
		fs >> n;

		if((n < 4)) {
			printf("Illegal Number of Points Specified in File");
		} else {
			points.clear();
			// get lines until EOF or we have enough points
			for (int i = 0; i < n; i++) {
				ControlPoint p;
				double x, y, z, ox, oy, oz;
				fs >> p.pos.x >> p.pos.y >> p.pos.z >> p.orient.x >> p.orient.y >> p.orient.z;
				p.orient.normalize();
				points.push_back(p);
			}
		}
		fs >> pathN;
		if (pathN < 4) {
			cout << "error" << endl;
		}
		else {
			for (int i = 0; i < pathN; i++) {
				int parent, child;
				fs >> parent >> child;
				points[parent].children.insert(child);
				points[child].parents.insert(parent);
			}
		}
		fs.close();
	}
	trainU = 0;
	BuildTrack();
}

//****************************************************************************
//
// * write the control points to our simple format
//============================================================================
void CTrack::
writePoints(const char* filename)
//============================================================================
{
	ofstream fs = ofstream(filename);
	if (!fs.is_open()) {
		printf("Can't open file for writing");
	} else {
		fs << points.size() << endl;
		for (const auto& p : points)
			fs << p.pos.x << " " << p.pos.y << " " << p.pos.z << " " << p.orient.x << " " << p.orient.y << " " << p.orient.z << endl;
		fs << pathN;
		for (int i = 0; i < points.size(); i++)
			for (const auto& c : points[i].children)
				fs << i << " " << c << endl;
		fs.close();
	}
}

void calParam(const ControlPoint& p0, const ControlPoint& p1, const ControlPoint& p2, const ControlPoint& p3, 
							int curve, ControlPoint& a, ControlPoint& b, ControlPoint& c, ControlPoint& d) {
	if (curve == Cardinal) {
		a.pos = -0.5 * p0.pos + 1.5 * p1.pos - 1.5 * p2.pos + 0.5 * p3.pos;
		b.pos =        p0.pos - 2.5 * p1.pos + 2   * p2.pos - 0.5 * p3.pos;
		c.pos = -0.5 * p0.pos                + 0.5 * p2.pos;
		d.pos =                       p1.pos;

		a.orient = -0.5 * p0.orient + 1.5 * p1.orient - 1.5 * p2.orient + 0.5 * p3.orient;
		b.orient =        p0.orient - 2.5 * p1.orient + 2   * p2.orient - 0.5 * p3.orient;
		c.orient = -0.5 * p0.orient                   + 0.5 * p2.orient;
		d.orient =                          p1.orient;
	} else {
		a.pos = -1 / 6.0 * p0.pos + 0.5     * p1.pos - 0.5     * p2.pos + 1 / 6.0 * p3.pos;
		b.pos =  0.5     * p0.pos -           p1.pos + 0.5     * p2.pos;
		c.pos = -0.5     * p0.pos +                  + 0.5     * p2.pos;
		d.pos =  1 / 6.0 * p0.pos + 2 / 3.0 * p1.pos + 1 / 6.0 * p2.pos;

		a.orient = -1 / 6.0 * p0.orient + 0.5     * p1.orient - 0.5     * p2.orient + 1 / 6.0 * p3.orient;
		b.orient =  0.5     * p0.orient -           p1.orient + 0.5     * p2.orient;
		c.orient = -0.5     * p0.orient +                     + 0.5     * p2.orient;
		d.orient =  1 / 6.0 * p0.orient + 2 / 3.0 * p1.orient + 1 / 6.0 * p2.orient;
	}
}

void CTrack::BuildTrack() {
	vertices.clear();
	for (auto& p : points) {
		p.visited = false;
		p.visIter = p.children.begin();
	}
	int start;
	queue<int> q;
	for (int i = 0; i < points.size(); i++) {
		if (points[i].visited)
			continue;
		q.push(i);
		while (!q.empty()) {
			int idx = q.front();
			points[idx].visited = true;
			q.pop();
			if (curve == Linear) {
				for (const auto& child : points[idx].children) {
					if (!points[child].visited)
						q.push(child);

				}
			}
		}
	}
}

void CTrack::draw(bool doingShadows) {
	if (!doingShadows) {
		glColor3d(1, 1, 1);
	}
	glLineWidth(4);

	vector<ControlPoint> tmpPoints = points;
	if (curve == Linear) {
		tmpPoints.push_back(points.front());
	} else {
		tmpPoints.push_back(points[0]);
		tmpPoints.push_back(points[1]);
		tmpPoints.insert(tmpPoints.begin(), points.back());
	}

	double percent = 1.0 / DIVIDE_LINE;
	double t;
	int idx;
	vector<ControlPoint> pList;
	ControlPoint qt;
	ControlPoint a, b, c, d;
	for (int i = 0; i < points.size(); i++) {
		idx = i + (curve != Linear);
		t   = 0;

		if (curve != Linear) {
			calParam(tmpPoints[idx - 1], tmpPoints[idx], tmpPoints[idx + 1], tmpPoints[idx + 2], curve, 
					 a, b, c, d);
		}

		for (int j = 0; j < DIVIDE_LINE; j++, t += percent) {
			if (curve == Linear) {
				qt.pos    = (1 - t) * tmpPoints[idx].pos    + t * tmpPoints[idx + 1].pos;
				qt.orient = (1 - t) * tmpPoints[idx].orient + t * tmpPoints[idx + 1].orient;
			} else {
				qt.pos    = pow(t, 3) * a.pos    + pow(t, 2) * b.pos    + t * c.pos    + d.pos;
				qt.orient = pow(t, 3) * a.orient + pow(t, 2) * b.orient + t * c.orient + d.orient;
			}
			pList.push_back(qt);
		}
	}

	Pnt3f w;
	Pnt3f pnt;
	ControlPoint prev = pList.back();
	glBegin(GL_LINE_LOOP);
	for (const auto& p : pList) {
		w = Pnt3f::CrossProduct(p.pos - prev.pos, prev.orient);
		w.normalize();
		w = w * 2;
		pnt = (p.pos + w);
		glVertex3d(pnt.x, pnt.y, pnt.z);
		prev = p;
	}
	glEnd();

	prev = pList.back();
	glBegin(GL_LINE_LOOP);
	for (const auto& p : pList) {
		w = Pnt3f::CrossProduct(p.pos - prev.pos, prev.orient);
		w.normalize();
		w = w * 2;
		pnt = (p.pos - w);
		glVertex3d(pnt.x, pnt.y, pnt.z);
		prev = p;
	}
	glEnd();
	glLineWidth(4);
}
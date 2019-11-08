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
		fs >> n;
		if (n < 4) {
			cout << "error" << endl;
		}
		else {
			for (int i = 0; i < n; i++) {
				int parent, child;
				fs >> parent >> child;
				points[parent].children.insert(child);
				points[child].parents.insert(parent);
			}
		}
		fs.close();
	}
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
		fs << paths.size() << endl;
		for (int i = 0; i < points.size(); i++)
			for (const auto& c : points[i].children)
				fs << i << " " << c << endl;
		fs.close();
	}
}

void calParam(const ControlPoint& p0, const ControlPoint& p1, const ControlPoint& p2, const ControlPoint& p3, PathData& pd) {
	if (PathData::type == Cardinal) {
		pd.a.pos = -0.5 * p0.pos + 1.5 * p1.pos - 1.5 * p2.pos + 0.5 * p3.pos;
		pd.b.pos =        p0.pos - 2.5 * p1.pos + 2   * p2.pos - 0.5 * p3.pos;
		pd.c.pos = -0.5 * p0.pos                + 0.5 * p2.pos;
		pd.d.pos =                       p1.pos;

		pd.a.orient = -0.5 * p0.orient + 1.5 * p1.orient - 1.5 * p2.orient + 0.5 * p3.orient;
		pd.b.orient =        p0.orient - 2.5 * p1.orient + 2   * p2.orient - 0.5 * p3.orient;
		pd.c.orient = -0.5 * p0.orient                   + 0.5 * p2.orient;
		pd.d.orient =                          p1.orient;
	} else {
		pd.a.pos = -1 / 6.0 * p0.pos + 0.5     * p1.pos - 0.5     * p2.pos + 1 / 6.0 * p3.pos;
		pd.b.pos =  0.5     * p0.pos -           p1.pos + 0.5     * p2.pos;
		pd.c.pos = -0.5     * p0.pos +                  + 0.5     * p2.pos;
		pd.d.pos =  1 / 6.0 * p0.pos + 2 / 3.0 * p1.pos + 1 / 6.0 * p2.pos;

		pd.a.orient = -1 / 6.0 * p0.orient + 0.5     * p1.orient - 0.5     * p2.orient + 1 / 6.0 * p3.orient;
		pd.b.orient =  0.5     * p0.orient -           p1.orient + 0.5     * p2.orient;
		pd.c.orient = -0.5     * p0.orient +                     + 0.5     * p2.orient;
		pd.d.orient =  1 / 6.0 * p0.orient + 2 / 3.0 * p1.orient + 1 / 6.0 * p2.orient;
	}
}

void CTrack::BuildTrack() {
	paths.clear();
	for (auto& p : points) {
		p.visited = false;
	}

	double percent = 1.0 / DIVIDE_LINE;
	double t;
	int start;
	queue<int> q;
	for (int i = 0; i < points.size(); i++) {
		if (points[i].visited)
			continue;
		q.push(i);
		start = i;
		while (!q.empty()) {
			int idx = q.front();
			q.pop();

			if (points[idx].visited)
				continue;

			points[idx].visited = true;
			for (const auto& child : points[idx].children) {
				if (!points[child].visited)
					q.push(child);

				ControlPoint p0, p1, p2, p3;
				p1 = points[idx];
				p2 = points[child];

				set<int> p0Set;
				set<int> p3Set;
				if (p1.parents.size())
					p0Set = p1.parents;
				else
					p0Set.insert(start);
				if (p2.children.size())
					p3Set = p2.children;
				else
					p3Set.insert(start);
				
				Path path;
				bool isFirst = true;
				for (const auto& p0Id : p1.parents) {
					p0 = points[p0Id];
					for (const auto& p3Id : p2.children) {
						if (PathData::type == Linear && !isFirst) {
							path[pair<int, int>(p0Id, p3Id)] = path.begin()->second;
							continue;
						}

						PathData pd;
						pd.p0 = p0Id;
						pd.p1 = idx;
						pd.p2 = child;
						pd.p3 = p3Id;

						p3 = points[p3Id];
						if (PathData::type != Linear) {
							calParam(p0, p1, p2, p3, pd);
						}
						else {
							pd.a = p1;
							pd.b = p2;
						}

						t = 0;
						double lenth = 0;
						for (int j = 0; j <= DIVIDE_LINE; j++, t += percent) {
							pd.pointSet.push_back(pd.CalInterpolation(t));
							if (j > 0) {
								lenth += (pd.pointSet[j].pos - pd.pointSet[j - 1].pos).Lenth();
							}
						}
						pd.length = lenth;

						path[pair<int, int>(p0Id, p3Id)]  = pd;
						paths[pair<int, int>(idx, child)] = path;
					}
				}
			}
		}
	}
}

void drawLine(const vector<ControlPoint>& pointSet, int side) {
	Pnt3f w;
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < pointSet.size() - 1; i++) {
		w = Pnt3f::CrossProduct(pointSet[i + 1].pos - pointSet[i].pos, pointSet[i].orient);
		w.normalize();
		w = w * 2.5;
		glVertex3dv((pointSet[i].pos + side * w).v());
	}
	glVertex3dv((pointSet.back().pos + side * w).v());
	glEnd();
}

void drawTrack(const vector<ControlPoint>& pointSet) {
	Pnt3f w, v;
	Pnt3f p;
	glBegin(GL_QUADS);
	for (int i = 0; i < pointSet.size() - 1; i += 5) {
		v = pointSet[i + 1].pos - pointSet[i].pos;
		v.normalize();
		w = Pnt3f::CrossProduct(v, pointSet[i].orient);
		w.normalize();
		w = w * 5;
		p = pointSet[i].pos - pointSet[i].orient;
		glVertex3dv((p + w + v).v());
		glVertex3dv((p + w - v).v());
		glVertex3dv((p - w - v).v());
		glVertex3dv((p - w + v).v());
	}
	glEnd();
}

void drawRoad(const vector<ControlPoint>& pointSet) {
	Pnt3f w;
	glBegin(GL_TRIANGLE_STRIP);
	for (int i = 0; i < pointSet.size() - 1; i++) {
		w = Pnt3f::CrossProduct(pointSet[i + 1].pos - pointSet[i].pos, pointSet[i].orient);
		w.normalize();
		w = w * 2.4;
		glVertex3dv((pointSet[i].pos + w).v());
		glVertex3dv((pointSet[i].pos - w).v());
	}
	glVertex3dv((pointSet.back().pos + w).v());
	glVertex3dv((pointSet.back().pos - w).v());
	glEnd();
}

void CTrack::Draw(bool doingShadows) {
	glLineWidth(4);

	for (const auto& path : paths) {
		for (const auto& pathData : path.second) {
			if (!doingShadows) {
				glColor3d(0.22, 0.18, 0.04);
			}
			drawLine(pathData.second.pointSet, 1);
			drawLine(pathData.second.pointSet, -1);
			if (!doingShadows) {
				glColor3d(0.49, 0.38, 0.11);
			}
			if (track == Track)
				drawTrack(pathData.second.pointSet);
			else if (track == Road)
				drawRoad(pathData.second.pointSet);
		}
	}

	glLineWidth(1);
}

void CTrack::SetCurve(CurveType type) {
	PathData::type = type;
	BuildTrack();
}

void CTrack::AddPoint(const ControlPoint& p) {
	points.push_back(p);
}

void CTrack::RemovePoint(int index) {
	ControlPoint p = points[index];
	set<int>::iterator child  = p.children.begin();
	set<int>::iterator parent = p.parents.begin();
	int lastChild, lastParent;
	for (; child != p.children.end() && parent != p.parents.end(); ++child, ++parent) {
		points[*child].parents.insert(*parent);
		points[*parent].children.insert(*child);
		lastChild  = *child;
		lastParent = *parent;
	}
	if (child != p.children.end()) {
		for (; child != p.children.end(); ++child) {
			points[lastParent].children.insert(*child);
		}
	} else {
		for (; parent != p.parents.end(); ++parent) {
			points[lastChild].parents.insert(*parent);
		}
	}
	BuildTrack();
}

void CTrack::AddPath(int p1, int p2) {
	points[p1].children.insert(p2);
	points[p2].parents.insert(p1);
	BuildTrack();
}

void CTrack::RemovePath(int p1, int p2) {
	points[p1].children.erase(p2);
	points[p2].parents.erase(p1);
	BuildTrack();
}

PathData CTrack::GetRandomPath() {
	auto it1 = paths.begin();
	advance(it1, rand() % paths.size());
	Path nextPath = it1->second;
	auto it2 = nextPath.begin();
	advance(it2, rand() % nextPath.size());
	return it2->second;
}

PathData CTrack::GetNextPath(const PathData& curr) {
	set<int> children = points[curr.p3].children;
	Path nextPath = paths[pair<int, int>(curr.p2, curr.p3)];
	set<int>::iterator it = children.begin();
	advance(it, rand() % children.size());
	int p3 = *it;
	return nextPath[pair<int, int>(curr.p1, p3)];
}
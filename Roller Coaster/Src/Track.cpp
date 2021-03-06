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

Terrain* CTrack::terrain;

//****************************************************************************
//
// * Constructor
//============================================================================
CTrack::CTrack()
//============================================================================
{
	// resetPoints();
}

//****************************************************************************
//
// * provide a default set of points
//============================================================================
void CTrack::resetPoints()
//============================================================================
{

	points.clear();
	pointCount = 0;
	//points[pointCount++] = new ControlPoint(Pnt3f(50, 5, 0));
	//points[pointCount++] = new ControlPoint(Pnt3f(0, 5, 50));
	//points[pointCount++] = new ControlPoint(Pnt3f(-50, 5, 0));
	//points[pointCount++] = new ControlPoint(Pnt3f(0, 5, -50));

	// we had better put the train back at the start of the track...
}

//****************************************************************************
//
// * The file format is simple
//   first line: an integer with the number of control points
//	  other lines: one line per control point
//   either 3 (X,Y,Z) numbers on the line, or 6 numbers (X,Y,Z, orientation)
//============================================================================
void CTrack::readPoints(const char* filename)
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
			pointCount = 0;
			// get lines until EOF or we have enough points
			for (int i = 0; i < n; i++) {
				ControlPoint *p = new ControlPoint();
				CtrlPoint center;
				double x, y, z, ox, oy, oz;
				fs >> center.pos.x >> center.pos.y >> center.pos.z >> center.orient.x >> center.orient.y >> center.orient.z;
				center.pos.y += terrain->getHeightOfTerrain(center.pos.x, center.pos.z);
				center.orient.normalize();
				p->center = center;
				points[pointCount++] = p;
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
				auto it1 = points.begin();
				advance(it1, parent);
				auto it2 = points.begin();
				advance(it2, child);
				points[it1->first]->children.insert(it2->first);
				points[it2->first]->parents.insert(it1->first);
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
void CTrack::writePoints(const char* filename)
//============================================================================
{
	ofstream fs = ofstream(filename);
	if (!fs.is_open()) {
		printf("Can't open file for writing");
	} else {
		fs << points.size() << endl;
		for (const auto& v : points) {
			CtrlPoint p = v.second->center;
			fs << p.pos.x << " " << p.pos.y - terrain->getHeightOfTerrain(p.pos.x, p.pos.z) << " " << p.pos.z << " " << p.orient.x << " " << p.orient.y << " " << p.orient.z << endl;
		}
		fs << paths.size() << endl;
		for (auto p : points)
			for (const auto& c : p.second->children)
				fs << p.first << " " << c << endl;
		fs.close();
	}
}

void calParam(const CtrlPoint& p0, const CtrlPoint& p1, const CtrlPoint& p2, const CtrlPoint& p3, PathData& pd) {
	if (PathData::curve == Cardinal) {
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

void subdivision(PathData &pd, CtrlPoint p1, CtrlPoint p2) {
	double t = (p1.inter + p2.inter) / 2;
	CtrlPoint mid = pd.CalInterpolation(t);
	Pnt3f pos = (p1.pos + p2.pos) * 0.5;
	Pnt3f orient = (p1.orient + p2.orient) * 0.5;
	if ((pos - mid.pos).Lenth() > 0.05 || (orient - mid.orient).Lenth() > 0.05) {
		subdivision(pd, p1, mid);
		pd.pointSet.push_back(mid);
		subdivision(pd, mid, p2);
	}
}

void CTrack::BuildTrack() {
	paths.clear();
	for (auto& p : points) {
		p.second->visited = false;
	}

	int start;
	queue<int> q;
	for (auto it = points.begin(); it != points.end(); it++) {
		if (points[it->first]->visited)
			continue;
		q.push(it->first);
		start = it->first;
		while (!q.empty()) {
			int idx = q.front();
			q.pop();

			if (points[idx]->visited)
				continue;

			points[idx]->visited = true;
			for (const auto& child : points[idx]->children) {
				if (!points[child]->visited)
					q.push(child);

				ControlPoint *p0, *p1, *p2, *p3;
				p1 = points[idx];
				p2 = points[child];

				set<int> p0Set;
				set<int> p3Set;
				if (p1->parents.empty())
					p0Set.insert(start);
				else
					p0Set = p1->parents;
				if (p2->children.empty())
					p3Set.insert(start);
				else
					p3Set = p2->children;
				
				Path path;
				bool isFirst = true;
				for (const auto& p0Id : p0Set) {
					p0 = points[p0Id];
					for (const auto& p3Id : p3Set) {
						pair<int, int> key = pair<int, int>(p0Id, p3Id);
						if (PathData::curve == Linear && !isFirst) {
							path[key] = path.begin()->second;
							path[key].p0 = p0Id;
							path[key].p3 = p3Id;
							continue;
						}
						isFirst = false;

						PathData pd;
						pd.p0    = p0Id;
						pd.p1    = idx;
						pd.p2    = child;
						pd.p3    = p3Id;

						p3 = points[p3Id];
						if (PathData::curve != Linear) {
							calParam(p0->center, p1->center, p2->center, p3->center, pd);
						}
						else {
							pd.a = p1->center;
							pd.b = p2->center;
						}

						CtrlPoint p1, p2;
						p1 = pd.CalInterpolation(0);
						p2 = pd.CalInterpolation(1);
						pd.pointSet.push_back(p1);
						subdivision(pd, p1, p2);
						pd.pointSet.push_back(p2);

						pd.length = 0;
						for (int i = 1; i < pd.pointSet.size(); i++) {
							pd.length += (pd.pointSet[i - 1].pos - pd.pointSet[i].pos).Lenth();
						}
						path[key] = pd;
					}
				}
				paths[pair<int, int>(idx, child)] = path;
			}
		}
	}
}

void CTrack::Draw(bool doingShadows, int selectedPath) {
	glLineWidth(4);

	int i = 0;
	for (auto& path : paths) {
		for (auto& pathData : path.second) {
			pathData.second.Draw(doingShadows, i == selectedPath);
		}
		i++;
	}

	glLineWidth(1);
}

void CTrack::SetCurve(CurveType type) {
	PathData::curve = type;
	BuildTrack();
}

void CTrack::AddPoint(ControlPoint* p) {
	points[pointCount++] = p;
}

void CTrack::RemovePoint(int index) {
	ControlPoint *p = points[index];
	auto child  = p->children.begin();
	auto parent = p->parents.begin();

	int lastChild, lastParent;
	if (child != p->children.end())
		lastChild = *child;
	else
		lastChild = -1;
	if (parent != p->parents.end())
		lastParent = *parent;
	else
		lastParent = -1;

	for (auto point : points) {
		point.second->parents.erase(index);
		point.second->children.erase(index);
	}

	for (; child != p->children.end() && parent != p->parents.end(); ++child, ++parent) {
		points[*child]->parents.insert(*parent);
		points[*parent]->children.insert(*child);
		lastChild  = *child;
		lastParent = *parent;
	}
	if (child != p->children.end() && lastParent != -1) {
		for (; child != p->children.end(); ++child) {
			points[lastParent]->children.insert(*child);
		}
	} else if (parent != p->parents.end() && lastChild != -1) {
		for (; parent != p->parents.end(); ++parent) {
			points[lastChild]->parents.insert(*parent);
		}
	}

	points.erase(index);

	BuildTrack();
}

void CTrack::AddPath(int p1, int p2) {
	points[p1]->children.insert(p2);
	points[p2]->parents.insert(p1);
	BuildTrack();
}

void CTrack::RemovePath(int p1, int p2) {
	points[p1]->children.erase(p2);
	points[p2]->parents.erase(p1);
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
	pair<int, int> key(curr.p2, curr.p3);

	if (paths.find(key) == paths.end())
		return GetRandomPath();

	Path nextPath = paths[key];
	int p3;
	auto it = nextPath.begin();
	advance(it, rand() % nextPath.size());
	p3 = it->first.second;

	key = pair<int, int>(curr.p1, p3);
	if (nextPath.find(key) == nextPath.end())
		return GetRandomPath();
	return nextPath[key];
}

PathData CTrack::GetPrevPath(const PathData& curr) {
	pair<int, int> key(curr.p0, curr.p1);
	if (paths.find(key) == paths.end()) {
		return GetRandomPath();
	}

	Path prevPath = paths[key];
	int p0;
	auto it = prevPath.begin();
	advance(it, rand() % prevPath.size());
	p0 = it->first.first;

	key = pair<int, int>(p0, curr.p2);
	if (prevPath.find(key) == prevPath.end())
		return GetRandomPath();
	return prevPath[key];
}

PathData CTrack::GetPath(int& p0, int& p1, int& p2, int& p3) {
	pair<int, int> key(p1, p2);
	pair<int, int> key2(p0, p3);
	if (paths.find(key) == paths.end() || paths[key].find(key2) == paths[key].end()) {
		PathData pd = GetRandomPath();
		p0 = pd.p0;
		p1 = pd.p1;
		p2 = pd.p2;
		p3 = pd.p3;
		return pd;
	}
	return paths[key][key2];
}
#include "L1voronoi.h"

void L1_Bisector::ConstructVertice() {
	for (int i = n_vertice - 1; i >= 0;i--) {
		std::pair<int, int> p = vertex_seq[Case][Subcase][i];
		this->vertice->push_back(new Point(values[p.first], values[p.second]));
	}
}
void L1_Bisector::DetermineCase(double p_x, double p_y, double q_x, double q_y, double dx, double dy) {
	//Case 0 : they lie on the vertical line
	if (abs(p_x - q_x) < 1e-6) {
		Case = 0;
		n_vertice = 4;
		if (p_y > q_y) Subcase = 0;
		else Subcase = 1;
	}

	//Case 1 : they lie on the horizontal line
	else if (abs(p_y - q_y) < 1e-6) {
		Case = 1;
		n_vertice = 4;
		if (p_x > q_x) Subcase = 0;
		else Subcase = 1;
	}

	//Case 2 : they are the diagonal points of regular structure
	else if (dx == dy) {
		Case = 2;
		n_vertice = 7;
		if (p_x < q_x && p_y < q_y) Subcase = 0;
		if (p_x < q_x && p_y > q_y) Subcase = 1;
		if (p_x > q_x&& p_y < q_y) Subcase = 2;
		if (p_x > q_x&& p_y > q_y) Subcase = 3;
	}
	//CASE 3 : they are the diagonal points of rectangular
	else {
		Case = 3;
		n_vertice = 6;
		if (dx > dy) {
			if (p_x < q_x && p_y < q_y) Subcase = 0;
			if (p_x < q_x && p_y > q_y) Subcase = 1;
			if (p_x > q_x&& p_y < q_y) Subcase = 2;
			if (p_x > q_x&& p_y > q_y) Subcase = 3;
		}
		else {
			if (p_x < q_x && p_y < q_y) Subcase = 4;
			if (p_x < q_x && p_y > q_y) Subcase = 5;
			if (p_x > q_x&& p_y < q_y) Subcase = 6;
			if (p_x > q_x&& p_y > q_y) Subcase = 7;
		}

	}
}

L1_Bisector::L1_Bisector(Point& p, Point& q,double minX, double maxX, double minY, double maxY) {
	double p_x = p.getx(), q_x = q.getx(), p_y = p.gety(), q_y = q.gety();
	values[P_X] = p_x, values[P_Y] = p_y;
	values[Q_X] = q_x, values[Q_Y] = q_y;
	double dx = abs(p_x - q_x), dy = abs(p_y - q_y);
	double L = (dx + dy) / 2;
	values[MP_X] = ((dx - L) * q_x + L * p_x) / dx;
	values[MQ_X] = ((dx - L) * p_x + L * q_x) / dx;
	values[MP_Y] = ((dy - L) * q_y + L * p_y) / dy;
	values[MQ_Y] = ((dy - L) * p_y + L * q_y) / dy;
	values[MIN_X] = minX, values[MAX_X] = maxX;
	values[MIN_Y] = minY, values[MAX_Y] = maxY;
	vertice = new std::vector<Point>;
	this->DetermineCase(p_x, p_y, q_x, q_y, dx, dy);
	this->ConstructVertice();
}
L1_Bisector::~L1_Bisector() {
	delete vertice;
}
std::vector<Point> L1_Bisector::getVertice() {
	return std::vector<Point>(*(this->vertice));
}

SimplePolygon L1_Bisector::getPolygon() {
	return SimplePolygon(*(this->vertice));
}

L1_voronoi::L1_voronoi(std::vector<Point>& vertices) {
	n_site = vertices.size();
	maxX = minX = vertices[0].getx();
	maxY = minY = vertices[0].gety();

	for (int i = 1; i < n_site; i++) {
		minX = minX > vertices[i].getx() ? vertices[i].getx() : minX;
		maxX = maxX < vertices[i].getx() ? vertices[i].getx() : maxX;
		minY = minY > vertices[i].gety() ? vertices[i].gety() : minY;
		maxY = maxY < vertices[i].gety() ? vertices[i].gety() : maxY;
	}
	minX -= 50., maxX += 50., minY -= 50., maxY += 50.;

	std::vector<Point> B_pts = { Point(minX,minY),Point(minX,maxY) ,Point(maxX,maxY) ,Point(maxX,minY) };
	SimplePolygon BoundingBox(B_pts);
	PolygonClipping clipper;
	
	for (int i = 0; i < n_site;i++) {
		SimplePolygon cell = BoundingBox;
		for (int j = 0; j < n_site;j++) {
			if (i != j) {
				L1_Bisector Bisector(vertices[i], vertices[j],minX,maxX,minY,maxY);
				SimplePolygon op = Bisector.getPolygon();

				std::vector<SimplePolygon*> ret = clipper.clipping(cell, op);
				if (!ret.empty()) cell = *(ret.front()); 
			}
		}
		__Cell element(vertices[i], cell);
		diagram.push_back(element);
	}
}

std::vector<Edge> L1_voronoi::getEdges() {
	std::vector<Edge> ret;

	for (int i = 0; i < n_site;i++) {
		std::vector<Edge> edges = diagram[i].cell.getEdges();
		ret.insert(ret.end(), edges.begin(), edges.end());
	}
	std::sort(ret.begin(), ret.end());
	ret.erase(std::unique(ret.begin(), ret.end()), ret.end());

	return ret;
}


L1_voronoi::~L1_voronoi(){}


#pragma once
#include "Shape.h"
#include "Triangle.h"
#include <vector>

class Polygon : public Shape {
public:

	void AddTriangle(Triangle tri)
	{
		triangles.push_back(tri);
	}

	void move(float x, float y, float z)
	{
		for (int i = 0; i < triangles.size(); i++)
		{
			triangles[i].move(x, y, z);
		}
	}
	
	std::vector<Triangle> triangles;
private:

};
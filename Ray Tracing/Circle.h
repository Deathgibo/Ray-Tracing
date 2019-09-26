#pragma once
#include "Shape.h"

class Circle : public Shape {
public:
	Circle(float x, float y, float z, float radiust, float r, float g, float b, int reflect = 0) : Shape(r, g, b, reflect)
	{
		position.x = x; position.y = y; position.z = z;
		radius = radiust;
	}
	void move(float x, float y, float z)
	{
		position.x += x; position.y += y; position.z += z;
	}
	bool checkcollision(Ray r)
	{
		//computer discriminant if negative it doesnt touch,  if 0 grazes it at one point, if positive passes it at two points on it way in and out
		vector3 e, d, c;
		e = r.getposition();
		d = r.getdirection();
		c = position;

		//discriminant
		//cout << e.z << " after ";
		float disc = (((d*(e - c))*(d*(e - c)))) - (((d*d) * ((e - c)*(e - c) - (radius*radius))));
		//std::cout << disc << std::endl;
		//cout << disc << endl;
		//cout << e.z << endl;
		if (disc < 0)
		{
			return false;
		}
		if (disc == 0)
		{
			std::cout << "skimmed";
		}
		if (disc == 0)
		{
			ts = ((-d * (e - c))) / (d*d);
			if (ts < 0)
			{
				return false;
			}
			return true;
		}
		float t1, t2;
		t1 = ((-d * (e - c)) + sqrt(disc)) / (d*d);
		t2 = ((-d * (e - c)) - sqrt(disc)) / (d*d);
		//std::cout << t1 << " " << t2 << std::endl;
		//std::cout << r.getdirection().z << std::endl;
		//std::cout << (r.getposition() + r.getdirection()*t1).z << std::endl;
		if (t1 >= 0)
		{
			ts = t1;

			if (t2 < ts && t2 >= 0)
			{
				ts = t2;
			}
			return true;
		}
		else
		{
			ts = t2;
			return true;
		}
	}

	vector3 getcollisionpoint(Ray r)
	{
		return (r.getposition() + r.getdirection()*ts);
	}

	vector3 calculatenormal(Ray r)
	{
		vector3 n = ((position) - (r.getposition() + r.getdirection()*ts)) *2;
		//std::cout << (r.getposition() + r.getdirection()*ts).z << std::endl;
		//std::cout << n.z << std::endl;
		return n;
	}

	vector3 getposition() 
	{
		vector3 tmp(0, 0, 0);
		return tmp;
	}

	vector3 position;
	float radius;
private:

};

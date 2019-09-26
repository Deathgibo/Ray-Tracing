#pragma once
#include "Shape.h"

class Triangle : public Shape {
public:
	Triangle(float thex1, float they1, float thez1, float thex2, float they2, float thez2, float thex3, float they3, float thez3, float r, float g, float b, int reflect = 0) : Shape(r,g,b, reflect)
	{
		one.x = thex1; one.y = they1; one.z = thez1;
		two.x = thex2; two.y = they2; two.z = thez2;
		three.x = thex3; three.y = they3; three.z = thez3;
	}
	void move(float x, float y, float z)
	{
		one.x += x; two.x += x; three.x += x;
		one.y += y; two.y += y; three.y += y;
		one.z += z; two.z += z; three.z += z;
	}
	bool checkcollision(Ray r)
	{
		//compute scalar on 3rd tri vertex (y)
		//std::cout << r.getdirection().z << std::endl;
		float tt = raytrihelper(2, r);
		//std::cout << tt << std::endl;
		if (tt < 0 || tt > 10000)
		{
			return false;
		}
		ts = tt;
		float y = raytrihelper(1, r);
		//cout << y << endl;
		if (y < 0 || y > 1)
		{
			return false;
		}

		//computer scalar on 2nd tri vertex(b)
		float b = raytrihelper(0, r);

		if (b < 0 || b > 1 - y)
		{
			return false;
		}

		return true;
	}
	
	float raytrihelper(int y, Ray r)
	{
		float a, b, c, d, e, f, g, h, i, j, k, l;
		a = one.x - two.x;
		b = one.y - two.y;
		c = one.z - two.z;
		d = one.x - three.x;
		e = one.y - three.y;
		f = one.z - three.z;
		g = r.getdirection().x;
		h = r.getdirection().y;
		i = r.getdirection().z;
		j = one.x - r.getposition().x;
		k = one.y - r.getposition().y;
		l = one.z - r.getposition().z;
		//regular determinent of raypointposition = trianglepositions
		float m = a * (e*i - h * f) + b * (g*f - d * i) + c * (d*h - e * g);
		float they;
		if (y == 1)
		{
			they = i * (a*k - j * b) + h * (j*c - a * l) + g * (b*l - k * c);
		}
		else if (y == 0)
		{
			they = j * (e*i - h * f) + k * (g*f - d * i) + l * (d*h - e * g);
		}
		else
		{
			they = f * (a*k - j * b) + e * (j*c - a * l) + d * (b*l - k * c);
			they = -(they / m);
			return they;
		}
		they = (they / m);
		return they;
	}
	vector3 calculatenormal(Ray r)//clockwise
	{
		vector3 first = two - one;
		vector3 second = three - one;
		vector3 n = crossproduct(first, second);
		n.normalize();

		//std::cout << n.x << " " << n.y << " " << n.z << std::endl;
		return n;
	}

	vector3 getcollisionpoint(Ray r)
	{
		return (r.getposition() + r.getdirection()*ts);
	}

	vector3 crossproduct(vector3 one, vector3 two)
	{
		vector3 p;
		p.x = one.y * two.z - one.z * two.y;
		p.y = one.z * two.x - one.x * two.z;
		p.z = one.x * two.y - one.y * two.x;

		return p;
	}

	vector3 getposition() {
		return two;
	}

	vector3 one;
	vector3 two;
	vector3 three;

private:

};

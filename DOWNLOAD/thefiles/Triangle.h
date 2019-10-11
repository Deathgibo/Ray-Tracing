#pragma once
#include "Shape.h"
using namespace std;

class Triangle : public Shape {
public:
	Triangle(float thex1, float they1, float thez1, float thex2, float they2, float thez2, float thex3, float they3, float thez3, float r, float g, float b, int reflect = 0, int refract = 0, float ref = 1) : Shape(r,g,b, reflect, refract,ref)
	{
		one.x = thex1; one.y = they1; one.z = thez1;
		two.x = thex2; two.y = they2; two.z = thez2;
		three.x = thex3; three.y = they3; three.z = thez3;
		reversenormals = 0;
	}
	Triangle(float thex1, float they1, float thez1, float theu1, float thev1, float thex2, float they2, float thez2, float theu2, float thev2 , float thex3, float they3, float thez3, float theu3, float thev3, float r, float g, float b, int reflect = 0, int refract = 0, float ref = 1) : Shape(r, g, b, reflect, refract, ref)
	{
		one.x = thex1; one.y = they1; one.z = thez1; one.u = theu1; one.v = thev1;
		two.x = thex2; two.y = they2; two.z = thez2; two.u = theu2; two.v = thev2;
		three.x = thex3; three.y = they3; three.z = thez3; three.u = theu3; three.v = thev3;
		reversenormals = 0;
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
		if (reversenormals)
			return -n;
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

	vector3 calculatetexturecolor(Ray r)
	{
		if (!texture.texthere())
		{
			return color;
		}
		vector3 t(0, 0, 0,0,0);
		vector3 col = getcollisionpoint(r);
		float a, b, c, d, e, f, g, h, i;
		float x, y, z;
		y = 0;
		a = two.x - one.x; b = three.x - one.x; c = col.x - one.x;
		d = two.y - one.y; e = three.y - one.y; f = col.y - one.y;
		g = two.z - one.z; h = three.z - one.z; i = col.z - one.z;
		//cout << a << " " << b << " " << d << " " << e << " " << g << " " << h << endl;
		if (d == 0 && e == 0)
		{
			if((-a * h + b * g) != 0)
				y = (c*g - a * i) / (-a * h + b * g);
		}
		else
		{
			if ((-a * e + b * d) != 0)
				y = (c*d - a * f) / (-a * e + b * d);
		}
		if (y == 0)
		{
			if((-g * e + b * d)!= 0)
				y = (i*d - g * f) / (-g * e + b * d);
		}
		if (y == 0)
		{
			if((-g * b + h * a) != 0)
				y = (i*a - g* c) / (-g * b + h* a);
		}
		//cout << y << endl;
		if (d != 0)
			x = (f - e * y) / (d);
		else if (g != 0)
			x = (i - h * y) / (g);
		else if(a != 0)
			x = (c - b * y) / (a);
		else
		{
			std::cout << "error points one and two are equal" << std::endl;
			return t;
		}

		z = 1 - x - y;

		//if correct just do x*oneuv + y*twouv + z*threeuv
		float u = z * one.u + x * two.u + y * three.u;
		float v = z * one.v + x * two.v + y * three.v;

		//then send uv to the shapes texture function to return a color
		vector3 texelcolor = texture.GetTextureColor(u, v);

		vector3 onecolor(255, 0, 0);
		vector3 twocolor(0, 255, 0);
		vector3 threecolor(0, 0, 255);
		//texelcolor = onecolor * z + twocolor * x + threecolor * y;

		return texelcolor;
	}

	vector3 one;
	vector3 two;
	vector3 three;

private:

};

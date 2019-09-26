#pragma once
#include <cmath>

class vector3 {
public:

	vector3() { x = 0; y = 0; z = 0; }
	vector3(float thex, float they, float thez) { x = thex; y = they; z = thez; }

	vector3 operator*(float scalar)
	{
		vector3 tmp(x, y, z);
		tmp.x *= scalar;
		tmp.y *= scalar;
		tmp.z *= scalar;
		return tmp;
	}

	float operator*(vector3 other)
	{
		return (x*other.x) + (y*other.y) + (z*other.z);
	}

	vector3 operator+(const vector3 &other)
	{
		vector3 tmp(x, y, z);
		tmp.x += other.x;
		tmp.y += other.y;
		tmp.z += other.z;
		return tmp;
	}

	vector3 operator-(const vector3 &other)
	{
		vector3 tmp(x, y, z);
		tmp.x -= other.x;
		tmp.y -= other.y;
		tmp.z -= other.z;
		return tmp;
	}

	vector3 operator-()
	{
		vector3 tmp(x, y, z);
		tmp.x = -x;
		tmp.y = -y;
		tmp.z = -z;
		return tmp;
	}

	void normalize()
	{
		float length = sqrt(x*x + y * y + z * z);

		x /= length;
		y /= length;
		z /= length;
	}

	vector3 normalizereturn()
	{
		float length = sqrt(x*x + y * y + z * z);
		vector3 tmp(x, y, z);
		tmp.x /= length;
		tmp.y /= length;
		tmp.z /= length;
		return tmp;
	}

	void clampthis(float clampvalue)
	{
		if (x > clampvalue)
		{
			x = clampvalue;
		}
		if (y > clampvalue)
		{
			y = clampvalue;
		}
		if (z > clampvalue)
		{
			z = clampvalue;
		}
	}

	float x;
	float y;
	float z;
private:

};
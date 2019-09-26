#include "camera.h"
#include <stdio.h>

camera::camera()
{
	position.x = 0;
	position.y = 0;
	position.z = 0;
}

camera::camera(float x, float y, float z)
{
	position.x = x; position.y = y; position.z = z;

	worldup.x = 0.0; worldup.y = 1.0; worldup.z = 0.0;
	lookat.x = 0.0; lookat.y = 0.0; lookat.z = -1.0;
	lookat = normalize(lookat);
	right = normalize(crossproduct(lookat, worldup));
	up = normalize(crossproduct(right, lookat));
}

void camera::printinfo()
{
	printf("position: (%f, %f, %f)\n", position.x, position.y, position.z);
	printf("lookat: (%f, %f, %f)\n", lookat.x, lookat.y, lookat.z);
	printf("up: (%f, %f, %f)\n", up.x, up.y, up.z);
	printf("right: (%f, %f, %f)\n", right.x, right.y, right.z);
}


vector3 camera::crossproduct(vector3 one, vector3 two)
{
	vector3 p;
	p.x = one.y * two.z - one.z * two.y;
	p.y = one.z * two.x - one.x * two.z;
	p.z = one.x * two.y - one.y * two.x;

	return p;
}

vector3 camera::normalize(vector3 one)
{

	float length = sqrt(one.x*one.x + one.y*one.y + one.z*one.z);
	vector3 thereturn = one;

	thereturn.x /= length;
	thereturn.y /= length;
	thereturn.z /= length;

	return thereturn;
}
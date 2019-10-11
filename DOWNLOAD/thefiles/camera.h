#pragma once
#include "vector3.h"

class camera
{
public:
	camera();
	camera(float x, float y, float z);
	void printinfo();
	vector3 Getposition() { return position; }
	vector3 GetLookAt() { return lookat; }
	vector3 getup() { return up; }
	vector3 getright() { return right; }
	void updatecamera();
	void direction(float x, float y, float z) { position.x = x; position.y = y; position.z = z; }
	float toradians(float d);
	void setangles(float thepitch, float theyaw);
	void Move(float x, float y, float z);

private:
	vector3 position;

	vector3 lookat;
	vector3 right;
	vector3 up;
	vector3 worldup;

	vector3 crossproduct(vector3 one, vector3 two);
	vector3 normalize(vector3 one);

	float yaw;
	float pitch;
};
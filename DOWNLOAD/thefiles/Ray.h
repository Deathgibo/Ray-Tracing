#pragma once
#include "vector3.h"

class Ray {
public:
	Ray();
	void setray(float px, float py, float pz, float dx, float dy, float dz);
	void setray(vector3 rayposition, vector3 raydirection);
	void calculateray();
	float getrefn() { return refn; }
	void setrefn(float r) { refn = r; }

	vector3 getposition() { return position; }
	vector3 getdirection() { return direction; }
	vector3 color;
private:
	vector3 position;
	vector3 direction; //normalized
	float refn;
};

//nx and ny is resolution we want in terms of precision
//l-r t-b is just the screen we want to scale it too
//i j start from -.5 and go to nx ny - .5
//u and v are just some point within the box l-r t-b


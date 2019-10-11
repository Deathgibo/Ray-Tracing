#pragma once
#include "vector3.h"
#include "Ray.h"
#include "Texture.h"
#include <iostream>

class Shape {
public:
	Shape(float r, float g, float b, int reflect = 0, int refract = 0, float ref = 1) {
		color.x = r; color.y = g; color.z = b; ts = 0; reflective = reflect; refractive = refract; refn = ref;
		reversenormals = 0;
	}

	virtual bool checkcollision(Ray r) = 0;
	virtual void move(float x, float y, float z) = 0;
	virtual vector3 calculatenormal(Ray r) = 0;
	virtual vector3 getposition() = 0;
	virtual vector3 getcollisionpoint(Ray r) = 0;
	virtual vector3 calculatetexturecolor(Ray r) = 0;
	void setreflective(int i) { reflective == i; }

	vector3 color;
	float ts;
	int reflective;
	int refractive;
	float refn;
	bool reversenormals;
	Texture texture;
	bool skybox = false;
private:

};
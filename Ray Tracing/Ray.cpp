#include "Ray.h"

Ray::Ray()
{
	calculateray();
}

void Ray::setray(float px, float py, float pz, float dx, float dy, float dz)
{
	position.x = px; position.y = py; position.z = pz;
	direction.x = dx; direction.y = dy; direction.z = dz;
}

void Ray::setray(vector3 rayposition, vector3 raydirection)
{
	position = rayposition;
	direction = raydirection;
}

void Ray::calculateray()
{

}

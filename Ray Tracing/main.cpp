#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>

#include "vector3.h"
#include "camera.h"
#include "Ray.h"
#include "Triangle.h"
#include "Circle.h"
using namespace std;

struct light {
	vector3 direction;
	float intensity;

	light(float x, float y, float z, float i)
	{
		direction.x = x; direction.y = y; direction.z = z;
		direction = direction.normalizereturn();
		intensity = i;
	}
};

const int screen_height = 720;
const int screen_width = 1280;

SDL_Window* gwindow = 0;
SDL_Surface* gsurface = 0;
SDL_Renderer* grender = 0;
SDL_Texture* ball = 0;

/*image box info*/
const float l = -screen_width / 2;
const float r = screen_width / 2;
const float t = -screen_height / 2;
const float b = screen_height / 2;
float dbox = 15; //this is like the fov too low and nothing can be close
float fov = tan(.2 / 2);


//screen resolution these numbers determine how many pixels, so rays, we have
int resolution = 124;

camera thecamera(0, 0, 0);

vector<Shape*> theshapes;
//triangle t1(300, 300, -3, 400, 300, -3, 350, 150, -3);
//triangles are clockwise
Triangle t1(-50, 50, -10, 50, 50, -10, 0, 0, -10, 0,255,0);
vector3 fl(-100,-16,0);
vector3 bl(-100,-16,-50);
vector3 br(100,-16,-50);
vector3 fr(100,-16,0);
Triangle g1(fl.x,fl.y,fl.z,bl.x,bl.y,bl.z,br.x,br.y,br.z,255,255,255,1);
Triangle g2(br.x,br.y,br.z,fr.x,fr.y,fr.z,fl.x,fl.y,fl.z,255,255,255,1);
//Triangle g1(-10, 0, -5, -10, 0, -8, 10, 0, -8, 150, 0, 0);
//Triangle g2(10, 0, -8, 10, 0, -5, -10, 0, -5, 150, 0, 0);
//Triangle g1(10, 5, -25, 20, 0, -15, 0, 10, -15, 150, 0, 0);//y works, x is inverse, z is inverse
//Triangle g2(-20, 0, -5, -10, 0, -8, 10, 0, -8, 150, 0, 0);
Circle s1(18, 0, -30, 15, 155, 155, 0);
Circle s2(-18, 0, -30, 15, 0, 255, 255);
Circle s3(10, 0, -15, 2, 200, 0, 0);
Circle s4(0, 0, -14, 2, 0, 0, 200);

light light1(.5, -.5, 0, .5);
float light1yaw = 270;
float light1pitch = 90;
light light2(.5, 0, -1, .8);
vector<light> lights;

bool init();
bool loadmedia();
SDL_Texture* loadimage(string a);
SDL_Texture* loadimageclear(string a);
void close();
void updatelight();

void calculateraysOrtho();
void calculateraysProj();
void drawrays(vector<Ray> therays);
float toradians(float degrees);
vector3 calculatecolor(Ray theray, float ts = 0, bool cancelambient = false);
vector3 componentwise(vector3 one, vector3 two);

int main(int argc, char* args[])
{
	if (!init())
	{
		cout << "could not initialize!";
	}
	else
	{
		if (!loadmedia())
		{
			cout << "could not load media!";
		}
		else
		{
			//thecamera.printinfo();
			//calculateraysOrtho();

			//theshapes.push_back(&t1);
			theshapes.push_back(&s1);
			theshapes.push_back(&s2);
			theshapes.push_back(&s3);
			theshapes.push_back(&s4);
			theshapes.push_back(&g1);
			theshapes.push_back(&g2);

			lights.push_back(light1);
			lights.push_back(light2);

			bool leave = false;
			SDL_Event e;
			while (leave == false)
			{
				while (SDL_PollEvent(&e) != 0)
				{
					if (e.type == SDL_QUIT)
					{
						leave = true;
					}
					if (e.type == SDL_KEYDOWN)
					{
						switch (e.key.keysym.sym)
						{
						case SDLK_f: fov += .1; break;
						case SDLK_g: fov -= .1; break;
						case SDLK_r: dbox++; break;
						case SDLK_q: dbox--; break;
						case SDLK_a: for (int i = 0; i < theshapes.size(); i++) { theshapes[i]->move(-1, 0, 0); }; break;
						case SDLK_d: for (int i = 0; i < theshapes.size(); i++) { theshapes[i]->move(1, 0, 0); };  break;
						case SDLK_w: for (int i = 0; i < theshapes.size(); i++) { theshapes[i]->move(0, 0, 1); };  break;
						case SDLK_s: for (int i = 0; i < theshapes.size(); i++) { theshapes[i]->move(0, 0, -1); }; break;
						case SDLK_LCTRL: for (int i = 0; i < theshapes.size(); i++) { theshapes[i]->move(0, -1, 0); };  break;
						case SDLK_SPACE: for (int i = 0; i < theshapes.size(); i++) { theshapes[i]->move(0, 1, 0); }; break;
						case SDLK_RIGHT: resolution += sqrt(resolution); cout << resolution << endl; break;
						case SDLK_LEFT: if (resolution > sqrt(resolution)) resolution -= sqrt(resolution); cout << resolution << endl; break;
						case SDLK_v: break;
						case SDLK_i: light1yaw+=10; updatelight(); break;
						case SDLK_k: light1yaw-=10; updatelight(); break;
						case SDLK_j: light1pitch-=10; updatelight(); break;
						case SDLK_l: light1pitch+=10; updatelight(); break;
						}
					}
				}
				/*steps
				1.) calculate position and direction for ray tracers
				2.) calculate the intersections closest hit
				3.) calculate shading/colors/shadows/lighting/etc

				*/
				SDL_SetRenderDrawColor(grender, 255, 255, 255, 255);
				SDL_RenderClear(grender);

				//cout << theshapes[0]->getposition().y << endl;
				calculateraysProj();


				SDL_RenderPresent(grender);
				//SDL_Delay(10000);
			}
		}

	}

	close();
	return 0;
}

bool init() //initialize video, screen, render and color, lineartexture, IMG
{
	bool success = true;
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
	{
		success = false;
	}
	else
	{
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			success = false;
		}

		gwindow = SDL_CreateWindow("Ray Tracer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_width, screen_height, SDL_WINDOW_SHOWN);
		if (gwindow == 0)
		{
			success = false;
		}
		grender = SDL_CreateRenderer(gwindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		if (grender == 0)
		{
			success = false;
		}
		else
		{
			SDL_SetRenderDrawColor(grender, 255, 255, 255, 255);

			int imgflags = IMG_INIT_PNG;
			if (!(IMG_Init(imgflags) & imgflags))
			{
				success = false;
			}
		}
	}


	return success;
}
bool loadmedia()
{
	bool success = true;


	return success;
}
SDL_Texture* loadimage(string a)
{
	SDL_Surface* tempsurf;
	tempsurf = IMG_Load(a.c_str());
	if (tempsurf == 0)
	{
		cout << "could not load image";
	}
	SDL_Texture* temptext;
	temptext = SDL_CreateTextureFromSurface(grender, tempsurf);

	SDL_FreeSurface(tempsurf);
	return temptext;
}
SDL_Texture* loadimageclear(string a) //white
{
	SDL_Surface* tempsurf;
	tempsurf = IMG_Load(a.c_str());
	if (tempsurf == 0)
	{
		cout << "could not load image";
	}
	SDL_SetColorKey(tempsurf, SDL_TRUE, SDL_MapRGB(tempsurf->format, 255, 255, 255));
	SDL_Texture* temptext;
	temptext = SDL_CreateTextureFromSurface(grender, tempsurf);

	SDL_FreeSurface(tempsurf);
	return temptext;
}

void close()
{
	SDL_DestroyWindow(gwindow);
	gwindow = 0;
	SDL_DestroyRenderer(grender);
	grender = 0;

	SDL_Quit();
	IMG_Quit();
}

void calculateraysOrtho()
{
	int nx = resolution;
	int ny = resolution;
	float u, v;

	float aspectratio = (r - l) / (t - b);

	vector<Ray> therays;

	/*loop through all the x and y pixels in our chosen resolution, u is the very left of our screen plus some percentage of its width which is based on pixel resolution width*/
	for (int i = 0; i < nx; i++)
	{

		u = l + (r - l) * ((i + .5) / static_cast<double>(nx));
		//u = u * fov;
		for (int j = 0; j < ny; j++)
		{
			v = b + (t - b) * ((j + .5) / static_cast<double>(ny));
			//v = v * fov;

			vector3 rayorigin = thecamera.Getposition() + thecamera.getright() * u + thecamera.getup() * v;

			Ray tmpray;
			tmpray.setray(rayorigin, thecamera.GetLookAt());
			tmpray.color = rayorigin;

			therays.push_back(tmpray);
		}
	}

	drawrays(therays);
}

void calculateraysProj()
{
	int nx = resolution;
	int ny = resolution;
	float u, v;

	vector<Ray> therays;

	float aspectratio = (r - l) / (t - b);
	/*loop through all the x and y pixels in our chosen resolution, u is the very left of our screen plus some percentage of its width which is based on pixel resolution width*/
	for (int i = 0; i < nx; i++)
	{

		u = l + (r - l) * ((i + .5) / static_cast<double>(nx));
		u = u*fov;
		for (int j = 0; j < ny; j++)
		{
			v = b + (t - b) * ((j + .5) / static_cast<double>(ny));
			v = v * fov;
			vector3 rayorigin = thecamera.Getposition();
			vector3 direction = thecamera.GetLookAt() * dbox + thecamera.getright() * u + thecamera.getup() * v;
			direction.normalize();
			//rays are pointing in negative z
			//if(i == 0)
				//cout << direction.x << " "<<direction.y << " " << direction.z << endl;

			Ray tmpray;
			tmpray.setray(rayorigin, direction);

			tmpray.color = -direction;

			therays.push_back(tmpray);
		}
	}

	drawrays(therays);
}

void drawrays(vector<Ray> therays)
{
	int nx = resolution;
	int ny = resolution;

	float width = ((r - l)) / nx;
	float height = (b - t) / ny;

	int acounter = 0;
	int counterh = 0;
	for (int i = 0; i < nx*ny; i++)
	{
		if (i % ny == 0 && i != 0)
		{
			acounter++;
			counterh = 0;
		}

		vector3 thecolor;
		thecolor = calculatecolor(therays[i]);

		SDL_SetRenderDrawColor(grender, thecolor.x, thecolor.y, thecolor.z, 0);


		float startingheight;
		if (counterh == 0)
			startingheight = 0;
		else
			startingheight = height * (counterh - 1);
		float heightlength = (height * (counterh + 1)) - startingheight;

		float startingwidth;
		if (acounter == 0)
			startingwidth = 0;
		else
			startingwidth = width * (acounter - 1);
		float widthlength = (width * (acounter + 1)) - startingwidth;

		//SDL_Rect therect = { therays[i].getposition().x, therays[i].getposition().y ,width, height };
		SDL_Rect therect = { startingwidth, startingheight ,widthlength, heightlength };
		SDL_RenderFillRect(grender, &therect);
		//SDL_RenderDrawPoint(grender, (therays[i].getdirection()).x, (therays[i].getdirection()).y);
		/*for (int q = 0; q < widthlength; q++)
		{
			for (int w = 0; w < heightlength; w++)
			{
				SDL_RenderDrawPoint(grender, startingwidth + q, startingheight + w);
			}
		}*/


		counterh++;
	}
}

float toradians(float degrees)
{
	return (degrees*3.14) / 180.0;
}

void updatelight()
{
	lights[0].direction.x = sin(toradians(light1pitch)) * cos(toradians(light1yaw));
	lights[0].direction.y = cos(toradians(light1pitch));
	lights[0].direction.z = sin(toradians(light1pitch)) * sin(toradians(light1yaw));
	lights[0].direction.normalize();

	//cout << lights[0].direction.x << " " << lights[0].direction.y << " " << lights[0].direction.z << endl;
}

vector3 calculatecolor(Ray theray, float tts, bool cancelambient)
{
	vector3 thecolor(0, 0, 0);

	int closestindex = 0;
	bool hitsomething = false;

	vector3 n;
	vector3 r;
	vector3 positionhit;

	//do a for loop through all shapes to pick the closest shape hit, then move on
	for (int x = 0; x < theshapes.size(); x++)
	{
		if (theshapes[x]->checkcollision(theray) && theshapes[x]->ts > tts)
		{
			if (hitsomething == false)
			{
				closestindex = x;
				hitsomething = true;
				//cout << theshapes[x]->ts << endl;
			}
			else
			{
				if (theshapes[x]->ts < theshapes[closestindex]->ts)
				{
					closestindex = x;
				}
			}
		}
	}

	vector3 ambientcolor(255, 255, 255);
	vector3 specularcolor(255, 255, 255);
	vector3 lightcolor(255, 255, 255);
	float specularintensity = .2f;
	ambientcolor = ambientcolor * .2f;

	if (hitsomething)//hitsomething
	{

		/* calculate light first before we overwrite ts */
		n = theshapes[closestindex]->calculatenormal(theray);//closestshape
		n.normalize();

		vector3 currentcolor(0, 0, 0);
		//thecolor = theshapes[closestindex]->color;
		for (int c = 0; c < lights.size(); c++)
		{
			//v is ray direction
			//l is light direction
			vector3 h = theray.getdirection() + lights[c].direction;
			h.normalize();

			currentcolor = (theshapes[closestindex]->color * (max(0.0f, n*lights[c].direction) *  lights[c].intensity)) + (specularcolor * pow(max(0.0f, n*h), 100) * specularintensity);
			thecolor = thecolor + (theshapes[closestindex]->color * (max(0.0f, n*lights[c].direction) *  lights[c].intensity)) + (specularcolor * pow(max(0.0f, n*h), 100) * specularintensity);
			//vector3 thecolor = (theshapes[closestindex]->color * (max(0.0f, n*light1.normalizereturn()) *  dI)); //+(ambientcolor*1);// (max(0.0f, n*light1)

			/*check if it hits anything from light source if not do regular else do regular color*/
			Ray lightray;
			vector3 positionvector = theshapes[closestindex]->getcollisionpoint(theray);

			lightray.setray(positionvector, -lights[c].direction);//lights[0].direction - positionvector
			bool objecthit = false;
			for (int z = 0; z < theshapes.size(); z++)
			{
				if (z != closestindex)
				{
					if (theshapes[z]->checkcollision(lightray) && theshapes[z]->ts > .5)
					{
						objecthit = true;
					}
				}
			}
			if (!objecthit)
			{
				//thecolor = thecolor


				//SDL_SetRenderDrawColor(grender, thecolor.x, thecolor.y, thecolor.z, 0);
				//SDL_SetRenderDrawColor(grender, therays[i].color.x, therays[i].color.y, therays[i].color.z, 0);
				//SDL_SetRenderDrawColor(grender, 0, 0, 255, 0);
			}
			else
			{
				thecolor = thecolor - currentcolor;
				//SDL_SetRenderDrawColor(grender, 0, 0, 0, 0);
			}
		}

		thecolor = thecolor + ambientcolor;
		
		thecolor.clampthis(255);
	}
	else
	{
		if(!cancelambient)
			thecolor = thecolor + ambientcolor;
	}

	if (theshapes[closestindex]->reflective == 1)
	{
		float mirrorcolorz = .5f;
		//use r instead of d (therays[i].direction)
		positionhit = theshapes[closestindex]->getcollisionpoint(theray);
		r = theray.getdirection() - (n* ((theray.getdirection()*n) * 2));
		Ray rr;
		rr.setray(positionhit, r);

		vector3 reflectivecolor = calculatecolor(rr,.5f) * mirrorcolorz;

	//	cout << reflectivecolor.x << " " << reflectivecolor.y << " " << reflectivecolor.z << endl;
		thecolor = thecolor + reflectivecolor;
		//thecolor = reflectivecolor;
		thecolor.clampthis(255);
	}

	return thecolor;
}





/*




*/

vector3 componentwise(vector3 one, vector3 two)
{
	vector3 p;
	/*p.x = (one.x/255.0) * (two.x / 255.0);
	p.y = (one.y / 255.0) * (two.y / 255.0);
	p.z = (one.z / 255.0) * (two.z / 255.0);
	
	p.x = p.x * 255;
	p.y = p.y * 255;
	p.z = p.z * 255;*/

	p.x = (one.x * two.x) / 2;
	p.y = (one.y * two.y) / 2;
	p.z = (one.z * two.z) / 2;

	return p;
}
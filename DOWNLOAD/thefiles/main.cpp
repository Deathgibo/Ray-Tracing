#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <random>
#include <fstream>

#include "vector3.h"
#include "camera.h"
#include "Ray.h"
#include "Triangle.h"
#include "Circle.h"
#include "Texture.h"
#include <time.h>
#include <chrono>
using namespace std::chrono;
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

bool antialiasing = false;

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
//const float l = -100;
//const float r = 100;
//const float t = -100;
//const float b = 100;
float dbox = 15; //this is like the fov too low and nothing can be close
float fov = tan(.1 / 2);//.2/2
bool pScreenShot = false;

//screen resolution these numbers determine how many pixels, so rays, we have
int resolution = 100;

camera thecamera(0, 0, 0);

vector<Shape*> theshapes;
//triangle t1(300, 300, -3, 400, 300, -3, 350, 150, -3);
//triangles are clockwise
Triangle t1(-50, 50, 50,0,1, 
	50, 50, 50,1,1, 
	0, 0, 50,.5,0,
	0,255,0,
	0,1,1.3f);
Triangle t2(-50, 50, 50, 0, 1,
	0, 0, 50, .5, 0,
	-50, 0, 50, 0, 0,
	0, 255, 0,
	0, 1, 1.3f);
Triangle t3(50, 50, 50, 1, 1,
	50, 0, 50, 1, 0,
	0, 0, 50, .5, 0,
	0, 255, 0,
	0, 1, 1.3f);
vector3 fl(-100,-16,0);
vector3 bl(-100,-16,-50);
vector3 br(100,-16,-50);
vector3 fr(100,-16,0);
Triangle g1(fl.x,fl.y,fl.z,0,0,
	bl.x,bl.y,bl.z,0,1,
	br.x,br.y,br.z,1,1,
	0, 0, 55,1);
Triangle g2(br.x,br.y,br.z,1,1,
	fr.x,fr.y,fr.z,1,0,
	fl.x,fl.y,fl.z,0,0,
	0,0,55,1);
Triangle t4(-25, 0, -50, 0, 0,
	25, 0, -50, 1, 0,
	0, 25, -50, .5, 1,
	0, 255, 0);
/*Sky box*/
int boxsize = 200;
vector3 fbl(-boxsize, boxsize, boxsize);
vector3 ftl(-boxsize, -boxsize, boxsize);
vector3 fbr(boxsize, boxsize, boxsize);
vector3 ftr(boxsize, -boxsize, boxsize);
vector3 bbl(-boxsize, boxsize, -boxsize);
vector3 btl(-boxsize, -boxsize, -boxsize);
vector3 bbr(boxsize, boxsize, -boxsize);
vector3 btr(boxsize, -boxsize, -boxsize);
Triangle s1(ftl.x, ftl.y, ftl.z, 0, 1,
	ftr.x, ftr.y, ftr.z, 1, 1,
	fbl.x, fbl.y, fbl.z, 0, 0,
	255, 255, 255);
Triangle s2(ftr.x, ftr.y, ftr.z, 1, 1,
	fbr.x, fbr.y, fbr.z, 1, 0,
	fbl.x, fbl.y, fbl.z, 0, 0,
	255, 255, 255);
Triangle s3(bbr.x, bbr.y, bbr.z, 1, 0,
	fbr.x, fbr.y, fbr.z, 0, 0,
	ftr.x, ftr.y, ftr.z, 0, 1,
	255, 255, 255);
Triangle s4(ftr.x, ftr.y, ftr.z, 0, 1,
	btr.x, btr.y, btr.z, 1, 1,
	bbr.x, bbr.y, bbr.z, 1, 0,
	255, 255, 255);
Triangle s5(btr.x, btr.y, btr.z, 0, 1,
	bbl.x, bbl.y, bbl.z, 1, 0,
	bbr.x, bbr.y, bbr.z, 0, 0,
	255, 255, 255);
Triangle s6(btl.x, btl.y, btl.z, 1, 1,
	bbl.x, bbl.y, bbl.z, 1, 0,
	btr.x, btr.y, btr.z, 0, 1,
	255, 255, 255);
Triangle s7(btl.x, btl.y, btl.z, 0, 1,
	ftl.x, ftl.y, ftl.z, 1, 1,
	bbl.x, bbl.y, bbl.z, 0, 0,
	255, 255, 255);
Triangle s8(ftl.x, ftl.y, ftl.z, 1, 1,
	bbl.x, bbl.y, bbl.z, 0, 0,
	fbl.x, fbl.y, fbl.z, 1, 0,
	255, 255, 255);
Triangle s9(ftl.x, ftl.y, ftl.z, 0, 0,
	btl.x, btl.y, btl.z, 0, 1,
	ftr.x, ftr.y, ftr.z, 1, 0,
	255, 255, 255);
Triangle s10(btr.x, btr.y, btr.z, 1, 1,
	ftr.x, ftr.y, ftr.z, 1, 0,
	btl.x, btl.y, btl.z, 0, 1,
	255, 255, 255);
Triangle s11(fbl.x, fbl.y, fbl.z, 1, 1,
	bbl.x, bbl.y, bbl.z, 0, 1,
	fbr.x, fbr.y, fbr.z, 1, 0,
	255, 255, 255);
Triangle s12(bbl.x, bbl.y, bbl.z, 0, 1,
	bbr.x, bbr.y, bbr.z, 0, 0,
	fbr.x, fbr.y, fbr.z, 1, 0,
	255, 255, 255);
//Triangle g1(-10, 0, -5, -10, 0, -8, 10, 0, -8, 150, 0, 0);
//Triangle g2(10, 0, -8, 10, 0, -5, -10, 0, -5, 150, 0, 0);
//Triangle g1(10, 5, -25, 20, 0, -15, 0, 10, -15, 150, 0, 0);//y works, x is inverse, z is inverse
//Triangle g2(-20, 0, -5, -10, 0, -8, 10, 0, -8, 150, 0, 0);
Circle c1(18, 0, -32, 15, 155, 155, 0,0,1,1.3f);
Circle c2(-38, 0, -30, 15, 0, 255, 255);
Circle c3(90, 0, -15, 2, 200, 0, 0);
Circle c4(80, 0, -14, 2, 0, 0, 200);
Circle c5(80, 0, -30, 5, 0, 0, 200);
Circle c6(-60, 20, -30, 8, 0, 0, 0,1);
Circle c7(-80, 20, -75, 15, 0, 0, 0, 0);


vector3 fl2(70, 15, -20);
vector3 bl2(70, -15, -20);
vector3 br2(100, -15, -20);
vector3 fr2(100, 15, -20);
Triangle m1(fl2.x, fl2.y, fl2.z, 0, 0,
	bl2.x, bl2.y, bl2.z, 0, 1,
	br2.x, br2.y, br2.z, 1, 1,
	0, 0, 0,1);
Triangle m2(br2.x, br2.y, br2.z, 1, 1,
	fr2.x, fr2.y, fr2.z, 1, 0,
	fl2.x, fl2.y, fl2.z, 0, 0,
	0, 0, 0, 1);
vector3 fl3(100, 15, 0);
vector3 bl3(100, -15, 0);
vector3 br3(100, -15, -20);
vector3 fr3(100, 15, -20);
Triangle m3(fl3.x, fl3.y, fl3.z, 0, 0,
	bl3.x, bl3.y, bl3.z, 0, 1,
	br3.x, br3.y, br3.z, 1, 1,
	0, 0, 0, 1);
Triangle m4(br3.x, br3.y, br3.z, 1, 1,
	fr3.x, fr3.y, fr3.z, 1, 0,
	fl3.x, fl3.y, fl3.z, 0, 0,
	0, 0, 0, 1);

light light1(.5, -.5, 0, .5);
float light1yaw = 270;
float light1pitch = 90;
light light2(0, -.8, -.2, .8);
light light3(0, 1, 0, .5);
light light4(-1, 0, 0, .5);
light light5(1, 0, 0, .5);
light light6(0, -1, 0, .5);
light light7(0, -0, -1, .5);
light light8(0, 0, 1, .5);
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
vector3 calculatecolor(Ray theray, float ts = 0, bool cancelambient = false, bool norefract = false);
vector3 componentwise(vector3 one, vector3 two);
vector3 Reflect(Ray theray, Shape& shape);
vector3 GlossyReflect(Ray theray, Shape& shape, float a);
vector3 Refract(Ray theray, Shape& shape);
float clamp(float min, float max, float num);
void fresnel(vector3 &I, vector3 &N, const float n1, float n2, float &kr);
vector3 crossproduct(vector3 one, vector3 two);
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
			s1.skybox = true; s2.skybox = true;
			s3.skybox = true; s4.skybox = true;
			s5.skybox = true; s6.skybox = true;
			s7.skybox = true; s8.skybox = true;
			s9.skybox = true; s10.skybox = true;
			s11.skybox = true; s12.skybox = true;
			//thecamera.printinfo();
			//calculateraysOrtho();

			Texture white;
			white.makecolortexture(255, 255, 255);
			Texture checkerboard;
			checkerboard.makecheckerboardtexture();
			Texture blackwhite;
			blackwhite.maketwocolor();
			Texture brick;
			brick.makeloadedtexture("brick.png");
			Texture other;
			other.makeloadedtexture("world2.png");
			Texture skyboxtop;
			Texture skyboxleft;
			Texture skyboxright;
			Texture skyboxbottom;
			Texture skyboxfront;
			Texture skyboxback;
			skyboxtop.makeloadedtexture("morning_up.tga"); skyboxleft.makeloadedtexture("morning_lf.tga"); skyboxright.makeloadedtexture("morning_rt.tga");
			skyboxbottom.makeloadedtexture("morning_dn.tga"); skyboxfront.makeloadedtexture("morning_ft.tga"); skyboxback.makeloadedtexture("morning_bk.tga");

			t1.texture = checkerboard;
			c2.texture = other;
			//g1.texture = checkerboard;
			//g2.texture = checkerboard;
			t2.texture = checkerboard;
			t3.texture = checkerboard;
			s1.texture = skyboxfront; s2.texture = skyboxfront;
			s3.texture = skyboxleft; s4.texture = skyboxleft;
			s5.texture = skyboxback; s6.texture = skyboxback;
			s7.texture = skyboxright; s8.texture = skyboxright;
			s9.texture = skyboxbottom; s10.texture = skyboxbottom;
			s11.texture = skyboxtop; s12.texture = skyboxtop;
			c5.texture = brick;
			t4.texture = checkerboard;
			c1.texture = checkerboard;
			//c6.texture = brick;
			c7.texture = checkerboard;

			theshapes.push_back(&t1);
			theshapes.push_back(&t2);
			theshapes.push_back(&t3);
			theshapes.push_back(&t4);
			theshapes.push_back(&c1);
			theshapes.push_back(&c2);
			theshapes.push_back(&c3);
			theshapes.push_back(&c4);
			theshapes.push_back(&c5);
			theshapes.push_back(&c6);
			theshapes.push_back(&c7);
			theshapes.push_back(&g1);
			theshapes.push_back(&g2);
			theshapes.push_back(&m2);
			theshapes.push_back(&m1);
			theshapes.push_back(&m3);
			theshapes.push_back(&m4);

			/*skybox*/
			theshapes.push_back(&s1);
			theshapes.push_back(&s2);
			theshapes.push_back(&s3);
			theshapes.push_back(&s4);
			theshapes.push_back(&s5);
			theshapes.push_back(&s6);
			theshapes.push_back(&s7);
			theshapes.push_back(&s8);
			theshapes.push_back(&s9);
			theshapes.push_back(&s10);
			theshapes.push_back(&s11);
			theshapes.push_back(&s12);

			//lights.push_back(light1);
			//lights.push_back(light2);
			//lights.push_back(light3);
			lights.push_back(light4);
			lights.push_back(light5);
			lights.push_back(light6);
			lights.push_back(light7);
			lights.push_back(light8);

			bool leave = false;
			float fps = 0;

			SDL_Event e;
			while (leave == false)
			{
				auto start = high_resolution_clock::now();
				pScreenShot = false;
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
						case SDLK_d: break;
						case SDLK_a: break;
						case SDLK_w: break;
						case SDLK_s:  break;
						case SDLK_SPACE:  break;
						case SDLK_LCTRL:  break;
						case SDLK_RIGHT:  break;
						case SDLK_LEFT:  break;
						case SDLK_UP:  break;
						case SDLK_DOWN:  break;
						case SDLK_i: light1yaw+=10; updatelight(); break;
						case SDLK_k: light1yaw-=10; updatelight(); break;
						case SDLK_j: light1pitch-=10; updatelight(); break;
						case SDLK_l: light1pitch+=10; updatelight(); break;
						case SDLK_v: resolution = 80; break;
						case SDLK_b: if ((int)(resolution - sqrt(resolution)) > 0) { resolution -= sqrt(resolution); } cout << resolution << endl; break;
						case SDLK_n: resolution += sqrt(resolution); cout << resolution << endl; break;
						case SDLK_m: resolution = 800; break;
						case SDLK_COMMA: resolution = 1200; break;
						case SDLK_ESCAPE: leave = true; break;
						case SDLK_x: antialiasing = !antialiasing; break;
						case SDLK_p: pScreenShot = true; break;
						}
					}
				}
				const Uint8* currentkeystate = SDL_GetKeyboardState(0);
				if (currentkeystate[SDL_SCANCODE_A])
				{
					thecamera.Move(-1, 0, 0);
				}
				if (currentkeystate[SDL_SCANCODE_D])
				{
					thecamera.Move(1, 0, 0);
				}
				if (currentkeystate[SDL_SCANCODE_W])
				{
					thecamera.Move(0, 0, 1);
				}
				if (currentkeystate[SDL_SCANCODE_S])
				{
					thecamera.Move(0, 0, -1);
				}
				if (currentkeystate[SDL_SCANCODE_SPACE])
				{
					thecamera.Move(0, 1, 0);
				}
				if (currentkeystate[SDL_SCANCODE_LCTRL])
				{
					thecamera.Move(0, -1, 0);
				}
				if (currentkeystate[SDL_SCANCODE_UP])
				{
					thecamera.setangles(-1, 0);
				}
				if (currentkeystate[SDL_SCANCODE_DOWN])
				{
					thecamera.setangles(1, 0);
				}
				if (currentkeystate[SDL_SCANCODE_LEFT])
				{
					thecamera.setangles(0, -2);
				}
				if (currentkeystate[SDL_SCANCODE_RIGHT])
				{
					thecamera.setangles(0, 2);
				}
				/*steps
				1.) calculate position and direction for ray tracers
				2.) calculate the intersections closest hit
				3.) calculate shading/colors/shadows/lighting/etc

				*/
				SDL_SetRenderDrawColor(grender, 255, 0, 255, 255);
				SDL_RenderClear(grender);
				thecamera.updatecamera();
				//cout << theshapes[0]->getposition().y << endl;
				calculateraysProj();

				if (pScreenShot)
				{
					
					const Uint32 format = SDL_PIXELFORMAT_ARGB8888;
					SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(0, screen_width, screen_height, 32, format);
					SDL_RenderReadPixels(grender, NULL, format, surface->pixels, surface->pitch);
					
					bool leave = false;
					string name = "screenshot.bmp";
					int number = 2;
					while (!leave)
					{
						std::ifstream infile(name);
						bool alreadyused = infile.good();
						if (alreadyused)
						{
							string newstring = "screenshot";
							string thenumber = to_string(number);
							newstring.append(thenumber);
							newstring.append(".bmp");

							name = newstring;
							number++;
						}
						else
						{
							SDL_SaveBMP(surface, name.c_str());
							leave = true;
							cout << "screenshot taken" << endl;
						}

					}
					
					SDL_FreeSurface(surface);
				}

				SDL_RenderPresent(grender);



				auto stop = high_resolution_clock::now();
				auto duration = duration_cast<microseconds>(stop - start);
				fps = 1 / (duration.count() / 1000000.0f);
				int fpsint = fps;
				string fpsstring = "FPS: ";
				fpsstring.append(to_string(fpsint));
				const char* lol = fpsstring.c_str();
				SDL_SetWindowTitle(gwindow, lol);

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

		vector3 thecolor(100, 0, 0);
		if (!antialiasing)
		{
			thecolor = calculatecolor(therays[i]);
		}
		else
		{
			/*float u, v;
			for (int i = 0; i < nx; i++)
			{
				u = l + (r - l) * ((i + .5) / static_cast<double>(nx));
				u = u * fov;
				for (int j = 0; j < ny; j++)
				{
					v = b + (t - b) * ((j + .5) / static_cast<double>(ny));
					v = v * fov;

					vector3 c(0, 0, 0);
					int n = 4;
					for (int p = 0; p < n - 1; p++)
					{
						for (int q = 0; q < n - 1; q++)
						{
							vector3 rayorigin = thecamera.Getposition();
							vector3 direction = thecamera.GetLookAt() * dbox + thecamera.getright() * u + thecamera.getup() * v;
							direction.normalize();

							Ray sampleray;
							sampleray.setray(rayorigin, direction);
							sampleray.color = -direction;
							c = c + calculatecolor(sampleray, 0);
						}
					}

				}
			}
			//thecolor = c;*/
		}

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
		//cout << startingwidth << " " << startingheight << endl;
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

vector3 calculatecolor(Ray theray, float tts, bool cancelambient, bool norefract)//tts is saying that the object hit has to be farther away than tts percent of the rays origin
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

	if (hitsomething && theshapes[closestindex]->refractive && !norefract)
	{
		thecolor = Refract(theray, *theshapes[closestindex]);
	}
	else if (hitsomething)
	{

		/* calculate light first before we overwrite ts */
		n = theshapes[closestindex]->calculatenormal(theray);//closestshape
		n.normalize();

		vector3 currentcolor(0, 0, 0);
		//vector3 shapescolor = theshapes[closestindex]->color;
		vector3 shapescolor = theshapes[closestindex]->calculatetexturecolor(theray);
		//thecolor = theshapes[closestindex]->color;
		for (int c = 0; c < lights.size(); c++)
		{
			//v is ray direction
			//l is light direction
			vector3 h = theray.getdirection() + lights[c].direction;
			h.normalize();

			/*doing regular diffuse and specular lighting*/
			currentcolor = (shapescolor * (max(0.0f, n*lights[c].direction) *  lights[c].intensity)) + (specularcolor * pow(max(0.0f, n*h), 100) * specularintensity);
			thecolor = thecolor + (shapescolor * (max(0.0f, n*lights[c].direction) *  lights[c].intensity)) + (specularcolor * pow(max(0.0f, n*h), 100) * specularintensity);
			//vector3 thecolor = (shapescolor * (max(0.0f, n*light1.normalizereturn()) *  dI)); //+(ambientcolor*1);// (max(0.0f, n*light1)

			/*check if from collision point in direction to light if it hits an object if it does its in shadow and shouldnt get that light*/
			Ray lightray;
			vector3 positionvector = theshapes[closestindex]->getcollisionpoint(theray);

			lightray.setray(positionvector, -lights[c].direction);//lights[0].direction - positionvector
			bool objecthit = false;
			if(theshapes[closestindex]->skybox != true)
			{
				for (int z = 0; z < theshapes.size(); z++)
				{
					if (z != closestindex)
					{
						if (theshapes[z]->checkcollision(lightray) && theshapes[z]->ts > .5 && theshapes[z]->skybox != true)
						{
							objecthit = true;
						}
					}
				}
			}
			if (!objecthit)
			{
			}
			else
			{
				thecolor = thecolor - currentcolor;
			}
		}
		
		thecolor = thecolor + ambientcolor;
		
		thecolor.clampthis(255);

		/*creating reflective ray and seeing what that color is to add to the color*/
		if (theshapes[closestindex]->reflective == 1)
		{
			//vector3 reflectivecolor = GlossyReflect(theray, *theshapes[closestindex],.07);
			vector3 reflectivecolor = Reflect(theray, *theshapes[closestindex]);

			thecolor = thecolor + reflectivecolor;
			thecolor.clampthis(255);
		}
	}
	else
	{
		if(!cancelambient)
			thecolor = thecolor + ambientcolor;
	}
	thecolor = thecolor * 1.2;
	thecolor.clampthis(255);
	return thecolor;
}

vector3 Reflect(Ray theray, Shape& shape)
{
	float mirrorcolorz = 0.5f;
	vector3 n = shape.calculatenormal(theray); n.normalize();
	//use r instead of d (therays[i].direction)
	vector3 positionhit = shape.getcollisionpoint(theray);
	vector3 r = theray.getdirection() - (n* ((theray.getdirection()*n) * 2));
	Ray rr;
	rr.setray(positionhit, r);

	vector3 reflectivecolor = calculatecolor(rr, 1.1f) * mirrorcolorz;
	return reflectivecolor;
}

vector3 GlossyReflect(Ray theray, Shape& shape, float a)
{
	//Get R
	vector3 n = shape.calculatenormal(theray); n.normalize();
	vector3 positionhit = shape.getcollisionpoint(theray);
	vector3 r = theray.getdirection() - (n* ((theray.getdirection()*n) * 2));
	Ray rr;
	rr.setray(positionhit, r);
	//Get U,V
	vector3 V = r;
	if (V.x < V.y && V.x < V.z)
		V.x = 1;
	else if (V.y < V.x && V.y < V.z)
		V.y = 1;
	else
		V.z = 1;
	r.normalize(); V.normalize();
	vector3 U = crossproduct(r, V);
	U.normalize();
	//get random numers 
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(0, 1);//uniform distribution between 0 and 1
	float e1 = dis(gen);
	float e2 = dis(gen);
	//get constants u, v
	float u = -(a / 2) + e1 * a;
	float v = -(a / 2) + e2 * a;

	vector3 r2 = r + (U * u) + (V * v);
	r2.normalize();

	Ray randomray;
	randomray.setray(positionhit, r2);
	
	vector3 reflectivecolor = calculatecolor(randomray, 1.5f);
	
	return reflectivecolor;
}

vector3 crossproduct(vector3 one, vector3 two)
{
	vector3 p;
	p.x = one.y * two.z - one.z * two.y;
	p.y = one.z * two.x - one.x * two.z;
	p.z = one.x * two.y - one.y * two.x;

	return p;
}
float clamp(float min, float max, float num)
{
	if (num < min)
	{
		num = min;
	}
	if (num > max)
	{
		num = max;
	}
	return num;
}
vector3 Refract(Ray theray, Shape& shape)//this function returns t
{
	vector3 t;

	float n1 = theray.getrefn();
	float n2 = shape.refn;
	vector3 d = theray.getdirection(); d.normalize();
	vector3 n = shape.calculatenormal(theray); n.normalize();
	float ddotn = d * n;

	if (ddotn < 0)//entering object, the direction is different than normal direction we need to make same because thats how the angle is given so switch normal
	{
		
	}
	else//leaving object the normal, t is sin(phi)b - ncos(phi), cos is negative because were using -n but now t is facing same direction as n so n needs to be positive 
	{
		n = -n;
	}

	float desc = 1 - ((pow(n1,2)*(1 - pow(d * n, 2))) / pow(n2,2));
	//cout << ((pow(n1, 2)*(1 - pow(d * n, 2))) / pow(n2, 2)) << endl;
	//cout << (pow(n1, 2)*(1 - pow(d * n, 2))) << endl;
	if (desc < 0)//angle is past critical point so its total reflection
	{
		return Reflect(theray, shape);
	}
	float cosone;
	vector3 partone;
	if (ddotn < 0)//entering object negative like normal
	{
		partone = ((d - n * (d * n))*n1) / n2;
		cosone = -(d * n);
	}
	else//leaving so dont negative the result of the dot product
	{
		partone = ((d + n * (d * n))*n1) / n2;
		cosone = d * n;
	}

	vector3 parttwo = n * sqrt(desc);
	t = partone - parttwo;
	vector3 oldts = shape.getcollisionpoint(theray);

	//calculate reflection color
	vector3 reflectioncolor = Reflect(theray, shape);
	
	//calculate transmiition color
	t.normalize();
	Ray refractray;

	vector3 positionvector = oldts;
	refractray.setray(positionvector, t);
	refractray.setrefn(shape.refn);

	vector3 refractioncolor = calculatecolor(refractray, 30.f, 0, 0);
	
	//find R 
	// Total internal reflection
	float R;

	fresnel(d, n, n1, n2, R);
	
	vector3 finalcolor = reflectioncolor * R + refractioncolor * (1 - R);
	finalcolor = refractioncolor;
	return finalcolor;
}

void fresnel(vector3 &I, vector3 &N, const float n1,float n2, float &kr)
{
	float cosi = clamp(-1, 1, (I*N));
	float etai = n1, etat = n2;
	if (cosi > 0) { std::swap(etai, etat); }
	// Compute sini using Snell's law
	float sint = etai / etat * sqrtf(std::max(0.f, 1 - cosi * cosi));
	// Total internal reflection
	if (sint >= 1) {
		kr = 1;
	}
	else {
		float cost = sqrtf(std::max(0.f, 1 - sint * sint));
		cosi = fabsf(cosi);
		float Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
		float Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
		kr = (Rs * Rs + Rp * Rp) / 2;
	}
	// As a consequence of the conservation of energy, transmittance is given by:
	// kt = 1 - kr;
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


/*
refraction code

if(objecthit == refractionobject)
{


}




*/
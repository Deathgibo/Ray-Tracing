#pragma once
#include "vector3.h"
#include "stb_image.h"
#include <vector>

class Texture {

public:
	Texture() { width = 0; height = 0;}

	void makeloadedtexture(const char* teximagepath)
	{
		unsigned char *texData = stbi_load(teximagepath, &width, &height, &bitDepth, 0);
		if (!texData)
		{
			printf("Failed to find: %s\n", teximagepath);
			return;
		}

		clearpixels();
		int counter = 0;
		std::cout << width << " " << height << "  " << bitDepth << std::endl;
		for (int i = 0; i < width; i++)
		{
			std::vector<vector3> tmp;
			pixels.push_back(tmp);
			for (int j = 0; j < height; j++)
			{
				vector3 yikes(0, 0, 0);
				pixels[i].push_back(yikes);
			}
		}
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				if (bitDepth == 3)
				{
					vector3 lol(texData[counter], texData[counter + 1], texData[counter + 2]);			
					counter += 3;
					pixels[j][i] = lol;
				}
				else if (bitDepth == 4)
				{
					vector3 lol(texData[counter], texData[counter + 1], texData[counter + 2]);
					counter += 4;
					pixels[j][i] = lol;
				}
			}
		}
		std::cout << counter << std::endl;


		stbi_image_free(texData);
	}
	void makecheckerboardtexture()
	{
		width = 110;
		height = 110;
		clearpixels();
		vector3 black(0, 0, 0);
		vector3 white(255, 255, 255);
		bool setblack = true;
		bool startblack = false;
		for (int i = 0; i < width; i++)
		{
			std::vector<vector3> tmp;
			if (i % 5 == 0)
			{
				startblack = !startblack;
			}
			if (startblack)
			{
				setblack = false;
			}
			else
			{
				setblack = true;
			}
			for (int j = 0; j < height; j++)
			{
				if (j % 5 == 0)
				{
					setblack = !setblack;
				}
				if (setblack)
				{
					tmp.push_back(black);
				}
				else
				{
					tmp.push_back(white);
				}
			}
			pixels.push_back(tmp);
		}
	}

	void maketwocolor()
	{
		width = 100;
		height = 100;
		clearpixels();
		vector3 black(0, 0, 0);
		vector3 white(255, 255, 255);
		vector3 red(255, 0, 0);
		vector3 blue(0, 0, 255);
		bool setblack = true;
		bool startblack = true;
		for (int i = 1; i < width + 1; i++)
		{
			std::vector<vector3> tmp;
			if (i % 51 == 0)
			{
				startblack = !startblack;
			}
			setblack = true;
			for (int j = 1; j < height + 1; j++)
			{
				if (startblack)
				{
					if (j % 50 == 0)
					{
						setblack = !setblack;
					}
					if (setblack)
					{
						tmp.push_back(black);
					}
					else
					{
						tmp.push_back(red);
					}
				}
				else//start white
				{
					if (j % 50 == 0)
					{
						setblack = !setblack;
					}
					if (setblack)
					{
						tmp.push_back(white);
					}
					else
					{
						tmp.push_back(blue);
					}
				}
			}
			pixels.push_back(tmp);
		}
	}

	void makecolortexture(int r, int g, int b)
	{
		width = 100;
		height = 100;
		clearpixels();
		for (int i = 0; i < width; i++)
		{
			std::vector<vector3> tmp;
			for (int j = 0; j < height; j++)
			{
				vector3 lol(r, g, b);
				tmp.push_back(lol);
			}
			pixels.push_back(tmp);
		}
	}

	void clearpixels()
	{
		for (int i = 0; i < pixels.size(); i++)
		{
			pixels[i].clear();
		}
		pixels.clear();
	}
	vector3 GetTextureColor(float u, float v)
	{
		vector3 thecolor(0, 0, 0);
		int x, y;

		x = u * (width-1);
		y = v * (height-1);

		thecolor = pixels[x][y];

		return thecolor;
	}
	bool texthere()
	{
		if (width != 0)
		{
			return true;	
		}
		return false;
	}

private:
	std::vector<std::vector<vector3>> pixels;
	int width;
	int height;
	int bitDepth;
};
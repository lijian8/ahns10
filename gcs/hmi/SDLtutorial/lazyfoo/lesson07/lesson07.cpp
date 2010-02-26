#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include <iostream>
#include <string>

using namespace std;

// Define Surfaces
SDL_Surface *screen = NULL;
SDL_Surface *background = NULL;
SDL_Surface *message = NULL;

// Define screen attributes
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;

// Define others
SDL_Event event;

//The font that's going to be used
TTF_Font *font = NULL;

//The colour of the font
SDL_Color textColor = {0, 0, 0};

bool init()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
	{
		return false;
	}

	screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE);

	if (screen == NULL)
	{
		return false;
	}

	if (TTF_Init() == -1)
	{
		return false;
	}

	SDL_WM_SetCaption("TTF Test", NULL);

	return true;
}

SDL_Surface *load_image(string filename)
{
	SDL_Surface *loadedImage = NULL;
	SDL_Surface *optimizedImage = NULL;

	loadedImage = IMG_Load(filename.c_str());

	if (loadedImage != NULL)
	{
		optimizedImage = SDL_DisplayFormat(loadedImage);
		SDL_FreeSurface(loadedImage);
	}

	return optimizedImage;
}

void apply_surface(int x, int y, SDL_Surface *source, SDL_Surface *destination)
{
	SDL_Rect offset;
	offset.x = x;
	offset.y = y;

	SDL_BlitSurface(source, NULL, destination, &offset);
}

bool load_files()
{
	background = load_image("../lesson08/fromSite/background.png");

	//Open the font
	//font = TTF_OpenFont("../lesson08/fromSite/lazy.ttf", 24);
	font = TTF_OpenFont("../../../FontLibrary/FuturaMedium.ttf", 24);

	if (background == NULL)
	{
		return false;
	}

	if (font == NULL)
	{
		return false;
	}

	return true;
}

void clean_up()
{
	SDL_FreeSurface(background);

	TTF_CloseFont(font);
	TTF_Quit();

	SDL_Quit();
}

int main(int argc, char *argv[])
{
	bool quit = false;

	if (!init())
	{
		return 1;
	}

	if (!load_files())
	{
		return 1;
	}

	message = TTF_RenderText_Solid(font, "The quickest brown fox jumps over the lazy hount", textColor);

	if (message == NULL)
	{
		return 1;
	}

	apply_surface(0, 0, background, screen);
	apply_surface(20, 200, message, screen);

	if (SDL_Flip(screen) == -1)
	{
		return 1;
	}

	while (quit == false)
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				quit = true;
			}
		}
	}

	clean_up();

	return 0;
}

#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_mixer.h"
#include "SDL/SDL_ttf.h"
#include "../include/lesson12.h"
#include <string>
#include <iostream>
#include <sstream>

SDL_Surface *load_image(std::string filename)
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

void declaration()
{
//	SCREEN_WIDTH = 640;
//	SCREEN_HEIGHT = 320;
//	SCREEN_BPP = 32;

	screen = NULL;
	background = NULL;
	message = NULL;
	seconds = NULL;

	font = NULL;
//	fontSize = 38;
//	textColor = {0xFF, 0xFF, 0xFF};
}

bool init()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) == -1)	return false;
	if (TTF_Init() == -1)				return false;

	screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE);
	if (screen == NULL)				return false;

	SDL_WM_SetCaption("Simple Timer", NULL);

	return true;
}

bool load_files()
{
	background = load_image("fromSite/background.png");
	font = TTF_OpenFont("fromSite/lazy.ttf", fontSize);

	if ((background == NULL) || (font == NULL))	return false;

	return true;
}

void clean_up()
{
	SDL_FreeSurface(background);
	SDL_FreeSurface(message);
	SDL_FreeSurface(seconds);
	SDL_FreeSurface(screen);
	TTF_CloseFont(font);

	TTF_Quit();
	SDL_Quit();

}

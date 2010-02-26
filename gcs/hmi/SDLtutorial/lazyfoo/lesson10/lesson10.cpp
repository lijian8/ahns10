#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include <string>
#include <iostream>

// Defining Screen properties
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;

// Defining Surfaces
SDL_Surface *screen = NULL;
SDL_Surface *background = NULL;
SDL_Surface *up = NULL;
SDL_Surface *down = NULL;
SDL_Surface *left = NULL;
SDL_Surface *right = NULL;
SDL_Surface *all = NULL;

// Defining everything else
SDL_Event event;
SDL_Color textColor = {0, 0, 0};
const short textSize = 48;
TTF_Font *font = NULL;

bool init()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) == -1)	return false;
	if (TTF_Init() == -1)
	{
		std::cerr << "ERROR: SDL could not be initialised" << std::endl;
		return false;
	}

	screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE);
	if (screen == NULL)
	{
		std::cerr << "ERROR: Screen could not be setup" << std::endl;
		return false;
	}

	SDL_WM_SetCaption("Key States - Without using events", NULL);

	return true;
}

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

bool load_files()
{
	background = load_image("fromSite/background.png");
	if (background == NULL)
	{
		std::cerr << "ERROR: Failed to load background.png" << std::endl;
		return false;
	}

	font = TTF_OpenFont("fromSite/lazy.ttf", textSize);
	if (font == NULL)
	{
		std::cerr << "ERROR: Failed to load lazy.ttf" << std::endl;
		return false;
	}

	return true;
}

void assign_text()
{
	up = TTF_RenderText_Solid(font, "Up", textColor);
	down = TTF_RenderText_Solid(font, "Down", textColor);
	left = TTF_RenderText_Solid(font, "Left", textColor);
	right = TTF_RenderText_Solid(font, "Right", textColor);
	all = TTF_RenderText_Solid(font, "ALL", textColor);
}

void clean_up()
{
	SDL_FreeSurface(screen);
	SDL_FreeSurface(background);
	SDL_FreeSurface(up);
	SDL_FreeSurface(down);
	SDL_FreeSurface(left);
	SDL_FreeSurface(right);
	SDL_FreeSurface(all);
	TTF_CloseFont(font);

	TTF_Quit();
	SDL_Quit();
}


// The MAIN function
int main(int argc, char *argv[])
{
	bool quit = false;

	if (init() == false)	return 1;
	if (load_files() == false)	return 1;

	assign_text();

	while (quit == false)
	{
		if (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				quit = true;
			}
		}

		apply_surface(0, 0, background, screen);

	//Get the keystates
		Uint8 *keystates = SDL_GetKeyState(NULL);
		if (keystates[SDLK_UP])
		{
			apply_surface((SCREEN_WIDTH - up->w)/2, (SCREEN_HEIGHT/2 - up->h)/2, up, screen);
		}

		if (keystates[SDLK_DOWN])
		{
			apply_surface((SCREEN_WIDTH - down->w)/2, (SCREEN_HEIGHT/2 - down->h)/2 + (SCREEN_HEIGHT/2), down, screen);
		}

		if (keystates[SDLK_LEFT])
		{
			apply_surface((SCREEN_WIDTH/2 - left->w)/2, (SCREEN_HEIGHT - left->h)/2, left, screen);
		}

		if (keystates[SDLK_RIGHT])
		{
			apply_surface((SCREEN_WIDTH/2 - right->w)/2 + (SCREEN_WIDTH/2), (SCREEN_HEIGHT - right->h)/2, right, screen);
		}

		if (keystates[SDLK_UP] & keystates[SDLK_DOWN] & keystates[SDLK_LEFT] & keystates[SDLK_RIGHT])
		{
			apply_surface((SCREEN_WIDTH - all->w)/2, (SCREEN_HEIGHT - all->h)/2, all, screen);
		}

		if (SDL_Flip(screen) == -1)
		{
			clean_up();
			return 1;
		}
	}

	clean_up();
	return 0;
}

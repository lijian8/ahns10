#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include <string>
#include <iostream>

using namespace std;

// Setup screen
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;

// Declare surfaces
SDL_Surface *screen = NULL;
SDL_Surface *dots = NULL;
SDL_Surface *background = NULL;

// Event structures
SDL_Event event;

//Portions of the sprite map to be blitted: different clippings - 4 dots
SDL_Rect clip[4];

// Load image
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

// Apply surface
void apply_surface(int x, int y, SDL_Surface *source, SDL_Surface *destination, SDL_Rect *clip = NULL)
{
	SDL_Rect offset;

	offset.x = x;
	offset.y = y;

	SDL_BlitSurface(source, clip, destination, &offset);
}

bool init()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
	{
		return false;
	}

	screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, NULL);

	if (screen == NULL)
	{
		return false;
	}

	SDL_WM_SetCaption("Clip Blitting and Sprite Sheets", NULL);

	return true;
}

void clip_rectangle()
{
	clip[0].x = 0;
	clip[0].y = 0;
	clip[0].w = 100;
	clip[0].h = 100;

	clip[1].x = 100;
	clip[1].y = 0;
	clip[1].w = 100;
	clip[1].h = 100;

	clip[2].x = 0;
	clip[2].y = 100;
	clip[2].w = 100;
	clip[2].h = 100;

	clip[3].x = 100;
	clip[3].y = 100;
	clip[3].w = 100;
	clip[3].h = 100;
}

void clean_up()
{
	SDL_FreeSurface(dots);
	SDL_Quit();
}

int main(int argc, char *argv[])
{
	bool quit = false;

	if (init() == false)
	{
		return 1;
	}

	dots = load_image("fromSite/dots.png");

	clip_rectangle();

	//Fill the screen white - Take 1st_arg surface, fill the region in 2nd_arg with colour in 3rd_arg 
	SDL_FillRect(screen, &screen->clip_rect, SDL_MapRGB(screen->format, 0xFF, 0xFF, 0xFF));

	//Apply the images to the screen
	apply_surface(0, 0, dots, screen, &clip[0]);
	apply_surface(540, 0, dots, screen, &clip[1]);
	apply_surface(0, 380, dots, screen, &clip[2]);
	apply_surface(540, 380, dots, screen, &clip[3]);

	if (SDL_Flip(screen) == -1)
	{
		return 1;
	}

	while (quit == false)
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)	quit = true;
		}
	}

	clean_up();

	return 0;
}

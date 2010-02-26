#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include <string>
#include <iostream>

using namespace std;

//Screen attributes
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;

//The surfaces
SDL_Surface *background = NULL;
SDL_Surface *foo = NULL;
SDL_Surface *screen = NULL;

//The event structure
SDL_Event event;

void apply_surface(int x, int y, SDL_Surface* source, SDL_Surface* destination)
{
	SDL_Rect offset;

	offset.x = x;
	offset.y = y;

	SDL_BlitSurface(source, NULL, destination, &offset);
}

bool init()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
	{
		return 1;
	}

	screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE);

	//If there is an error in setting up the screen, return 1
	if (screen == NULL)
	{
		return 1;
	}

	SDL_WM_SetCaption("Foo says \"Hello!\"", NULL);

	return true;
}

SDL_Surface *load_image(string filename)
{
	//The image that's loaded
	SDL_Surface* loadedImage = NULL;
	
	//Optimized image that will be used
	SDL_Surface* optimizedImage = NULL;

	//Load the image
	loadedImage = IMG_Load(filename.c_str());

	//If the image laoded
	if (loadedImage != NULL)
	{
		//Create optimized image
		optimizedImage = SDL_DisplayFormat(loadedImage);

		//Free the old image
		SDL_FreeSurface(loadedImage);

		//If the image was optimzied just fine
		if (optimizedImage != NULL)
		{
			//Map the colour key
			Uint32 colorkey = SDL_MapRGB(optimizedImage->format, 0, 0xFF, 0xFF); // Convert the RGB values to display format

			//Set all pixels of color R 0, G 0xFF, B 0xFF to be transparent
			SDL_SetColorKey(optimizedImage, SDL_SRCCOLORKEY, colorkey);
		}
	}

	return optimizedImage;
}

bool load_files()
{
	//Load the background image
	background = load_image("fromSite/background.png");

	//If background image could not load
	if (background == NULL)
	{
		return false;
	}

	//Load foo
	foo = load_image("fromSite/foo.png");

	//If foo did not load correctly
	if (foo == NULL)
	{
		return false;
	}

	return true;
}

//Clean up
void clean_up()
{
	SDL_FreeSurface(background);
	SDL_FreeSurface(foo);

	SDL_Quit();
}

int main(int argc, char* argv[])
{
	//Quit flag
	bool quit = false;

	//Initialization
	if (init() == false)
	{
		return 1;
	}

	//Load the files
	if (load_files() == false)
	{
		return 1;
	}

	//Apply surface
	apply_surface(0, 0, background, screen);
	apply_surface(190, 240, foo, screen);

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

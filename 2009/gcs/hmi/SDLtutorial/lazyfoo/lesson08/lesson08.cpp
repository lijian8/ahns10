#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include <iostream>
#include <string>

using namespace std;

// Define screen attibutes
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;

// Define surfaces
SDL_Surface *screen = NULL;
SDL_Surface *background = NULL;
SDL_Surface *message = NULL;
TTF_Font *font = NULL;

// MESSAGE SURFACE EXCLUSIVE TO THIS TUTORIAL
SDL_Surface *upMessage = NULL;
SDL_Surface *downMessage = NULL;
SDL_Surface *leftMessage = NULL;
SDL_Surface *rightMessage = NULL;

// Define other stuff
SDL_Event event;
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

	SDL_WM_SetCaption("Key Press", NULL);

	return true;
}

SDL_Surface *load_image(string filename)
{
	SDL_Surface *optimizedImage = NULL;
	SDL_Surface *loadedImage = NULL;

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

void clean_up()
{
	SDL_FreeSurface(screen);
	SDL_FreeSurface(background);
	SDL_FreeSurface(upMessage);
	SDL_FreeSurface(downMessage);
	SDL_FreeSurface(leftMessage);
	SDL_FreeSurface(rightMessage);

	SDL_Quit();
}

bool load_files()
{
	background = load_image("fromSite/background.png");
	font = TTF_OpenFont("fromSite/lazy.ttf", 28);

	if (background == NULL)	return false;
	if (font == NULL)	return false;

	return true;
}

int main(int argc, char *argv[])
{
	bool quit = false;

	if (!init())	return 1;
	if (!load_files())	return 1;
		
	upMessage = TTF_RenderText_Solid(font, "Up was pressed.", textColor);
	downMessage = TTF_RenderText_Solid(font, "Down was pressed.", textColor);
	leftMessage = TTF_RenderText_Solid(font, "Left was pressed.", textColor);
	rightMessage = TTF_RenderText_Solid(font, "Right was pressed.", textColor);
	if (upMessage == NULL)		return 1;
	if (downMessage == NULL)	return 1;
	if (leftMessage == NULL)	return 1;
	if (rightMessage == NULL)	return 1;

	while (quit == false)
	{
		//If there's an event to handle
		if (SDL_PollEvent(&event))
		{
			//If a key was pressed
			if (event.type == SDL_KEYDOWN)
			{
				//Set the proper message surface
				switch (event.key.keysym.sym)	// --1
				{
					case SDLK_UP:		message = upMessage; break;
					case SDLK_w:		message = upMessage; break;
					case SDLK_DOWN:		message = downMessage; break;
					case SDLK_s:		message = downMessage; break;
					case SDLK_LEFT:		message = leftMessage; break;
					case SDLK_a:		message = leftMessage; break;
					case SDLK_RIGHT:	message = rightMessage; break;
					case SDLK_d:		message = rightMessage; break;
					default: ;		// --2
				}
			}
			else if (event.type == SDL_QUIT)
			{
				//Quit the program
				quit = true;
			}
		}

		//If a message needs to be displayed
		if (message != NULL)
		{
			//Apply the images to the screen
			apply_surface(0, 0, background, screen);
			apply_surface((SCREEN_WIDTH - message->w)/2, (SCREEN_HEIGHT - message->w)/2, message, screen);

			message = NULL; // Clean it up after use - NULL the surface pointer
		}

		if (SDL_Flip(screen) == -1)	return 1;
	}

	clean_up();


	return 0;
}


/* EXPLANATION
 *
 * 1. 	SDL_PollEvent() puts the SDL_KEYDOWN data in the event structure as a SDL_KeyboardEvent named "key";
 *	Inside of the "key" is a keysym structure;
 *	Inside of the "keysym" is the SDL_Key named "sym", which is which key was pressed.
 *	If the up arrow was pressed, the sym will be SDLK_UP and we'll set the message to be up,
 *	if the down arrow was pressed, the sym will be SDLK_DOWN and we'll set the message to be down, etc, etc.
 * 2.	"default: ;" is used when not all keys are assigned, when other keys being pressed, it will go to this line.
 */

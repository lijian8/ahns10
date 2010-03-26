#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include <iostream>
#include <string>

using namespace std;

// Global definitions
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;

SDL_Surface *screen = NULL;
SDL_Surface *buttonSheet = NULL;
TTF_Font *font = NULL;
SDL_Event event;
SDL_Color textColor = {0, 0, 0};
SDL_Rect clips[4];
const int CLIP_MOUSEOVER = 0, CLIP_MOUSEOUT = 1, CLIP_MOUSEDOWN = 2, CLIP_MOUSEUP = 3;

// Function Implementation
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

void apply_surface(int x, int y, SDL_Surface *source, SDL_Surface *destination, SDL_Rect *clip = NULL)
{
	SDL_Rect offset;

	offset.x = x;
	offset.y = y;

	SDL_BlitSurface(source, clip, destination, &offset);
}

bool load_files()
{
	buttonSheet = load_image("fromSite/button.png");

	if (buttonSheet == NULL)	return false;

	return true;
}

bool init()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) == -1)	return false;

	screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE);
	if (screen == NULL)	return false;

	if (TTF_Init() == -1)	return false;

	SDL_WM_SetCaption("Mouse Event", NULL);

	return true;
}

void clean_up()
{
	SDL_FreeSurface(buttonSheet);
	SDL_FreeSurface(screen);
	TTF_CloseFont(font);

	TTF_Quit();
	SDL_Quit();
}

//The BUTTON class
class Button
{
public:
	Button(int x, int y, int w, int h); //Initialize variables
	void handle_events(); //Handles events and set the button's sprite region
	void show(); //Shows the button on the screen

private:
	SDL_Rect box; //The attributes of the button
	SDL_Rect *clip; //The part of the button sprite shee that will be shown
};

void set_clips()
{
	//Clip the sprite sheet
	clips[CLIP_MOUSEOVER].x = 0;
	clips[CLIP_MOUSEOVER].y = 0;
	clips[CLIP_MOUSEOVER].w = 320;
	clips[CLIP_MOUSEOVER].h = 240;

	clips[CLIP_MOUSEOUT].x = 320;
	clips[CLIP_MOUSEOUT].y = 0;
	clips[CLIP_MOUSEOUT].w = 320;
	clips[CLIP_MOUSEOUT].h = 240;

	clips[CLIP_MOUSEDOWN].x = 0;
	clips[CLIP_MOUSEDOWN].y = 240;
	clips[CLIP_MOUSEDOWN].w = 320;
	clips[CLIP_MOUSEDOWN].h = 240;

	clips[CLIP_MOUSEUP].x = 320;
	clips[CLIP_MOUSEUP].y = 240;
	clips[CLIP_MOUSEUP].w = 320;
	clips[CLIP_MOUSEUP].h = 240;
}

Button::Button(int x, int y, int w, int h)
{
	//Set the button's attriubtes
	box.x = x;
	box.y = y;
	box.w = w;
	box.h = h;

	clip = &clips[CLIP_MOUSEOUT]; //Set the default sprite
}

void Button::handle_events()
{
	//The mouse offsets
	int x = 0, y = 0;

	//If the mouse moved
	if (event.type == SDL_MOUSEMOTION)
	{
		//Get the mouse offsets
		x = event.motion.x;
		y = event.motion.y;

		//If the mouse is over the button
		if ((x > box.x) && (x < box.x+box.w) && (y > box.y) && (y < box.y+box.h))
		{
			//Set the button sprite
			clip = &clips[CLIP_MOUSEOVER];
		}
		//If not
		else
		{
			//Set the button sprite
			clip = &clips[CLIP_MOUSEOUT];
		}
	}

	//If a mouse button was pressed
	if (event.type == SDL_MOUSEBUTTONDOWN)
	{
		//cout << "SDL_GetMouseState(): " << (int)SDL_GetMouseState(NULL, NULL) << endl;

		//If the left mouse button was pressed
		if (event.button.button == SDL_BUTTON_LEFT)
		{
			//Get the mouse offsets
			x = event.button.x;
			y = event.button.y;

			//If the mouse is over the button
			if ((x > box.x) && (x < box.x+box.w) && (y > box.y) && (y < box.y+box.h))
			{
				//Set the button sprite
				clip = &clip[CLIP_MOUSEDOWN];
			}
		}
	}

	//If a mouse button was released
	if (event.type == SDL_MOUSEBUTTONUP)
	{
		//If the left mouse button was released
		if (event.button.button == SDL_BUTTON_LEFT)
		{
			//Get the mouse position
			x = event.button.x;
			y = event.button.y;

			//If the mouse is over the button
			if ((x > box.x) && (x < box.x+box.w) && (y > box.y) && (y < box.y+box.h))
			{
				//Set the button sprite
				clip = &clip[CLIP_MOUSEUP];
			}
		}
	}
}

void Button::show()
{
	//Show the button
	apply_surface(box.x, box.y, buttonSheet, screen, clip);
}

int main(int argc, char *argv[])
{
	bool quit = false;

	if (!init())		return 1;
	if (!load_files())	return 1;

	//Clip the sprite sheet
	set_clips();

	//Make the button
	Button myButton(170, 120, 320, 240);

	while (quit == false)
	{
		if (SDL_PollEvent(&event))
		{
			//Handle button events
			myButton.handle_events();

			//If the user has Xed out the window
			if (event.type == SDL_QUIT)
			{
				quit = true;
			}
		}

		//Fill the screen white
		SDL_FillRect(screen, &screen->clip_rect, SDL_MapRGB(screen->format, 0, 0, 0));

		//Show the button
		myButton.show();

		//Update the screen
		if (SDL_Flip(screen) == -1)	return 1;
	}

	return 0;
}

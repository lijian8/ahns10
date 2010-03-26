// This is only a mockup of the artificial horizon, working out the dimension
// 
// Brendan Chen
// 29 Jun 2009

#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include <string>
#include <iostream>

using namespace std;

// Set up the screen
const int SCREEN_WIDTH = 383;
const int SCREEN_HEIGHT = 500;
const int SCREEN_BPP = 32;

// Set up surfaces
SDL_Surface *screen = NULL;
SDL_Surface *optimizedImage = NULL;
SDL_Surface *componentAH = NULL;
SDL_Surface *componentMAP = NULL;
SDL_Surface *componentANN = NULL;
SDL_Surface *dispText = NULL;

// Set up clipping space
SDL_Rect img_annBar;
SDL_Rect img_topAH;
SDL_Rect img_bottomAH;
SDL_Rect img_annWarning;
SDL_Rect img_annCaution;
SDL_Rect img_annOFF;

// Set up event action
SDL_Event event;

// Set up other attributes
SDL_Color textColour = {255, 164, 0};
TTF_Font *fontFuturaM = NULL;
TTF_Font *fontLucidaR = NULL;
const short fontSize = 11;
const short textSepSize = 2;
const int delayTime = 800;

SDL_Surface *load_image(string filename)
{
	SDL_Surface *loadedImage = IMG_Load(filename.c_str());
	optimizedImage = SDL_DisplayFormat(loadedImage);
	SDL_FreeSurface(loadedImage);

	return optimizedImage;
}

void Extract_ComponentAH()
{
	img_annBar.x = 0;
	img_annBar.y = 0;
	img_annBar.w = 60;
	img_annBar.h = 300;

	img_topAH.x = 60;
	img_topAH.y = 0;
	img_topAH.w = 323;
	img_topAH.h = 150;

	img_bottomAH.x = 60;
	img_bottomAH.y = 150;
	img_bottomAH.w = 323;
	img_bottomAH.h = 150;
}

void Extract_ComponentANN()
{
	img_annWarning.x = 0;
	img_annWarning.y = 0;
	img_annWarning.w = 60;
	img_annWarning.h = 25;

	img_annCaution.x = 0;
	img_annCaution.y = 25;
	img_annCaution.w = 60;
	img_annCaution.h = 25;

	img_annOFF.x = 0;
	img_annOFF.y = 0;
	img_annOFF.w = 60;
	img_annOFF.h = 27;
}

bool init()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
	{
		return false;
	}

	screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE);

	if (TTF_Init() == -1)	return false;

	if (screen == NULL)
	{
		return false;
	}

	SDL_WM_SetCaption("mockupAH - Testing AH", NULL);

	//annDim.x = 64; // +-2 pixels either side
	//annDim.y = 27; // +-1 pixel top and bottom
	
	//cerr << "ERROR" << endl;
	//SDL_FillRect(annOFF, annDim, SDL_MapRGB(screen->format, 0, 0, 0));

	return true;
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
	componentAH = load_image("images/Component_AH.jpg");
	if (componentAH == NULL)
	{
		return false;
	}

	componentMAP = load_image("images/Component_MAP_Background.jpg");
	if (componentMAP == NULL)
	{
		return false;
	}

	componentANN = load_image("images/Component_Annunciators.jpg");
	if (componentANN == NULL)
	{
		return false;
	}

	fontFuturaM = TTF_OpenFont("../../FontLibrary/FuturaMedium.ttf", fontSize);
	if (fontFuturaM == NULL)
	{
		cerr << "ERROR: Cannot load font FuturaMedium.ttf" << endl;
		return false;
	}

	fontLucidaR = TTF_OpenFont("../../FontLibrary/LucidaSansTypewriter_REGULAR.ttf", fontSize);
	if (fontLucidaR == NULL)
	{
		cerr << "ERROR: Cannot load font LucidaSansTypewriter_REGULAR.ttf" << endl;
		return false;
	}
	
	return true;
}

void Display_Components()
{
	apply_surface(60, 0-50, componentAH, screen, &img_topAH); // Top segment of AH
	apply_surface(60, 150-50, componentAH, screen, &img_bottomAH); // Bottom segment of AH
	apply_surface(0, 0, componentAH, screen, &img_annBar); // Annunciator Side (left)
	apply_surface(323, 0, componentAH, screen, &img_annBar); // Annunciator Side (right)
	apply_surface(10, 0, componentAH, screen, &img_annBar); // Annunciator Side (left) - extended
	apply_surface(313, 0, componentAH, screen, &img_annBar); // Annunciator Side (right) - extended

	// Applying bottom side of the AH
	apply_surface(0, 200, componentMAP, screen, NULL);
}

void clean_up()
{
	SDL_FreeSurface(screen);
	SDL_FreeSurface(componentAH);
	SDL_FreeSurface(componentANN);
	SDL_FreeSurface(componentMAP);
	TTF_CloseFont(fontFuturaM);
	SDL_FreeSurface(dispText);
}

bool detect_Xed()
{
	if (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			clean_up();
			return true;
		}
	}

	return false;
}

int flicker_annWarning()
{
	apply_surface(318, 0, componentAH, screen, &img_annOFF);
	if (detect_Xed()) return 1;
	SDL_Delay(delayTime);
	SDL_Flip(screen);
	apply_surface(318, 1, componentANN, screen, &img_annWarning);
	apply_surface(5, 205, dispText, screen, NULL);
	if (detect_Xed()) return 1;
	SDL_Delay(delayTime);
	SDL_Flip(screen);

	return 0;
}

int flicker_annCaution()
{
	apply_surface(318, 26, componentAH, screen, &img_annOFF);
	if (detect_Xed()) return 1;
	SDL_Delay(delayTime);
	SDL_Flip(screen);
	apply_surface(318, 27, componentANN, screen, &img_annCaution);
	apply_surface(5, 205+fontSize+textSepSize, dispText, screen, NULL);
	if (detect_Xed()) return 1;
	SDL_Delay(delayTime);
	SDL_Flip(screen);

	return 0;
}

int main(int argc, char *argv[])
{
	bool quit = false;

	if (init() != true)
	{
		return 1;
	}

	//SDL_FillRect(screen, &screen->clip_rect, SDL_MapRGB(screen->format, 128, 128, 128));

	if (load_files() == false)
	{
		return 1;
	}

	Extract_ComponentAH();
	Extract_ComponentANN();
	Display_Components();

	dispText = TTF_RenderText_Solid(fontLucidaR, "MASTER WARNING", textColour);
	//apply_surface(5, 205, dispText, screen, NULL);
	for (int i=0; i < 3; i++)
	{
		if (flicker_annWarning())
		{
			SDL_Quit();
			return 0;
		}
	}
	apply_surface(318, 1, componentANN, screen, &img_annWarning);

	dispText = TTF_RenderText_Solid(fontLucidaR, "MASTER CAUTION", textColour);
	for (int j=0; j < 3; j++)
	{
		if(flicker_annCaution())
		{
			SDL_Quit();
			return 0;
		}
	}
	apply_surface(318, 27, componentANN, screen, &img_annCaution);

	// Flip the final image
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
	SDL_Quit();

	return 0;
}

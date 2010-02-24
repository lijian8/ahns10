#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_mixer.h"
#include "SDL/SDL_ttf.h"
#include <iostream>
#include <string>

// Define screen attributes
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;

// Defining surfaces
SDL_Surface *background = NULL;
SDL_Surface *screen = NULL;
SDL_Surface *message = NULL;

// Defining other attributes
SDL_Event event;
const int fontSize = 32;
SDL_Color textColor = {0, 0, 0};
TTF_Font *font = NULL;

//The music that will be played
Mix_Music *music = NULL;

//The sound effects that will be used
Mix_Chunk *scratch = NULL;
Mix_Chunk *high = NULL;
Mix_Chunk *med = NULL;
Mix_Chunk *low = NULL;

void apply_surface(int x, int y, SDL_Surface *source, SDL_Surface *destination)
{
	SDL_Rect offset;

	offset.x = x;
	offset.y = y;

	SDL_BlitSurface(source, NULL, destination, &offset);
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

bool init()
{
	//Initialise all SDL subsystems
	if (SDL_Init(SDL_INIT_EVERYTHING) == -1)					return false;

	screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE);
	if (screen == NULL)								return false;

	if (TTF_Init() == -1)								return false;

	if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1)			return false;

	SDL_WM_SetCaption("Monitor Music", NULL);

	return true;
}

bool load_files()
{
	background = load_image("fromSite/background.png");
	if (background == NULL)								return false;

	font = TTF_OpenFont("fromSite/lazy.ttf", fontSize);
	if (font == NULL)								return false;

	//Load the music
	//music = Mix_LoadMUS("fromSite/beat.wav");
	music = Mix_LoadMUS("../../../SoundFiles/navionics.wav");
	if (music == NULL)								return false;

	//Load the sound effects
	//scratch = Mix_LoadWAV("fromSite/scratch.wav");
	//high = Mix_LoadWAV("fromSite/high.wav");
	//med = Mix_LoadWAV("fromSite/medium.wav");
	//low = Mix_LoadWAV("fromSite/low.wav");
	scratch = Mix_LoadWAV("../../../SoundFiles/nbankangle.wav");
	high = Mix_LoadWAV("../../../SoundFiles/nglideslope.wav");
	med = Mix_LoadWAV("../../../SoundFiles/nretard.wav");
	low = Mix_LoadWAV("../../../SoundFiles/nsinkrate.wav");
	if ((scratch == NULL) || (high == NULL) || (med == NULL) || (low == NULL))	return false;

	return true;
}

void clean_up()
{
	SDL_FreeSurface(background);
	Mix_FreeChunk(scratch);
	Mix_FreeChunk(high);
	Mix_FreeChunk(med);
	Mix_FreeChunk(low);
	Mix_FreeMusic(music);

	TTF_CloseFont(font);
	Mix_CloseAudio();
	TTF_Quit();
	SDL_Quit();
}

bool assign_text()
{
	message = TTF_RenderText_Solid(font, "The quick brown fox jumps over the lazy hound", textColor);
	if (message == NULL)								return false;

	return true;
}

int main(int argc, char *argv[])
{
	bool quit = false;

	if (!init())		return 1;
	if (!load_files())	return 1;

	while (!quit)
	{
		if (SDL_PollEvent(&event))
		{
			apply_surface(0, 0, background, screen);
			//If a key was pressed
			if (event.type == SDL_KEYDOWN)
			{
				//If 1 was pressed
				if (event.key.keysym.sym == SDLK_1)
				{
					if (Mix_PlayChannel(-1, scratch, 0) == -1)	return 1;
				}
				//If 2 was pressed
				else if (event.key.keysym.sym == SDLK_2)
				{
					if (Mix_PlayChannel(-1, high, 0) == -1)	return 1;
				}
				//If 3 was pressed
				else if (event.key.keysym.sym == SDLK_3)
				{
					if (Mix_PlayChannel(-1, med, 0) == -1)	return 1;
				}
				//If 4 was pressed
				else if (event.key.keysym.sym == SDLK_4)
				{
					if (Mix_PlayChannel(-1, low, 0) == -1)	return 1;
				}
				//If 9 was pressed
				else if (event.key.keysym.sym == SDLK_9)
				{
					//If there is no music playing
					if (Mix_PlayingMusic() == 0)
					{
						//Play the music
						if (Mix_PlayMusic(music, -1) == -1)	return 1;
					}
					//If the music is being played
					else
					{
						//If the music is paused
						if (Mix_PausedMusic() == 1)	Mix_ResumeMusic();
						else				Mix_PauseMusic();
					}
				}
				else if (event.key.keysym.sym == SDLK_0)
				{
					//Stop the music
					Mix_HaltMusic();
				}
			}

			if (event.type == SDL_QUIT)	quit = true;

			if (SDL_Flip(screen) == -1)	return 1;
		}
	}

	clean_up();
	return 0;
}

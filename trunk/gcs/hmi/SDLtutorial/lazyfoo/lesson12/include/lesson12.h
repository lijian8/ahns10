#ifndef _LESSON12_H_
#define _LESSON12_H_

// Includes
#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_mixer.h"
#include <string>

// Define Screen Attributes
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define SCREEN_BPP 32

// Define Surfaces
SDL_Surface *screen;
SDL_Surface *background;
SDL_Surface *message;
SDL_Surface *seconds;

// Define all other attributes
SDL_Event event;
TTF_Font *font;
#define fontSize 38
const SDL_Color textColor = {0xFF, 0xFF, 0xFF};

// Function Prototypes
SDL_Surface *load_image(std::string filename);
void apply_surface(int x, int y, SDL_Surface *source, SDL_Surface *destination);
void declaration();
bool init();
bool load_files();
void clean_up();

#endif

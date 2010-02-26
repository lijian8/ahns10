// HELLO WORLD SDL STYLE

//The headers
#include "SDL/SDL.h"

//The attributes of the screen
const int SCREEN_WIDTH = 640; // Screen Width
const int SCREEN_HEIGHT = 480; // Screen Height
const int SCREEN_BPP = 32; // Bits per pixel: currently 32-bit colour is used

//The surfaces that will be used
SDL_Surface *message = NULL; // Background image
SDL_Surface *marker = NULL;
SDL_Surface *mask = NULL;
SDL_Surface *background = NULL; // Bitmap that says "Hello World"
SDL_Surface *screen = NULL; // The screen/monitor
// ALWAYS GOOD IDEA TO SET POINTERS TO 'NULL' IF THEY ARE NOT POINTING TO ANYTHING

SDL_Surface *load_image(string filename) // The argument "filename" is path of image to be loaded

// Surface Blitting Function
void apply_surface(int x, int y, SDL_Surface* source, SDL_Surface* destination)
// PRE: Position of where to blit the surface
//	Surface you are going to blit
//	Surface you are blitting on
	//Wait (display) for 2 seconds

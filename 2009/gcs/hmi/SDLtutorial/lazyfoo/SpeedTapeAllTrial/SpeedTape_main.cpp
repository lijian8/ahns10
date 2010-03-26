// HELLO WORLD SDL STYLE

//The headers
#include "SDL/SDL.h"
#include "SpeedTape.h"
#include "iostream"
#include <string>

using namespace std;

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
{
	//Temporary storage for the image that's loaded
	SDL_Surface* loadedImage = NULL; // The surface when image is loaded

	//The optimized image that will be used
	SDL_Surface* optimizedImage = NULL; // Surface that is to be used

	//Load the image
	loadedImage = SDL_LoadBMP(filename.c_str()); // Function used to load image

	//If nothing went wrong in loading the image
	if (loadedImage == NULL)
		cerr << "Image not loaded!" << endl;
	else
	{
		//Create an optimized image
		optimizedImage = SDL_DisplayFormat(loadedImage); // Create new version of "loadedImage" in same format as screen

		//Free (get rid of) the old image
		SDL_FreeSurface(loadedImage);
	}
	
	//Return the optimized image
	return optimizedImage;
}


// Surface Blitting Function
void apply_surface(int x, int y, SDL_Surface* source, SDL_Surface* destination)
// PRE: Position of where to blit the surface
//	Surface you are going to blit
//	Surface you are blitting on
{
	//Make a temporary rectangle to hold the offsets
	SDL_Rect offset; //A data type of a rectangle, 4 members: X & Y offsets, and height of rectangle

	//Give the offsets to the rectangle
	offset.x = x;
	offset.y = y;
	//BlitSurface function only accepts offsets inside the SDL_Rect data members

	//Blit the surface (Surface we're using, NULL, surface we're blitting on, position of the desintation of source)
	SDL_BlitSurface(source, NULL, destination, &offset);
}

int main(int argc, char* argv[])
{
	//Initialise all SDl subsystems
	if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
	{
		cerr << "System could not successfully initialise all the SDL subsystems." << endl;
		return 1; // Start up every SDl subsystem.
	}

	//Set up the screen
	screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE); // 4th creates screen surface in system memory

	//If there was an error in setting up the screen
	if (screen == NULL)
	{
		cerr << "Screen could not successfully be set up." << endl;
		return 1;
	}

	int xDir = -612;

	//Set the window caption
	SDL_WM_SetCaption("Speedtape Mockup", NULL);

	//Load the image
	message = load_image("AH_Files/speedTapeMockup.bmp");
	marker = load_image("AH_Files/markerMockup.bmp");
	background = load_image("background.bmp");
	mask = load_image("AH_Files/rect_strip.bmp");

for (int a = 0; a <= 206; a++)
{
	//Apply the background to the screen (offset x, offset y, source, destination)
	apply_surface(0, 0, background, screen); // This is our own function

	//Apply the marker to the screen
	apply_surface(20, 116, marker, screen);

	//Apply the message to the screen
	apply_surface(xDir, 140, message, screen); //(offset x, offset y, source, destination)

	//Apply the mask to filter out other numbers
	apply_surface(10-300, 140, mask, screen);
	apply_surface(45, 140, mask, screen);
	apply_surface(345, 140, mask, screen);

	//Update the screen
	if (SDL_Flip(screen) == -1)
	{
		cerr << "Screen in memory do not match screen we see." << endl;
		return 1;
	}

	if (a == 0) SDL_Delay(2000);

	if (a == 206) SDL_Delay(2000);

	//Wait (display) for 2 seconds
	SDL_Delay(20);

	xDir += 3;
}

	//Free the surfaces
	SDL_FreeSurface(message);
	SDL_FreeSurface(marker);
	SDL_FreeSurface(background);

	//Quit SDL
	SDL_Quit();

	return 0;
}



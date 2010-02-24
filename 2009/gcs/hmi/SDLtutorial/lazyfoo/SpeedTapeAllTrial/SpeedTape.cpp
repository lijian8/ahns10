// HELLO WORLD SDL STYLE

//The headers
#include "SDL/SDL.h"
#include "SpeedTape.h"
#include "iostream"
#include <string>

using namespace std;

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

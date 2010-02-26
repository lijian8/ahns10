#include "../include/lesson12.h"
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include <iostream>
#include <string>
#include <sstream>

int main(int argc, char *args[])
{
	bool quit = false;

	declaration();
	if (!init())		return 1;
	if (!load_files())	return 1;

	//The timer starting time
	Uint32 start = 0;

	//The timer start/stop flag
	bool running = true;

	//Start the timer
	start = SDL_GetTicks(); // <--- Starts the timer

	//While the user hasn't quit
	while (!quit)
	{
		if (SDL_PollEvent(&event))
		{
			//If a key was pressed
			if (event.type == SDL_KEYDOWN)
			{
				// If that key was s
				if (event.key.keysym.sym == SDLK_s)
				{
					if (running)
					{
						//Stop the timer
						running = false;
						start = 0;
					}
					else
					{
						//Start the timer
						running = true;
						start = SDL_GetTicks();
					}
				}
			}

			apply_surface(0, 0, background, screen);
			message = TTF_RenderText_Solid(font, "Press S to start or stop the timer", textColor);
			if (message == NULL)	return 1;
			apply_surface((SCREEN_WIDTH - message->w)/2, (SCREEN_HEIGHT - message->h)/2, message, screen);

			//If the timer is running
			if (running == true)
			{
				//The timer's time as a strong
				std::stringstream time;

				//Convert the timer's time to a string
				time << "Timer: " << SDL_GetTicks() - start;

				//Render the time surface
				seconds = TTF_RenderText_Solid(font, time.str().c_str(), textColor);

				//Apply the time surface
				apply_surface((SCREEN_WIDTH - seconds->w)/2, 50, seconds, screen);

				//Free the time surface
				SDL_FreeSurface(seconds);
			}

			//Update the screen
			if (SDL_Flip(screen) == -1)	return 1;

			if (event.type == SDL_QUIT)	quit = true;
		}
	}

	clean_up();

	return 0;
}

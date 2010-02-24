#include "pracSetup.h"
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char* argv[])
{
	char* inMsg = NULL;

	cin >> inMsg >> endl;
	DisplayHello(inMsg);

	return 0;
}

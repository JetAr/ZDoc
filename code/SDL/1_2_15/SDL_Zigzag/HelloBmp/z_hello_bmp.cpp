#include ".\..\..\Common\z_sdl.h"

//Include SDL functions and datatypes
int main(int argc, char* args[])
{
	//The images
	SDL_Surface* hello = NULL;
	SDL_Surface* screen = NULL;

	//Start SDL
	SDL_Init(SDL_INIT_EVERYTHING);

	//Set up screen
	screen = SDL_SetVideoMode(640, 480, 32, SDL_SWSURFACE);

	//Load image
	hello = SDL_LoadBMP("hello.bmp");

	//Apply image to screen
	SDL_BlitSurface(hello, NULL, screen, NULL);

	//Update Screen
	SDL_Flip(screen);

	//Pause
	SDL_Delay(1500);

	//Free the loaded image
	SDL_FreeSurface(hello);

	//Quit SDL
	SDL_Quit();

	return 0;
}

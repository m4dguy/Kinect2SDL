#include <SDL.h>

#include "BasicKinect2Grabber.h"
#include "UtilsKinect.h"

#define COLOR_MODE 0
#define DEPTH_MODE 1

int main(int argc, char *argv[]){
	// choose mode
	bool mode = COLOR_MODE;
	if (argc > 1)
		mode = !strcmp(argv[1], "depth");

	// initialize Kinect
	BasicKinect2Grabber grabber;
	if (!grabber.isInitialized()){
		printf("Kinect could bot be initialized!\n");
		return 1;
	}

	// initialize SDL
	if(SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return 1;
	}

	const int SCREEN_WIDTH  = grabber.getColourImageWidth();
	const int SCREEN_HEIGHT = grabber.getColourImageHeight();

	const int DEPTH_WIDTH  = grabber.getDepthImageWidth();
	const int DEPTH_HEIGHT = grabber.getDepthImageHeight();
		
	//init resources
	SDL_Window* window;
	if (mode == COLOR_MODE)
		window = SDL_CreateWindow("Kinect Color Stream", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
	else
		window = SDL_CreateWindow("Kinect Depth Stream", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, DEPTH_WIDTH, DEPTH_HEIGHT, SDL_WINDOW_OPENGL);

	SDL_Surface* screenSurface = SDL_GetWindowSurface(window);
	SDL_Surface* kdata = new SDL_Surface(*screenSurface);

	bool quit = 0;
	SDL_Event event;
	while (!quit){

		if (!grabber.update())
			continue;

		if (mode == COLOR_MODE)
			kdata->pixels = grabber.getColourImage();
		else
			UtilsKinect::compressDepth(grabber.getDepthImage(), kdata->pixels, DEPTH_WIDTH, DEPTH_HEIGHT);

		//rendering
		SDL_BlitSurface(kdata, NULL, screenSurface, NULL);
		SDL_UpdateWindowSurface(window);

		//process event
		quit = false;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				quit |= true;
			}
			if (event.type == SDL_KEYDOWN){
				quit |= true;
			}
		}
	}

	// tear down
	SDL_DestroyWindow(window);
	SDL_FreeSurface(kdata);
	SDL_Quit();

	return 0;
}
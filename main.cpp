#include <SDL.h>

#include "BasicKinect2Grabber.h"
#include "UtilsKinect.h"

#define COLOR_MODE 0
#define DEPTH_MODE 1
#define IR_MODE 2

int main(int argc, char *argv[]){
	// choose mode
	int mode = COLOR_MODE;
	if (argc > 1){
		if (!strcmp(argv[1], "depth"))
			mode = DEPTH_MODE;

		if (!strcmp(argv[1], "ir"))
			mode = IR_MODE;
	}
	
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

	const int IR_WIDTH = grabber.getIRImageWidth();
	const int IR_HEIGHT = grabber.getIRImageHeight();
		
	//init resources
	SDL_Window* window;
	switch (mode){
		case COLOR_MODE:
			window = SDL_CreateWindow("Kinect Color Stream", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
			break;
		case DEPTH_MODE:
			window = SDL_CreateWindow("Kinect Depth Stream", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, DEPTH_WIDTH, DEPTH_HEIGHT, SDL_WINDOW_OPENGL);
			break;
		case IR_MODE:
			window = SDL_CreateWindow("Kinect Infrared Stream", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, IR_WIDTH, IR_HEIGHT, SDL_WINDOW_OPENGL);
			break;
	}

	SDL_Surface* screenSurface = SDL_GetWindowSurface(window);
	SDL_Surface* kdata = new SDL_Surface(*screenSurface);

	// main loop
	bool quit = 0;
	SDL_Event event;
	while (!quit){
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

		// get kinect data
		if (!grabber.update())
			continue;
		
		switch (mode){
			case COLOR_MODE:
				kdata->pixels = grabber.getColourImage();
				break;
			case DEPTH_MODE:
				UtilsKinect::cutoffRescale(grabber.getDepthImage(), kdata->pixels, DEPTH_WIDTH, DEPTH_HEIGHT, 2500); // cutoff roughly corresponds to distance of pixel in mm
				break;
			case IR_MODE:
				UtilsKinect::cutoffRescale(grabber.getIRImage(), kdata->pixels, IR_WIDTH, IR_HEIGHT, 4096);
				break;
		}

		//rendering
		SDL_BlitSurface(kdata, NULL, screenSurface, NULL);
		SDL_UpdateWindowSurface(window);
	}

	// tear down
	SDL_DestroyWindow(window);
	SDL_FreeSurface(kdata);
	SDL_Quit();

	return 0;
}
#pragma once
#include <SDL.h>
#include <string>
#include <vector>
#include "gfx.hpp"
#include "dpad.hpp"
using namespace std;

struct SDLmanager {
	const string gamename = "My Game";
	const int SCREEN_WIDTH  = 640;
	const int SCREEN_HEIGHT = 480;
	const uint32_t TRANSPARENT_COLOR = 0xffff00ff;
	// const int SCREEN_SCALE  = 1;

	bool quit = false;
	SDL_Window* window = NULL;
	SDL_Surface* screenSurface = NULL;  // The surface contained by the window
	SDL_Surface* buffer = NULL;
	int _framecount = 0, _framelast = 0;
	char fps[10] = "fps";
	uint64_t framestart = 0, frameend = 0;

	SDLmanager() {}
	SDLmanager(const string& name, int width, int height) : gamename(name), SCREEN_WIDTH(width), SCREEN_HEIGHT(height) {}

	int init() {
		// Initialize SDL
		if (SDL_Init( SDL_INIT_VIDEO ) < 0) {
			printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
			return -1;
		}
		// create window
		window = SDL_CreateWindow( gamename.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN  );
		if (window == NULL) {
			printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
			return -1;
		}
		// Get window surface
		screenSurface = SDL_GetWindowSurface( window );
		if (window == NULL) {
			printf( "Window surface could not be created! SDL_Error: %s\n", SDL_GetError() );
			return -1;
		}
		// misc
		quit = false;
		// OK
		return 0;
	}

	int close() {
		// destroy everything
		SDL_FreeSurface(buffer);
		SDL_DestroyWindow(window);
		window = NULL;
		screenSurface = NULL;  // don't free this (according to API docs)
		buffer = NULL;
		// Quit SDL subsystems
		SDL_Quit();
		return 0;
	}

	int makebmp(const string& fname) {
		auto* bmp = SDL_LoadBMP(fname.c_str());
		if (bmp == NULL) {
			printf( "Could not load bitmap \"%s\"! SDL_Error: %s\n", fname.c_str(), SDL_GetError() );
			return 0;
		}
		if (bmp->format->BitsPerPixel != 24) {
			printf( "Error on BMP load - expected 24-bit bitmap\n" );
			SDL_FreeSurface(bmp);
			return 0;
		}
		int ptr = GFX::makeimagegl(bmp->w, bmp->h);
		auto& img = GFX::getimagegl(ptr);
		uint8_t* data = (uint8_t*)bmp->pixels;
		// convert BGR -> ARGB
		for (int i = 0; i < bmp->w * bmp->h; i++) {
			uint32_t col = 0xff000000 | (data[i*3] << 0) | (data[i*3+1] << 8) | (data[i*3+2] << 16);
			img.data[i] = col == TRANSPARENT_COLOR ? 0 : col;
		}
		SDL_FreeSurface(bmp);
		return ptr;
	}

	void testloop() {
		while (!quit) {			
			// SDL_UpdateWindowSurface(window);
			SDL_Delay(16);
			input();
		}
	}

	void flip(const GFX::Image& screen) {
		repaint(screen.w, screen.h, screen.data);
		input();
	}

	void flip(const GFX::Image& screen, DPad& dpad) {
		repaint(screen.w, screen.h, screen.data);
		input(dpad);
	}

	int repaint(int w, int h, const vector<uint32_t>& data) {
		// manage backbuffer
		if (buffer == NULL || buffer->w != w || buffer->h != h) {
			SDL_FreeSurface(buffer);
			buffer = SDL_CreateRGBSurface(0, w, h, 32, 0, 0, 0, 0);
			if (buffer == NULL) {
				printf( "Buffer could not be created! SDL_Error: %s\n", SDL_GetError() );
				return -1;
			}
			printf("SDL Buffer created: %d %d\n", w, h);
		}
		// paint pixels
		uint32_t* pixels = (uint32_t*)buffer->pixels;
		for (int i = 0; i < w * h; i++)
			pixels[i] = data[i];
		// flip to screen
		// int error = SDL_BlitSurface(buffer, NULL, screenSurface, NULL);
		int error = SDL_BlitScaled(buffer, NULL, screenSurface, NULL);
		if (error) {
			printf( "flip error ocurred! SDL_Error: %s\n", SDL_GetError() );
			return -1;
		}
		SDL_UpdateWindowSurface(window);
		// update FPS counter
		_framecount++;
		int current = SDL_GetTicks();
		if (current - _framelast >= 1000) {
			snprintf(fps, 10, "%.1f", _framecount / float(current - _framelast) * 1000);
			_framelast = current;
			_framecount = 0;
		}
		// wait until next frame
		frameend = SDL_GetPerformanceCounter();
		double frametime = (frameend - framestart) * 1000.0 / SDL_GetPerformanceFrequency();
		double delay = max( (1000.0 / 60) - frametime, 1.0 );  // minimum delay in case of frame skips 
		SDL_Delay(delay);
		framestart = SDL_GetPerformanceCounter();  // start of next frame
		// OK
		return 0;
	}

	int input(DPad& dpad) {
		// normalize d-pad
		dpad.normalize();
		// manage SDL events for next frame
		SDL_Event e;
		while (SDL_PollEvent(&e))
			if (e.type == SDL_QUIT)
				quit = true;
			else if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) {
				int v = e.type == SDL_KEYDOWN ? DPad::KDOWN : DPad::KUP;
				switch (e.key.keysym.sym) {
					case SDLK_ESCAPE:    quit = true;    break;
					case SDLK_UP:        dpad.u = v;     break;
					case SDLK_DOWN:      dpad.d = v;     break;
					case SDLK_LEFT:      dpad.l = v;     break;
					case SDLK_RIGHT:     dpad.r = v;     break;
					case SDLK_SPACE:
					case SDLK_z:         dpad.a = v;     break;
					case SDLK_x:         dpad.b = v;     break;
				}
			} 
		// handle d-pad axis
		dpad.setaxis();
		return 0;
	}

	int input() {
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			if      (e.type == SDL_QUIT)  quit = true;
			else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)  quit = true;
		}
		return 0;
	}
};


// emscripten extension:
// void emscripten_set_main_loop_arg(em_arg_callback_func func, void *arg, int fps, int simulate_infinite_loop)
// void callback_func(void *arg);
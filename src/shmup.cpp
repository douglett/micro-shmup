#include <iostream>
#include "global.hpp"
using namespace std;

SDLmanager sdl( "Shmup", 534, 480 );
GFX gfx;


void update() {
}

void paint() {
	gfx.fill(0xff000000);
	gfx.print(sdl.fps, 144, 1);
	sdl.flip(gfx.screen, Scene::dpad);
}


int main(int argc, char* args[]) {
	sdl.init();
	gfx.init(160, 160);
	// gfx.flag_hit = 1;

	Scene::tilesetimage = sdl.makebmp("assets/ship.bmp");
	// Scene::pimage = sdl.makebmp("assets/player.bmp");

	while (!sdl.quit) {
		update();
		paint();
	}

	sdl.close();
	return 0;
}
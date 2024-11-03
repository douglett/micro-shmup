#include <iostream>
#include "global.hpp"
#include "scene_game.hpp"
using namespace std;

SDLmanager sdl( "Shmup", 534, 480 );
GFX gfx;

SceneGame game;


void update() {
	game.update();
}

void drawall() {
	gfx.fill(0xff000000);
	game.drawscene();
	// gfx.blit( gfx.getimagegl(Scene::tilesetimage), 0, 100 );
	gfx.print(sdl.fps, 144, 1);
	sdl.flip(gfx.screen, Scene::dpad);
}


int main(int argc, char* args[]) {
	sdl.init();
	gfx.init(160, 160);
	// gfx.flag_hit = 1;

	Scene::tilesetimage = sdl.makebmp("assets/ship.bmp");

	game.init();

	while (!sdl.quit) {
		update();
		drawall();
	}

	sdl.close();
	return 0;
}
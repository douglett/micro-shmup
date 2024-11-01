#include <iostream>
#include "global.hpp"
using namespace std;

SDLmanager sdl( "Shmup", 534, 480 );
GFX gfx;


struct GameScene : Scene {
	// scene data
	GFX::Scene gfx;
	int shipspriteid = 0;
	// bullet data
	const int BULLET_CD = 7, BULLET_SPEED = 2;
	vector<int> bullets;
	int bulletcd = 0;

	void init() {
		shipspriteid = gfx.makesprite( tilesetimage, TSIZE, TSIZE );
		auto& ship = gfx.getsprite( shipspriteid );
		ship.pos.x = (SCENEW - ship.pos.w) / 2;
		ship.pos.y = SCENEH - 16;
	}

	void update() {
		// move ship
		auto& ship = gfx.getsprite( shipspriteid );
		ship.pos.x = max( 1, min( SCENEW - ship.pos.w - 1, ship.pos.x + dpad.xaxis ) );
		// move bullets
		for (int i = bullets.size() - 1; i >= 0; i--) {
			auto& bullet = gfx.getsprite( bullets[i] );
			bullet.pos.y -= BULLET_SPEED;
			if (bullet.pos.y < -TSIZE) {
				gfx.freesprite( bullets[i] );
				bullets.erase( bullets.begin() + i );
			}
		}
		// spawn bullets
		bulletcd = max( bulletcd - 1, 0 );
		if (bulletcd == 0 && dpad.a > 0) {
			// printf("spawn  %d  (%d)\n", (int)bullets.size(), gfx.pcounter);
			bullets.push_back( gfx.makesprite( tilesetimage, 4, TSIZE ) );
			auto& bullet = gfx.getsprite( bullets.back() );
			bullet.src.x = TSIZE * 2;
			bullet.pos.x = ship.pos.x + 2;
			bullet.pos.y = ship.pos.y - TSIZE - 1;
			bulletcd = BULLET_CD;
		}
	}

	void draw() {
		gfx.drawscene();
	}
};
GameScene game;


void update() {
	game.update();
}

void paint() {
	gfx.fill(0xff000000);
	game.draw();
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
		paint();
	}

	sdl.close();
	return 0;
}
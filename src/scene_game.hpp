#pragma once
#include "global.hpp"
#include "waves.hpp"
#include <vector>
#include <cmath>
using namespace std;


// User Interface
struct Interface : Scene {
	GFX::Scene gfx;
	int ifsprite = 0;

	void init() {
		ifsprite = gfx.makespriteimage( SCENEW, SCENEH );
		auto& spr = gfx.getsprite( ifsprite );
		spr.pos.x = SCENEW;
		spr.z = 1000;
		// paint ui
		gfx.fill( gfx.getimage(spr.image), 0xff000022 );
	}

	void drawscene() {
		gfx.drawscene();
	}
};


// Stars
struct StarField : Scene {
	const float STAR_SMALL_SPEED = 0.3, STAR_LARGE_SPEED = 0.6;
	GFX::Scene gfx;
	int starimageid = 0;
	vector<int> stars_small, stars_large;
	
	void init() {
		srand(100);
		// make star background image
		starimageid = gfx.makeimage( 8, 8 );
		auto& starimg = gfx.getimage( starimageid );
		// make small stars
		gfx.px( starimg, 0xff666666, 0, 0 );
		for (int i = 0; i < 100; i++) {
			int starid = gfx.makesprite( starimageid, 1, 1 );
			stars_small.push_back( starid );
			auto& spr = gfx.getsprite( starid );
			spr.src.x = 0;
			spr.pos.y = rand() % SCENEH;
			spr.pos.x = 2 + ( rand() % (SCENEW - 4) );
		}
		// make large stars
		gfx.px( starimg, 0xffffffff, 1, 0 );
		for (int i = 0; i < 15; i++) {
			int starid = gfx.makesprite( starimageid, 1, 1 );
			stars_large.push_back( starid );
			auto& spr = gfx.getsprite( starid );
			spr.src.x = 1;
			spr.pos.y = rand() % SCENEH;
			spr.pos.x = 2 + ( rand() % (SCENEW - 4) );
		}
	}

	void update() {
		for (int starid : stars_small) {
			auto& starspr = gfx.getsprite( starid );
			starspr.pos.y = fmod( starspr.pos.y + STAR_SMALL_SPEED, SCENEH );
		}
		for (int starid : stars_large) {
			auto& starspr = gfx.getsprite( starid );
			starspr.pos.y = fmod( starspr.pos.y + STAR_LARGE_SPEED, SCENEH );
		}
	}

	void drawscene() {
		gfx.drawscene();
	}
};


// Main Game scene 
struct SceneGame : Scene {
	// scene data
	GFX::Scene gfx;
	int shipspriteid = 0;
	// bullet data
	const int BULLET_CD = 7, BULLET_SPEED = 2;
	vector<int> bullets;
	int bulletcd = 0;
	// enemys
	Wave1 wave = { { gfx } };
	// effects & interface
	StarField starfield;
	Effects::Fade fade;
	Interface interface;


	void init() {
		shipspriteid = gfx.makesprite( tilesetimage, TSIZE, TSIZE );
		auto& ship = gfx.getsprite( shipspriteid );
		ship.pos.x = (SCENEW - ship.pos.w) / 2;
		ship.pos.y = SCENEH - 16;
		starfield.init();
		interface.init();
	}

	void update() {
		starfield.update();

		// move ship
		auto& ship = gfx.getsprite( shipspriteid );
		ship.z = 101;
		ship.pos.x = max( 1.0, min( SCENEW - ship.pos.w - 1.0, ship.pos.x + dpad.xaxis ) );

		// move bullets
		for (int i = bullets.size() - 1; i >= 0; i--) {
			auto& bullet = gfx.getsprite( bullets[i] );
			bullet.pos.y -= BULLET_SPEED;
			// offscreen collision
			int collide = bullet.pos.y < -TSIZE;
			// enemy collision
			if (gfx.collidesprite( bullet )) {
				collide++;
				for (int c : gfx.collisions_sprite)
					wave.kill( c );
			}
			// erase collided bullets
			if (collide) {
				gfx.freesprite( bullets[i] );
				bullets.erase( bullets.begin() + i );
			}
		}

		// spawn bullets
		bulletcd = max( bulletcd - 1, 0 );
		if (bulletcd == 0 && dpad.a > 0) {
			bullets.push_back( gfx.makesprite( tilesetimage, 4, TSIZE ) );
			auto& bullet = gfx.getsprite( bullets.back() );
			bullet.src.x = TSIZE * 2;
			bullet.pos.x = ship.pos.x + 2;
			bullet.pos.y = ship.pos.y - TSIZE - 1;
			bullet.z = 100;
			bulletcd = BULLET_CD;
		}

		// move enemys
		wave.update();
	}

	void drawscene() {
		starfield.drawscene();
		gfx.drawscene();
		interface.drawscene();
	}
};

extern SceneGame game;
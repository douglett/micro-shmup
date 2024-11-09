#pragma once
#include "global.hpp"
#include <vector>
#include <cmath>
using namespace std;


// User Interface
struct UIF : Scene {
	GFX::Scene gfx;
	int uifsprite = 0;

	void init() {
		uifsprite = gfx.makespriteimage( SCENEW, SCENEH );
		auto& spr = gfx.getsprite( uifsprite );
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
	float starsmalldy = 0, starlargedy = 0;
	
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
		starsmalldy += STAR_SMALL_SPEED;
		starlargedy += STAR_LARGE_SPEED;
		for (int starid : stars_small) {
			auto& starspr = gfx.getsprite( starid );
			starspr.pos.y = ( starspr.pos.y + int(starsmalldy) ) % SCENEH;
		}
		for (int starid : stars_large) {
			auto& starspr = gfx.getsprite( starid );
			starspr.pos.y = ( starspr.pos.y + int(starlargedy) ) % SCENEH;
		}
		starsmalldy -= int(starsmalldy);
		starlargedy -= int(starlargedy);
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
	vector<int> enemys;
	// interface
	UIF uif;
	StarField starfield;
	Effects::Fade fade;

	void init() {
		shipspriteid = gfx.makesprite( tilesetimage, TSIZE, TSIZE );
		auto& ship = gfx.getsprite( shipspriteid );
		ship.pos.x = (SCENEW - ship.pos.w) / 2;
		ship.pos.y = SCENEH - 16;
		uif.init();
		starfield.init();
	}

	void update() {
		starfield.update();

		// move ship
		auto& ship = gfx.getsprite( shipspriteid );
		ship.z = 101;
		ship.pos.x = max( 1, min( SCENEW - ship.pos.w - 1, ship.pos.x + dpad.xaxis ) );

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
					enemykill( c );
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
		for (int i = enemys.size() - 1; i >= 0; i--) {
			int enemyid = enemys[i];
			if (enemyupdate( enemyid )) {
				gfx.freesprite( enemyid );
				enemys.erase( enemys.begin() + i );
			}
		}
		// spawn new enemys
		if (enemys.size() < 10 && rand() % 10 == 0)
			enemys.push_back( enemymake_wobble() );
	}

	void drawscene() {
		starfield.drawscene();
		gfx.drawscene();
		uif.drawscene();
		// fade.fade( 0.5 );
		fade.multiply( 0xff7777 );
	}

	struct SentryData { char alive; };
	int enemymake() {
		int enemyid = gfx.makesprite( tilesetimage, TSIZE, TSIZE );
		auto& enemy = gfx.getsprite( enemyid );
		enemy.src.x = TSIZE * 4;
		enemy.pos.x = 2 + ( rand() % (Scene::SCENEW - TSIZE - 4) );
		enemy.pos.y = -TSIZE;
		// enemy data
		enemy.usertype = 1;
		// enemy.userdata.push_back( true );
		enemy.userdata.resize( sizeof(SentryData) );
		auto& data = (SentryData&)enemy.userdata[0];
		data.alive = true;
		return enemyid;
	}

	struct SentryWobbleData : SentryData { double x, y, xspeed, yspeed, xacc; };
	int enemymake_wobble() {
		int enemyid = enemymake();
		auto& enemy = gfx.getsprite( enemyid );
		// enemy data
		enemy.usertype = 2;
		enemy.userdata.resize( sizeof(SentryWobbleData) );
		auto& data = (SentryWobbleData&)enemy.userdata[0];
		data.x = enemy.pos.x;
		data.y = enemy.pos.y;
		data.yspeed = 1;
		data.xacc = 0.07;
		return enemyid;
	}

	bool enemyupdate(int enemyid) {
		auto& enemy = gfx.getsprite( enemyid );
		auto& data = (SentryData&)enemy.userdata[0];
		// straight down enemy
		if (enemy.usertype == 1) {
			enemy.pos.y += 1;
		}
		// wobble enemy
		if (enemy.usertype == 2) {
			auto& data = (SentryWobbleData&)enemy.userdata[0];
			data.xspeed += data.xacc;
			if (abs(data.xspeed) >= 1.0)  data.xacc = -data.xacc;
			data.x += data.xspeed;
			data.y += data.yspeed;
			enemy.pos.x = round(data.x);
			enemy.pos.y = round(data.y);
		}
		// check offscreen, return true if dead
		if (enemy.pos.y > SCENEH)
			data.alive = false;
		return !data.alive;
	}

	void enemykill(int enemyid) {
		auto& enemy = gfx.getsprite( enemyid );
		enemy.userdata[0] = false;
	}
};

extern SceneGame game;